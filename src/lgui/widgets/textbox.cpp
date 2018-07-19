/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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
#include "textbox.h"
#include "platform/font.h"
#include "platform/utf8.h"
#include "platform/graphics.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"
#include "lgui/style/style.h"


namespace lgui {

    const int MIN_WIDTH_CWH = 6, MIN_HEIGHT_LINES = 3;
    const int WANT_WIDTH_CWH = 60, WANT_HEIGHT_LINES = 20;

    TextBox::TextBox(const std::string& initial_text, const Font* font)
    : mtext(initial_text),
      mhorz_scrollbar(Horizontal),
      mvert_scrollbar(Vertical),
      mscroll(0, 0),
      manchor_rowcol(-1, -1),
      manchor_tpx(-1, -1), mmax_line_width(0),
      mwrap_mode(WrapMode::FittingWords),
      mread_only(false), mcursor_blinking_status(true)
    {
        mhorz_scrollbar.on_scrolled.connect(&TextBox::x_scrolled, *this);
        mvert_scrollbar.on_scrolled.connect(&TextBox::y_scrolled, *this);
        mhorz_scrollbar.set_outside_children_area(true);
        mvert_scrollbar.set_outside_children_area(true);
        mhorz_scrollbar.set_invisible();
        mvert_scrollbar.set_invisible();
        add_private_child(mhorz_scrollbar);
        add_private_child(mvert_scrollbar);

        style_changed(); // initialize padding etc.
        if(font)
            Widget::set_font(font);
        set_clipped(true);
        set_focusable(true);
        set_may_tab_into(true);
        set_may_tab_out_of(true);
        set_receive_timer_ticks(true);
        set_timer_tick_skip_mod(30);
        mcursor_width = style().text_field_cursor_width();
    }

    void TextBox::draw(const DrawEvent& de) const
    {
        // clipped contents

        Rect ca(mpadding.left(), mpadding.top(), width_available(), height_available());

        StyleArgs style_args(*this, de);

        style().draw_text_box_bg(de.gfx(), style_args);

        de.gfx().push_draw_area(ca, true);

        int x = -mscroll.x() + mtext_margins.left();
        int sy = -mscroll.y() + mtext_margins.top();

        int start_line = -sy / line_height();
        sy += start_line * line_height();
        int end_line = std::min(start_line+ca.h() / line_height() + 2, signed(mtext_lines.size()));

        int y = sy;

        if(!mselection_tpx.empty()) {
            int sel_start_line = std::min(manchor_rowcol.y(), mcaret_rowcol.y());
            int sel_end_line = std::max(manchor_rowcol.y(), mcaret_rowcol.y())+1;

            int draw_sel_start = std::max(sel_start_line, start_line);
            int draw_sel_end = std::min(sel_end_line, end_line);

            for(int i = draw_sel_start; i < draw_sel_end; i++) {
                Rect r = mselection_tpx.at(i - sel_start_line);
                r.translate(-mscroll + mtext_margins.left_top_offs());
                de.gfx().filled_rect(r, style().text_field_selection_color(style_args.state, de.opacity()));
            }
        }
        for(int i = start_line; i < end_line; i++) {
            de.gfx().draw_text(font(), x, y,
                               style().text_field_text_color(style_args.state, de.opacity()),
                               mtext_lines[i]);
            y+= line_height();
        }

        if(has_focus() && mcursor_blinking_status) {
            Rect caret_rect(mcaret_tpx - mscroll + mtext_margins.left_top_offs(), Size(mcursor_width, line_height()));
            de.gfx().filled_rect(caret_rect,
                                 style().text_field_cursor_color(style_args.state, style_args.opacity));
        }

        de.gfx().pop_draw_area(); // /clipped contents

        style().draw_text_box_fg(de.gfx(), style_args);

        draw_private_children(de, false);

        //gfx.filled_rect(size_rect(), rgb(0.0, 0.0, 1.0));
    }

    void TextBox::set_text(const std::string& text)
    {
        mtext = text;
        update_rows();
        on_text_changed.emit(mtext);
    }

