/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "platform/keycodes.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"
#include "lgui/timertickevent.h"
#include "platform/font.h"
#include "textfield.h"
#include "platform/graphics.h"
#include "lgui/style/style.h"

#include "platform/utf8.h"

namespace lgui {

    TextField::TextField(const std::string& initial_text, const Font* font)
        : mtext(initial_text), mcursor_pos(0), msel_anchor(std::string::npos),
          mcursor_pos_px(0), mscroll_pos_px(0), mmax_length(32767),
          mdouble_clicked(false),
          mlast_clicked_timestamp(0.0), mlast_pressed_timestamp(0.0),
          mcursor_width(style().text_field_cursor_width()),
          mvalidation_enabled(false)
    {
        auto padd = style().text_field_padding();
        mpadding = padd.widget;
        mtext_margins = padd.text_margins;
        if(font)
            Widget::set_font(font);
        // set some default size
        //set_size(mpadding.horz()+Widget::font().text_width("ABCDEFG")+mcursor_width,
        //         Widget::font().line_height() + mpadding.vert());
        set_clipped(true);
        set_focusable(true);
        set_may_tab_into(true);
        set_may_tab_out_of(true);
    }

    void TextField::key_char(KeyEvent& event)
    {
        bool consume = true;
        bool sel = event.modifiers() & KeyModifiers::KEYMOD_SHIFT;
        bool ctrl = event.modifiers() & KeyModifiers::KEYMOD_CTRL;
        bool removed_sel = false;
        bool deleted_sth = false;
        bool changed = false;

        if(is_move_key(event.key_code())) {
            // deal with moving the cursor leading to clearing the selection
            if(sel && msel_anchor == std::string::npos)
                msel_anchor = mcursor_pos;
            else if(!sel && msel_anchor != std::string::npos) {
                bool anchor_on_right = msel_anchor > mcursor_pos;
                if((event.key_code() == Keycodes::KEY_RIGHT && anchor_on_right) ||
                   (event.key_code() == Keycodes::KEY_LEFT && !anchor_on_right)) {
                    mcursor_pos = msel_anchor;
                }
                removed_sel = true;
                msel_anchor = std::string::npos;
            }
        }
        switch(event.key_code()) {
            case Keycodes::KEY_LEFT:
                if(ctrl)
                    mcursor_pos = find_next_word_boundary(true);
                else if(!removed_sel) {
                    utf8::prev_cp(mtext, mcursor_pos);
                }
                break;
            case Keycodes::KEY_RIGHT:
                if(ctrl)
                    mcursor_pos = find_next_word_boundary(false);
                else if(!removed_sel) {
                    utf8::next_cp(mtext, mcursor_pos);
                }
                break;
            case Keycodes::KEY_HOME:
                mcursor_pos = 0;
                break;
            case Keycodes::KEY_END:
                mcursor_pos = mtext.size();
                break;
            case Keycodes::KEY_DELETE:
                if(msel_anchor != std::string::npos) {
                    if (may_replace_selection(nullptr)) {
                        delete_selection();
                        deleted_sth = true;
                    }
                }
                else {
                    if (may_delete_char(mcursor_pos)) {
                        delete_char();
                        deleted_sth = true;
                    }
                }
                break;
            case Keycodes::KEY_BACKSPACE:
                if(msel_anchor != std::string::npos) {
                    if (may_replace_selection(nullptr)) {
                        delete_selection();
                        deleted_sth = true;
                    }
                }
                else {
                    size_t cursor_back = mcursor_pos;
                    if(utf8::prev_cp(mtext, cursor_back)) {
                        if (may_delete_char(cursor_back))  {
                            mcursor_pos = cursor_back;
                            delete_char();
                            deleted_sth = true;
                        }
                    }
                }
                break;
            case Keycodes::KEY_A:
                if(ctrl) {
                    if(!event.repeated()) {
                        select_all();
                    }
                    break;
                }
            // FALLTHRU
            default:
            {
                int c = event.unichar();
                if(c >= ' ' && is_char_insertable(c)) {
                    if(msel_anchor != std::string::npos) {
                        std::string chr;
                        utf8::append_chr(chr, c);
                        if (may_replace_selection(&chr)) {
                            delete_selection();
                            insert_char(c);
                            changed = true;
                        }
                    }
                    else {
                        if(utf8::length_cps(mtext) < mmax_length) {
                            if (may_insert_char(c)) {
                                insert_char(c);
                                changed = true;
                            }
                        }
                    }
                }
                else
                    consume = false;
                break;
            }
        }
        if(consume)
            event.consume();
        maybe_scroll(deleted_sth);
        changed = changed || deleted_sth;
        if(changed)
            on_text_changed.emit(mtext);
    }