    void TextBox::set_font(const Font* font)
    {
        Widget::set_font(font);
        // react
    }

    void TextBox::select_none()
    {
        manchor_rowcol = manchor_tpx = Point(-1, -1);
        mselection_tpx.clear();
    }

    void TextBox::select_all()
    {
        if(mtext.empty() || mtext_lines.empty()) {
            select_none();
            return;
        }
        manchor_rowcol = Point(0, 0);
        mcaret_rowcol = Point(mtext_lines_chrlen.back(),
                              mtext_lines.size()-1);
        update_caret_tpx_location();
        update_selection();
        scroll_to_caret();
    }

    void TextBox::set_read_only(bool read_only)
    {
        mread_only = read_only;
    }

    void TextBox::style_changed()
    {
        WidgetPC::style_changed();
        // do something
        mcursor_width = style().text_field_cursor_width();

        auto padd = style().text_box_padding();
        mpadding = padd.widget;
        mscrollbar_padding = padd.scroll_bars;
        mtext_margins = padd.text_margins;
    }

    void TextBox::resized(const Size& old_size)
    {
        (void) old_size;

        // Preserve cursor position and selection
        size_t anchor = rowcol_to_textoffs(manchor_rowcol);
        size_t caret = rowcol_to_textoffs(mcaret_rowcol);
        update_rows();
        mcaret_rowcol = texoffs_to_rowcol(caret);
        update_caret_tpx_location();
        if(anchor != std::string::npos) {
            manchor_rowcol = texoffs_to_rowcol(anchor);
            manchor_tpx = rowcol_to_tpx(manchor_rowcol);
            update_selection();
        }
    }

    bool TextBox::update_caret_position_from_mouse(Point mp, bool clip_outside_margins)
    {
        Point tpx = mp - mpadding.left_top_offs() + mscroll;
        if(!clip_outside_margins) {
            if(tpx.x() < 0 ||
               tpx.y() < 0 ||
               tpx.x() >= text_dims().w() ||
               tpx.y() >= text_dims().h())
                return false;
        }
        tpx -= mtext_margins.left_top_offs();
        Rect(0, 0, mmax_line_width+mcursor_width, line_height()*mtext_lines.size())
                .clip_point(tpx);
        Point rc = tpx_to_rowcol(tpx, true);
        if(rc.x() >= 0 && rc.y() >= 0) {
            mcaret_rowcol = rc;
            update_caret_tpx_location();
            return true;
        }
        return false;
    }


    void TextBox::mouse_pressed(MouseEvent& event)
    {
        if(update_caret_position_from_mouse(event.pos(), true)) {
            manchor_tpx = mcaret_tpx;
            manchor_rowcol = mcaret_rowcol;
            scroll_to_caret();
            update_selection();
        }
        focus();
        event.consume();
    }

    void TextBox::mouse_released(MouseEvent& event)
    {
        (void) event;
        // clear selection if none
        if(manchor_rowcol == mcaret_rowcol)
            manchor_rowcol = manchor_tpx = Point(-1, -1);
    }

    void TextBox::mouse_dragged(MouseEvent& event)
    {
        if(update_caret_position_from_mouse(event.pos(), true)) {
            scroll_to_caret();
            update_selection();
        }
        event.consume();
    }

    void TextBox::mouse_wheel_down(MouseEvent& event)
    {
        if(mvert_scrollbar.is_visible())
            mvert_scrollbar.send_mouse_event(event);
    }

    void TextBox::mouse_wheel_up(MouseEvent& event)
    {
        if(mvert_scrollbar.is_visible())
            mvert_scrollbar.send_mouse_event(event);
    }

    void TextBox::timer_ticked(const TimerTickEvent& event)
    {
        (void) event;
        mcursor_blinking_status = !mcursor_blinking_status;
    }


    void TextBox::key_char(KeyEvent& event)
    {
        if(!event.repeated() && keyboard_hotkeys(event)) {
            event.consume();
            return;
        }
        if(is_move_key(event.key_code()) && move_caret_keyboard(event)) {
            event.consume();
            return;
        }
        if(event.key_code() == Keycodes::KEY_BACKSPACE) {
            if(has_selection())
                remove_selection();
            else
                remove_chr(true);
            event.consume();
        }
        else if(event.key_code() == Keycodes::KEY_DELETE ||
                event.key_code() == Keycodes::KEY_PAD_DELETE) {
            if(has_selection())
                remove_selection();
            else
                remove_chr(false);
            event.consume();
        }
        else {
            if(!(event.modifiers() & KeyModifiers::KEYMOD_CTRL)) {
                int c = event.unichar();
                if(c >= ' ' || c == '\r') {
                    if(c == '\r')
                        c = '\n';
                    if(has_selection())
                        remove_selection();
                    type_chr(c);
                    event.consume();
                }
            }
        }
    }

    void TextBox::do_update_rows()
    {
        mtext_lines.clear();
        mtext_lines_chrlen.clear();
        mtext_line_split_cwidth.clear();
        if(mwrap_mode == WrapMode::None)
            make_rows_newlines(mtext);
        else
            make_rows_wordwrap(mtext, width_available()-mcursor_width-mtext_margins.horz(),
                               mwrap_mode);
    }

    void TextBox::update_rows(bool second_pass)
    {
        do_update_rows();
        if(mtext_lines.empty()) {
            mvert_scrollbar.set_invisible();
            mhorz_scrollbar.set_invisible();
            mscroll = Point(0, 0);
            return;
        }

        bool had_v_scrollb = mvert_scrollbar.is_visible();
        bool had_h_scroolb = mhorz_scrollbar.is_visible();

        Size text_size = text_dims();

        bool need_v_scrollb = text_size.h() > height_available();
        bool need_h_scrollb = text_size.w() > width_available();


        if(had_v_scrollb && !need_v_scrollb) {
            mvert_scrollbar.set_invisible();
            mscroll.set_y(0);
            // do again, since more width available
            do_update_rows();
            text_size = text_dims();
            need_h_scrollb = text_size.w() > width_available();
        }
        else if(!had_v_scrollb && need_v_scrollb) {
            mvert_scrollbar.setup(text_size.h(),
                                  height_available(), height_available()+mpadding.vert()-mscrollbar_padding.vert(),
                                  mscroll.y(), line_height());
            mvert_scrollbar.set_visible(); // enable consideration for width
            // do again, since less width available
            do_update_rows();
            text_size = text_dims();
            need_h_scrollb = text_size.w() > width_available();
        }


        if(need_h_scrollb) {
            // need scrollbar?
            mhorz_scrollbar.setup(text_size.w(),
                                  width_available(), width_available()+mpadding.horz()-mscrollbar_padding.horz(),
                                  mscroll.x(), line_height());
            mhorz_scrollbar.set_visible();
        }
        else {
            mhorz_scrollbar.set_invisible();
            mscroll.set_x(0);
        }

        bool recheck_vbar = mwrap_mode == WholeWords &&
                            !second_pass &&
                            need_h_scrollb != had_h_scroolb &&
                            need_v_scrollb == had_v_scrollb;

        if(recheck_vbar)
            update_rows(true);

        if(need_v_scrollb) {
            // readjust
            mvert_scrollbar.setup(text_size.h(),
                                  height_available(), height_available()+mpadding.vert()-mscrollbar_padding.vert(),
                                  mscroll.y(), line_height());
        }
        position_scrollbars();
    }

    int TextBox::width_available() const
    {
        int cw = width() - mpadding.horz();
        if(mvert_scrollbar.is_visible())
            cw -= mvert_scrollbar.width();
        return cw;
    }

    int TextBox::height_available() const
    {
        int ch = height() - mpadding.vert();
        if(mhorz_scrollbar.is_visible())
            ch -= mhorz_scrollbar.height();
        return ch;
    }

    static bool is_smaller(Point rc1, Point rc2) {
        if(rc1.y() < rc2.y())
            return true;
        else if(rc1.y() > rc2.y())
            return false;
        else {
            return rc1.x() < rc2.x();
        }
    }