    void TextField::key_pressed(KeyEvent& event)
    {
        if(event.key_code() == Keycodes::KEY_ENTER ||
           event.key_code() == Keycodes::KEY_ENTER_PAD) {
            on_activated.emit(mtext);
            event.consume();
        }
    }

    // cursor_pos may have changed, the rest needs to be adapted
    void TextField::maybe_scroll(bool deleted_something)
    {
        const int W = width() - mpadding.horz() - mcursor_width;

        int old_cursor_x = mcursor_pos_px - mscroll_pos_px;
        update_cursor_pos_px();
        int cursor_x = mcursor_pos_px - mscroll_pos_px;

        if(deleted_something) {
            // try to keep the cursor at the same position
            mscroll_pos_px = mcursor_pos_px - old_cursor_x;
            // clip end to right (use all the width)
            int end_px = font().text_width(mtext)+mtext_margins.horz();
            if(end_px - mscroll_pos_px < W)
                mscroll_pos_px = end_px-W;
            // clip beginning to left (more important)
            if(mscroll_pos_px < 0)
                mscroll_pos_px = 0;
            // changed, update
            cursor_x = mcursor_pos_px - mscroll_pos_px;
            // return?
        }
        if(cursor_x < 0) {
            mscroll_pos_px = mcursor_pos_px;
            if(mcursor_pos == 0) // show left text margin
                mscroll_pos_px = 0;
        }
        else {
            // show right text margin
            if(mcursor_pos == mtext.size() && cursor_x+mtext_margins.right() >= W)
                mscroll_pos_px = mcursor_pos_px+mtext_margins.right()-W;
            else if(cursor_x >= W) {
                mscroll_pos_px = mcursor_pos_px-W;
            }
        }
    }

    // Works on text only, not taking padding into account.
    // Could use binary search here, currently not done due to utf8.
    int TextField::get_char_pos(int px) const
    {
        return font().hit_char(mtext, px).first;
    }

    bool TextField::is_move_key(int key)
    {
        switch(key) {
            case Keycodes::KEY_LEFT:
            case Keycodes::KEY_RIGHT:
            case Keycodes::KEY_HOME:
            case Keycodes::KEY_END:
                return true;
            default:
                return false;
        }
    }

    size_t TextField::find_next_word_boundary(bool backwards)
    {
        return utf8::skip_to_next_word_boundary(mtext, mcursor_pos, backwards);
    }

    void TextField::mouse_pressed(MouseEvent& event)
    {
        if(!has_focus()) {
            focus();
        }
        int y = event.y();
        if(y >= mpadding.top() && y <= font().line_height() + mpadding.vert()) {
            int cpos = get_char_pos(event.x() - mpadding.left() + mscroll_pos_px - mtext_margins.left());
            if(cpos >= 0) {
                mcursor_pos = cpos;
                update_cursor_pos_px();
                msel_anchor = cpos; // FIXME: set here? enables drag selection
            }
        }
        mlast_pressed_timestamp = event.timestamp();
        event.consume();
    }

    void TextField::mouse_dragged(MouseEvent& event)
    {
        int cpos = get_char_pos(event.x() - mpadding.left() + mscroll_pos_px - mtext_margins.left());
        if(cpos >= 0) {
            mcursor_pos = cpos;
            maybe_scroll(); // this will update mcursor_pos_px
        }
        event.consume();
    }