    bool TextBox::move_caret_keyboard(const KeyEvent& ke)
    {
        // we already know ke represents a "move key"
        bool ctrl = ke.modifiers() & KeyModifiers::KEYMOD_CTRL;;
        bool no_further_reaction = false;
        bool sel = ke.modifiers() & KeyModifiers::KEYMOD_SHIFT;
        if(sel && !has_selection()) {
            manchor_rowcol = mcaret_rowcol;
            manchor_tpx = mcaret_tpx;
        }
        else if(!sel && has_selection()) {
            if(mcaret_rowcol != manchor_rowcol &&
               (ke.key_code() == Keycodes::KEY_RIGHT ||
                ke.key_code() == Keycodes::KEY_LEFT)) {
                bool caret_first = is_smaller(mcaret_rowcol, manchor_rowcol);
                if((ke.key_code() == Keycodes::KEY_RIGHT && caret_first) ||
                   (ke.key_code() == Keycodes::KEY_LEFT && !caret_first)) {
                    mcaret_rowcol = manchor_rowcol;
                    update_caret_tpx_location();
                }
                no_further_reaction = true;
            }
            select_none();
        }
        if(no_further_reaction)
            return true;
        switch(ke.key_code()) {
            case Keycodes::KEY_LEFT:
                if(!ctrl)
                    position_caret_keyboard(mcaret_rowcol+Point(-1, 0), PosColMode::Wrap);
                else {
                    size_t caret_offs = rowcol_to_textoffs(mcaret_rowcol);
                    size_t nc = utf8::skip_to_next_word_boundary(mtext, caret_offs, true);
                    position_caret_keyboard(texoffs_to_rowcol(nc), PosColMode::Fixed);
                }
                return true;
            case Keycodes::KEY_RIGHT:
                if(!ctrl)
                    position_caret_keyboard(mcaret_rowcol+Point(1, 0), PosColMode::Wrap);
                else {
                    size_t caret_offs = rowcol_to_textoffs(mcaret_rowcol);
                    size_t nc = utf8::skip_to_next_word_boundary(mtext, caret_offs, false);
                    position_caret_keyboard(texoffs_to_rowcol(nc), PosColMode::Fixed);
                }
                return true;
            case Keycodes::KEY_UP:
                position_caret_keyboard(mcaret_rowcol+Point(0, -1), PosColMode::Search);
                return true;
            case Keycodes::KEY_DOWN:
                position_caret_keyboard(mcaret_rowcol+Point(0, 1), PosColMode::Search);
                return true;
            case Keycodes::KEY_HOME:
                if(ke.modifiers() & KeyModifiers::KEYMOD_CTRL)
                    position_caret_keyboard(Point(0, 0), PosColMode::Fixed);
                else
                    position_caret_keyboard(Point(0, mcaret_rowcol.y()),
                                            PosColMode::Fixed);
                return true;
            case Keycodes::KEY_END:
                if(!mtext_lines.empty()) {
                    if(ke.modifiers() & KeyModifiers::KEYMOD_CTRL)
                        position_caret_keyboard(Point(mtext_lines_chrlen.back(), mtext_lines.size()-1),
                                                PosColMode::Fixed);
                    else
                        position_caret_keyboard(Point(mtext_lines_chrlen[mcaret_rowcol.y()], mcaret_rowcol.y()),
                                PosColMode::Fixed);
                }
                return true;
            default:
                return false;
        }
    }

    bool TextBox::keyboard_hotkeys(const KeyEvent& ke)
    {
        if(ke.key_code() == Keycodes::KEY_A &&
                (ke.modifiers() & KeyModifiers::KEYMOD_CTRL)) {
            select_all();
            return true;
        }
        return false;
    }

    void TextBox::position_caret_keyboard(Point nrc, PosColMode pcm)
    {
        //debug("\nloc-in : %d, %d", nrc.x(), nrc.y());
        if(!mtext_lines.empty()) {
            if(nrc.y() < 0)
                nrc.set_y(0);
            else if(nrc.y() >= signed(mtext_lines.size()))
                nrc.set_y(signed(mtext_lines.size()-1));

            if(pcm == PosColMode::Fixed)
                mcaret_rowcol = clip_row_col(nrc);
            else if(pcm == PosColMode::Search) {
                if(nrc.y() != mcaret_rowcol.y()) {
                    int px = mcaret_tpx.x();
                    int new_col = font().hit_char(mtext_lines[nrc.y()], px).second;
                    nrc.set_x(new_col);
                }
            }
            else { // Wrap
                bool last_char_access = mtext_line_split_cwidth[nrc.y()];
                if(nrc.x() >= mtext_lines_chrlen[nrc.y()] + (last_char_access ? 1 : 0)) {
                    if(nrc.y() < signed(mtext_lines.size())-1) {
                        nrc.set_y(nrc.y()+1);
                        nrc.set_x(0);
                    }
                    else // last line: can always access last position
                        nrc.set_x(mtext_lines_chrlen.back());
                }
                else if(nrc.x() < 0) {
                    if(nrc.y() > 0) {
                        nrc.set_y(nrc.y()-1);
                        // y changed here:
                        bool last_char_access_ny = mtext_line_split_cwidth[nrc.y()];
                        nrc.set_x(mtext_lines_chrlen[nrc.y()]
                                - (last_char_access_ny ? 0 : 1));

                    }
                    else
                        nrc.set_x(0);
                }
            }
        }
        else
            nrc = Point(0, 0);

        //debug("\nloc-out: %d, %d  (", nrc.x(), nrc.y());
        mcaret_rowcol = nrc;
        update_caret_tpx_location();
        if(has_selection())
            update_selection();
        scroll_to_caret();
    }


    void TextBox::update_caret_tpx_location()
    {
        mcaret_tpx = rowcol_to_tpx(mcaret_rowcol);
    }

    void TextBox::update_selection()
    {
        mselection_tpx.clear();
        if(mtext_lines.empty() || !has_selection() ||
                mcaret_rowcol == manchor_rowcol) {
            return;
        }

        Point start = mcaret_rowcol;
        Point end = manchor_rowcol;
        Point start_tpx = mcaret_tpx;
        Point end_tpx = manchor_tpx;

        if(is_smaller(end, start)) {
            std::swap(start, end);
            std::swap(start_tpx, end_tpx);
        }

        Point spx = start_tpx;

        for(int y = start.y(); y < end.y(); y++) {
            int epx = text_width(mtext_lines[y]);
            // empty lines symbolised by space
            if(epx <= 0)
                epx = text_width(" ");
            int ly = line_height()*y;
            mselection_tpx.emplace_back(Rect(Point(spx.x(), ly), Size(epx - spx.x(), line_height())));
            spx = Point(0, ly+line_height());
        }
        mselection_tpx.emplace_back(Rect(spx, Size(end_tpx.x() - spx.x(), line_height())));
    }

    void TextBox::remove_selection()
    {
        if(is_read_only() || !has_selection())
            return;
        size_t a = rowcol_to_textoffs(manchor_rowcol);
        size_t c = rowcol_to_textoffs(mcaret_rowcol);
        if(c == a)
            return;
        if(c < a)
            std::swap(a, c);
        mtext.erase(mtext.begin()+a, mtext.begin()+c);
        select_none();
        update_rows();
        mcaret_rowcol = texoffs_to_rowcol(a);
        update_caret_tpx_location();
        scroll_to_caret();
        on_text_changed.emit(mtext);
    }

    bool TextBox::remove_chr(bool backspace)
    {
        if(is_read_only())
            return false;
        size_t c = rowcol_to_textoffs(mcaret_rowcol);
        if(backspace) {
            if(!utf8::prev_cp(mtext, c))
                return false;
        }
        bool success = utf8::remove_chr(mtext, c);
        if(success) {
            update_rows();
            mcaret_rowcol = texoffs_to_rowcol(c);
            update_caret_tpx_location();
            scroll_to_caret();
            on_text_changed.emit(mtext);
        }
        return success;
    }

    int TextBox::type_chr(int c)
    {
        if(is_read_only())
            return 0;
        size_t caret = rowcol_to_textoffs(mcaret_rowcol);
        int cw = utf8::insert_chr(mtext, caret, c);
        caret += cw;
        update_rows();
        mcaret_rowcol = texoffs_to_rowcol(caret);
        update_caret_tpx_location();
        scroll_to_caret();
        on_text_changed.emit(mtext);
        return cw;
    }