    void TextField::mouse_released(MouseEvent& event)
    {
        if(mcursor_pos == msel_anchor)
            msel_anchor = std::string::npos; // no selection formed, reset
        event.consume();
    }

    void TextField::mouse_clicked(MouseEvent& event)
    {
        double dt = event.timestamp()-mlast_pressed_timestamp;
        double dc = event.timestamp()-mlast_clicked_timestamp;
        if(dt < mclick_intervall && dc < mclick_intervall) {
            double_clicked();
        }
        else
            mdouble_clicked = false;
        mlast_clicked_timestamp = event.timestamp();
        event.consume();
    }

    void TextField::double_clicked()
    {
        int s = 0, e = mtext.size();
        if(mdouble_clicked)
            mdouble_clicked = false;
        else {
            mdouble_clicked = true;
            s = find_next_word_boundary(true),
            e = find_next_word_boundary(false);
        }
        if(!mtext.empty()) {
            msel_anchor = s;
            mcursor_pos = e;
            maybe_scroll();
        }
    }

    void TextField::style_changed()
    {
        auto padd = style().text_field_padding();
        mpadding = padd.widget;
        mtext_margins = padd.text_margins;
        mcursor_width = style().text_field_cursor_width();
        request_layout();
    }

    void TextField::resized(const Size& old_size)
    {
        (void) old_size;
        maybe_scroll(true);
    }

    void TextField::timer_ticked(const TimerTickEvent& event)
    {
        mcursor_blink_helper.timer_tick(event);
    }

    void TextField::focus_gained(FocusEvent& event)
    {
        (void) event;
        set_receive_timer_ticks(true);
    }

    void TextField::focus_lost(FocusEvent& event)
    {
        (void) event;
        set_receive_timer_ticks(false);
    }

    bool TextField::is_char_insertable(int c) const
    {
        (void) c;
        return true;
    }

    bool TextField::validate(const std::string& s) const
    {
        if (mvalidator)
            return mvalidator(s);
        return true;
    }

    void TextField::delete_char() {
        utf8::remove_chr(mtext, mcursor_pos);
    }

    void TextField::insert_char(int c) {
        mcursor_pos += utf8::insert_chr(mtext, mcursor_pos, c);
    }


    void TextField::delete_selection()
    {
        if(msel_anchor != std::string::npos) {
            int start = msel_anchor, end = mcursor_pos;
            if(end < start)
                std::swap(start, end);
            mtext.erase(start, end-start);
            mcursor_pos = start;
            if(mcursor_pos > mtext.size())
                mcursor_pos = mtext.size();
            msel_anchor = std::string::npos;
            maybe_scroll(true);
        }
    }

    bool TextField::may_delete_char(size_t pos) const
    {
        if (!mvalidation_enabled)
            return true;
        std::string new_text = mtext;
        utf8::remove_chr(new_text, pos);
        return validate(new_text);
    }

    bool TextField::may_replace_selection(const std::string* repl) const
    {
        if (!mvalidation_enabled)
            return true;
        if(msel_anchor != std::string::npos) {
            std::string new_text = mtext;
            int start = msel_anchor, end = mcursor_pos;
            if(end < start)
                std::swap(start, end);
            new_text.erase(start, end-start);
            size_t cursor_pos = start;
            if(cursor_pos > mtext.size())
                cursor_pos = mtext.size();
            if (repl && !repl->empty()) {
                new_text.insert(cursor_pos, *repl);
            }
            return validate(new_text);
        }
        return true;
    }

    bool TextField::may_insert_char(int c) const
    {
        if (!mvalidation_enabled)
            return true;
        std::string new_text = mtext;
        utf8::insert_chr(new_text, mcursor_pos, c);
        return validate(new_text);
    }

    void TextField::update_cursor_pos_px()
    {
        std::string before_cursor = mtext.substr(0, mcursor_pos);
        mcursor_pos_px = font().text_width(before_cursor) + mtext_margins.left();
    }