    void TextBox::scroll_to_caret()
    {
        // FIXME: find better solutions?
        const int rscroll_ahead = std::max(font().char_width_hint(), mtext_margins.horz() / 2);
        const int lscroll_ahead = std::max(mtext_margins.horz() / 2 - font().char_width_hint(), 1);

        if(mvert_scrollbar.is_visible()) {
            const int cursor_height = line_height();
            if(mcaret_tpx.y()+mtext_margins.top() < mscroll.y()) {
                mvert_scrollbar.scroll_to(mcaret_tpx.y() + mtext_margins.top(), true);
            }
            else if(mcaret_tpx.y()+mtext_margins.top() + cursor_height >
                    mscroll.y() + height_available()) {
                mvert_scrollbar.scroll_to(mcaret_tpx.y() + mtext_margins.top() + cursor_height -
                                          height_available(), true);
            }
        }
        if(mhorz_scrollbar.is_visible()) {
            if(mcaret_tpx.x()+mtext_margins.left() - lscroll_ahead < mscroll.x()) {
                mhorz_scrollbar.scroll_to(mcaret_tpx.x() + mtext_margins.left() - lscroll_ahead, true);
            }
            else if(mcaret_tpx.x() + mtext_margins.left() + mcursor_width + rscroll_ahead >
                    mscroll.x() + width_available()) {
                mhorz_scrollbar.scroll_to(mcaret_tpx.x() + mtext_margins.left() + mcursor_width + rscroll_ahead
                                          - width_available(), true);
            }
        }
    }

    bool TextBox::has_selection() const
    {
        return (manchor_rowcol.x() >= 0 && manchor_rowcol.y() >= 0);
    }

    bool TextBox::is_move_key(int key)
    {
        switch(key) {
            case Keycodes::KEY_LEFT:
            case Keycodes::KEY_RIGHT:
            case Keycodes::KEY_UP:
            case Keycodes::KEY_DOWN:
            case Keycodes::KEY_HOME:
            case Keycodes::KEY_END:
                return true;
            default:
                return false;
        }
    }


    void TextBox::add_row(const std::string& text, size_t offs, size_t size, int split_c_w)
    {
        std::string line = text.substr(offs, size);
        mtext_lines.emplace_back(line);
        int w = text_width(mtext_lines.back());
        mmax_line_width = std::max(mmax_line_width, w);
        mtext_line_split_cwidth.emplace_back(split_c_w);
        mtext_lines_chrlen.emplace_back(utf8::length_cps(mtext_lines.back()));
    }

    void TextBox::make_rows_newlines(const std::string& text)
    {
        mmax_line_width = 0;
        if(text.empty())
            return;
        size_t last_line_start = 0, pos = 0;
        do {
            pos = text.find('\n', last_line_start);
            add_row(text, last_line_start, (pos == std::string::npos)
                    ? pos : pos-last_line_start, 1);
            last_line_start = pos+1;
        } while(pos != std::string::npos);
    }