    void TextField::draw(const DrawEvent& de) const
    {
       StyleArgs style_args(*this, de);

       style().draw_text_field_bg(de.gfx(), style_args);

       int x = mpadding.left();
       int y = mpadding.top();

       de.gfx().push_draw_area(lgui::Rect(x, y, mpadding.sub(size())), true);

       int cursor_x = mcursor_pos_px - mscroll_pos_px;

       // look for sel_anchor start and draw selection
       if(msel_anchor != std::string::npos && msel_anchor != mcursor_pos) {
           std::string sub = mtext.substr(0, msel_anchor);
           int sel_anchor_x = font().text_width(sub) - mscroll_pos_px + mtext_margins.left();
           int sel_x1 = cursor_x, sel_x2 = sel_anchor_x;
           if(sel_x1 > sel_x2)
               std::swap(sel_x1, sel_x2);
           de.gfx().filled_rect(sel_x1, 0, sel_x2, font().line_height(),
                           style().text_field_selection_color(style_args.state, style_args.opacity));
       }

       // Draw cursor.

       if(has_focus() && mcursor_blink_helper.blink_status())
           de.gfx().filled_rect(cursor_x, 0, cursor_x+mcursor_width, font().line_height(),
                           style().text_field_cursor_color(style_args.state, style_args.opacity));

       // TODO: maybe draw only parts of it (remember mleft_pos?)
       // clip text within padding
       de.gfx().draw_text(font(), -mscroll_pos_px+mtext_margins.left(), 0,
                     style().text_field_text_color(style_args.state, style_args.opacity), mtext);

       de.gfx().pop_draw_area();

       style().draw_text_field_fg(de.gfx(), style_args);
    }

    void TextField::set_text(const std::string& text, bool sel_all)
    {
        mtext = text;
        mcursor_pos = mtext.size();
        // we pretend the text has been entered (TODO: disable this)
        mcursor_pos_px = mscroll_pos_px = 0;
        maybe_scroll();
        if(sel_all && !mtext.empty())
            msel_anchor = 0;
        else
            msel_anchor = std::string::npos;
        on_text_changed.emit(mtext);
    }

    void TextField::select_none()
    {
        msel_anchor = std::string::npos;
    }

    void TextField::select_all()
    {
        if(!mtext.empty()) {
            msel_anchor = 0;
            mcursor_pos = mtext.size();
            maybe_scroll();
        }
    }

    void TextField::set_cursor_position(size_t pos)
    {
        mcursor_pos = std::min(pos, mtext.size());
        maybe_scroll();
    }

    void TextField::select(size_t anchor, size_t cursor)
    {
        if(!mtext.empty()) {
            msel_anchor = (anchor != std::string::npos) ?
                          std::min(anchor, mtext.size()) :
                          std::string::npos;
            mcursor_pos = std::min(cursor, mtext.size());
            maybe_scroll();
        }
    }

    void TextField::set_max_length(size_t length)
    {
        // have some limit
        if(length >= 65535)
            length = 65535;
        mmax_length = length;
        if(mtext.length() >= length) {
            set_text(mtext.substr(0, length), false);
        }
    }

    void TextField::set_validator(const std::function<bool (const std::string&)>& validator)
    {
        mvalidator = validator;
        set_validation_enabled(mvalidator != nullptr);
    }

    void TextField::set_size(Size s)
    {
        Widget::set_size(lgui::Size(s.w(), font().line_height() + mpadding.vert()));
    }

    void TextField::set_font(const Font* font)
    {
        Widget::set_font(font);
        style_changed();
    }

    Size TextField::min_size_hint()
    {
        // FIXME: makes sense?
        return Size(mpadding.add(Size(font().char_width_hint(), font().line_height())));
    }

    MeasureResults TextField::measure(SizeConstraint wc, SizeConstraint hc)
    {
        int text_w = mmax_length*font().char_width_hint();
        int text_h = font().line_height();
        return force_size_constraints(mpadding.add(Size(text_w, text_h)), wc, hc);
    }

}