    void TextBox::make_rows_wordwrap(const std::string& text, int max_width, enum WrapMode wm)
    {
        const char *whitespace = " \t\r\n";
        mmax_line_width = 0;
        if(text.empty() || max_width <= 0)
            return;

        bool finished = false;
        size_t line_start = 0, last_word = 0,
               next_word = text.find_first_of(whitespace);
        int line_width = 0;

        const Font& fnt = font();

        const int space_width = fnt.text_width(" ");

        const size_t npos = std::string::npos;

        while(!finished) {
            int word_width = fnt.text_width(text, last_word,
                                            (next_word != npos) ? next_word - last_word : npos);
            if(line_width + word_width > max_width) {
                // line would be too long with this word
                bool word_too_wide = word_width > max_width;
                bool split_word = (wm == FittingWords && word_too_wide) ||
                                   wm == Characters;
                if(split_word) {
                    // always the case for WrapMode::Characters
                    size_t word_size;
                    if(next_word != npos)
                        word_size = next_word - last_word;
                    else
                        word_size = text.size() - last_word;
                    unsigned int split_idx = 1;
                    for(; split_idx < word_size; split_idx++) {
                        int part_word_w = fnt.text_width(text, last_word, split_idx);
                        if(part_word_w + line_width > max_width) {
                            break;
                        }
                    }
                    split_idx--;

                    // in case even one character won't fit:
                    // force emitting at least one character to avoid endless loop
                    if(split_idx == 0 && line_start == last_word)
                        split_idx++;

                    add_row(text, line_start, last_word + split_idx - line_start, 0);
                    last_word = last_word+split_idx;
                }
                else { // not splitting: always the case for WrapMode::WholeWords
                    if(!word_too_wide || line_start < last_word) {
                        // emit start of line until current word
                        // (always applicable in case of WrapMode::FittingWords)
                        add_row(text, line_start, last_word-line_start-1, 1);
                    }
                    else {
                        // (may be applicable for WrapMode::WholeWords only)
                        if(next_word != npos) {
                            // emit whole word on one line
                            add_row(text, line_start, next_word-line_start, 1);
                            last_word = next_word+1;
                            next_word = text.find_first_of(whitespace, next_word+1);
                        }
                        else {
                            // emit last line
                            add_row(text, line_start, next_word, 1);
                            last_word = next_word;
                            finished = true;
                        }
                    }
                }
                line_start = last_word;
                line_width = 0;
            }
            else if(next_word != npos && text[next_word] == '\n') {
                // new line: make new line in any case
                add_row(text, line_start, next_word-line_start, 1);
                last_word = line_start = next_word+1;
                line_width = 0;
                next_word = text.find_first_of(whitespace, next_word+1);
            }
            else if(next_word == npos) {
                // finished: add last line
                add_row(text, line_start, next_word, 1); // will clip npos
                finished = true;
            }
            else {
                if(wm == Characters &&
                        line_width + word_width + space_width > max_width) {
                    // catch spaces leading to lines being too wide
                    add_row(text, line_start, next_word-line_start, 1);
                    last_word = next_word+1;
                    line_start = last_word;
                    line_width = 0;
                }
                else {
                    // "add" the word to the same line
                    line_width += word_width + space_width;
                    last_word = next_word+1;
                }
                next_word = text.find_first_of(whitespace, next_word+1);
            }
        }
    }

    Point TextBox::tpx_to_rowcol(Point p, bool clip_to_text) const
    {
        if(mtext_lines.empty())
            return Point(-1, -1); // not hit

        int row = p.y() / line_height();
        if(clip_to_text) {
            // hit_char will clip x below
            if(row < 0)
                row = 0;
            else if(row >= signed(mtext_lines.size()))
                    row = signed(mtext_lines.size()-1);
        }
        else {
            if(row < 0 || row >= signed(mtext_lines.size()) ||
               p.x() < 0 || p.x() >= mmax_line_width) {
                return Point(-1, -1); // not hit
            }
        }
        std::pair <size_t, size_t> h;
        h = font().hit_char(mtext_lines.at(row), p.x());
        int col = h.second;
        return Point(col, row);
    }

    Point TextBox::rowcol_to_tpx(Point rc) const
    {
        if(mtext_lines.empty())
            return Point(0, 0);

        Point rcc = clip_row_col(rc);
        int y = rcc.y()*line_height();

        size_t offs = 0;
        for(int i = 0; i < rcc.x(); i++)
            utf8::next_cp(mtext_lines[rcc.y()], offs);

        int x = text_width(mtext_lines[rcc.y()].substr(0, offs));

        return Point(x,y);
    }

    size_t TextBox::rowcol_to_textoffs(Point rc) const
    {
        if(rc.x() == -1 && rc.y() == -1)
            return std::string::npos;
        if(mtext_lines.empty())
            return 0;
        Point rcc = clip_row_col(rc);
        //debug("\nClipped: %d, %d", rcc.x(), rcc.y());
        size_t offs = 0;
        for(int i = 0; i < rcc.y(); i++) {
            //debug("\nline %d: %d + %d", i, mtext_lines[i].size(), mtext_line_split_cwidth[i]);
            offs += mtext_lines[i].size()+mtext_line_split_cwidth[i];
        }
        size_t line_offs = 0;
        for(int i = 0; i < rcc.x(); i++) {
            utf8::next_cp(mtext_lines[rcc.y()], line_offs);
        }
        return offs + line_offs;
    }

    Point TextBox::texoffs_to_rowcol(size_t offs) const
    {
        if(mtext_lines.empty() || mtext.empty())
            return Point(0, 0);

        int o = (signed) offs; // 2 GB will suffice

        // o converted to signed can be < 0 if offs
        // was larger than 2 GB
        if(o < 0 || o >= signed(mtext.size())) {
            int y = mtext_lines.size()-1;
            return Point(mtext_lines_chrlen[y], y);
        }

        int row = 0;
        while(o-signed(mtext_lines[row].size()) - signed(mtext_line_split_cwidth[row]) >= 0
              && row < signed(mtext_lines.size())) {
            o-= mtext_lines[row].size() + mtext_line_split_cwidth[row];
            row++;
        }

        int x = utf8::length_cps_substr(mtext_lines[row], 0, size_t(o));

        return Point(x, row);
    }

    Point TextBox::clip_row_col(Point rc) const
    {
        if(mtext_lines.empty())
            return Point(0, 0);

        if(rc.y() >= signed(mtext_lines.size()))
            rc.set_y(signed(mtext_lines.size())-1);
        if(rc.y() < 0)
            rc.set_y(0);

        int split_c = mtext_line_split_cwidth.at(rc.y()) > 0;
        if(rc.x() >= mtext_lines_chrlen.at(rc.y())+split_c)
            rc.set_x(mtext_lines_chrlen.at(rc.y())+split_c-1);
        if(rc.x() < 0)
            rc.set_x(0);
        return rc;
    }

    void TextBox::set_wrap_mode(TextBox::WrapMode wrap_mode)
    {
        if(wrap_mode != mwrap_mode) {
            mwrap_mode = wrap_mode;
            update_rows();
        }
    }

    MeasureResults TextBox::measure(SizeConstraint wc, SizeConstraint hc)
    {
        int w = wc.value(), h = hc.value();

        // FIXME: shall we take everything? Avoid magic numbers.
        if(wc.mode() == SizeConstraintMode::NoLimits)
            w = font().char_width_hint() * WANT_WIDTH_CWH + mpadding.horz();
        if(hc.mode() == SizeConstraintMode::NoLimits)
            h = font().line_height() * WANT_HEIGHT_LINES + mpadding.vert();
        Size min_size = min_size_hint();
        return MeasureResults(w, h, w < min_size.w(), h < min_size.h());
    }

    Size TextBox::min_size_hint()
    {
        Size s(font().char_width_hint() * MIN_WIDTH_CWH + mvert_scrollbar.min_size_hint().w(),
               font().line_height() * MIN_HEIGHT_LINES + mhorz_scrollbar.min_size_hint().h());
        return mpadding.add(s);
    }

    int TextBox::line_height() const
    {
        return font().line_height()+1;
    }

    int TextBox::text_width(const std::string& str) const
    {
        return font().text_width(str);
    }

    Size TextBox::text_dims() const
    {
        return Size(mmax_line_width + mcursor_width + mtext_margins.horz(),
                    signed(mtext_lines.size()) *line_height() + mtext_margins.vert());
    }

    void TextBox::position_scrollbars()
    {
        mvert_scrollbar.set_pos(width() - mvert_scrollbar.width() - mscrollbar_padding.right(),
                                mscrollbar_padding.top());

        mhorz_scrollbar.set_pos(mscrollbar_padding.left(),
                                height() - mhorz_scrollbar.height() - mscrollbar_padding.bottom());
    }


    void TextBox::x_scrolled(int new_x_pos)
    {
        mscroll.set_x(new_x_pos);
    }

    void TextBox::y_scrolled(int new_y_pos)
    {
        mscroll.set_y(new_y_pos);
    }

}
