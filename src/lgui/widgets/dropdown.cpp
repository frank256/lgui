/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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
#include "dropdown.h"
#include "platform/graphics.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"
#include "lgui/style/style.h"

namespace lgui {

    DropDown::DropDown()
        : mdrop_down_event_filter(*this), mis_dropped_down(false), mabove(false)
    {
        set_focusable(true);
        set_may_tab_into(true);
        set_may_tab_out_of(true);

        style_changed(); // set padding etc.
        mlistbox.set_size_behavior(ScrollArea::ShrinkWithContent);
        mlistbox.list_view().set_select_on_hover_activate_on_click(true);
        mlistbox.list_view().set_wrap_around_keys(true);
        mlistbox.on_item_activated.connect([this](int idx, const std::string&) {
            listbox_activated(idx);
        });
        mlistbox.set_event_filter(&mdrop_down_event_filter);
    }

    void DropDown::draw(const DrawEvent& de) const
    {
        style().draw_drop_down(de.gfx(), StyleArgs(*this, de), mpadding,
                                  mlistbox.model().item_at_safe(mlistbox.selected_idx()),
                                  mis_dropped_down, mabove);
    }

    void DropDown::set_selected(int idx)
    {
        if(idx >= 0)
            mlistbox.set_selected_idx(idx);
    }

    void DropDown::set_model(StringListModel* model)
    {
        mlistbox.set_model(model);
    }

    void DropDown::mouse_pressed(MouseEvent& event)
    {
        if(mis_dropped_down) {
            fold_up();
        }
        else {
            drop_down();
        }
        event.consume();
    }

    void DropDown::key_pressed(KeyEvent& event)
    {
        if(mis_dropped_down) {
            if(event.key_code() == Keycodes::KEY_ESC ||
               event.key_code() == Keycodes::KEY_ENTER) {
                fold_up();
                event.consume();
            }
        }
        else {
            if(event.key_code() == Keycodes::KEY_SPACE) {
                drop_down();
                event.consume();
            }
        }
    }

    void DropDown::key_char(KeyEvent& event) {
        if(!mis_dropped_down) {
            int n = mlistbox.model().no_items();
            int idx = mlistbox.selected_idx();
            switch(event.key_code()) {
                case Keycodes::KEY_UP:
                    idx--;
                    if(idx < 0)
                        idx = n-1;
                    event.consume();
                    break;
                case Keycodes::KEY_DOWN:
                    idx++;
                    if(idx >= n)
                        idx = 0;
                    event.consume();
                    break;
                case Keycodes::KEY_HOME:
                    idx = 0;
                    event.consume();
                    break;
                case Keycodes::KEY_END:
                    idx = n-1;
                    event.consume();
                    break;
                default:
                    break;
            }
            if(idx != mlistbox.selected_idx()) {
                if(n > 0)
                    mlistbox.set_selected_idx(idx);
            }
        }
    }


    void DropDown::style_changed()
    {
        Widget::style_changed();
        mpadding = style().get_drop_down_padding();
        if(!mlistbox.has_strong_style())
            mlistbox.set_style(&style());
        // patch lists's text indent to match our padding!
        mlistbox.list_view().set_text_indent(mpadding.left() - mlistbox.padding().left());
    }

    void DropDown::set_font(const Font* font)
    {
        mlistbox.set_font(font);
        Widget::set_font(font);
    }

    void DropDown::listbox_activated(int idx)
    {
        (void)idx;
        if(mis_dropped_down)
            fold_up();
        on_selected_changed.emit(idx);
    }


    void DropDown::set_size(Size s)
    {
        Size min_size = style().get_drop_down_folded_min_size(font());
        // We don't allow changing the height
        Widget::set_size(Size(s.w(), min_size.h()));
    }

    // FIXME: should we occupy more space when dropped-down at all (happening inside the parent?)
    MeasureResults DropDown::measure(SizeConstraint wc, SizeConstraint hc)
    {
        Size min_size = style().get_drop_down_folded_min_size(font());
        int w = std::max(mlistbox.list_view().width(),min_size.w() + mlistbox.list_view().min_size_hint().w());
        MeasureResults r = force_size_constraints(Size(w, min_size.h()), wc, hc);;
        return r;

    }

    Size DropDown::min_size_hint()
    {
        Size s = style().get_drop_down_folded_min_size(font());
        s.set_w(s.w() + mlistbox.list_view().min_size_hint().w());
        return s;
    }


    void DropDown::drop_down()
    {
        mis_dropped_down = true;

        Rect r = get_absolute_rect();

        // Try below.
        mabove = false;

        mlistbox.set_pos(r.x1(), r.y2());

        const int MARGIN = 5;
        const int MIN_H = std::max(style().get_drop_down_folded_min_size(font()).h(),
                                   min_size_hint().h());
        SizeConstraint hc;
        if(top_widget()) {
            int avail = top_widget()->height() - mlistbox.pos_y() - MARGIN;
            if (avail < MIN_H) {
                // Try above
                avail = r.y1()-MARGIN;
                mabove = true;
            }

            hc = SizeConstraint(avail, SizeConstraintMode::Maximum);
        }
        else
            hc = SizeConstraint::no_limits();

        Size s = mlistbox.measure(SizeConstraint(width(), SizeConstraintMode::Exactly), hc);
        mlistbox.set_size(s);
        if (mabove)
            mlistbox.set_pos(r.x1(), r.y1()-s.h());
        request_modal_widget(mlistbox);
        mlistbox.list_view().focus();
        mlistbox.make_visible(mlistbox.selected_idx());
    }

    void DropDown::fold_up()
    {
        if(mis_dropped_down) {
            release_modal_widget(mlistbox);
            mis_dropped_down = false;
            mabove = false;
            focus(); // retain focus - since listbox had it, it's cleared now
        }
    }

    bool DropDown::DropDownEventFilter::mouse_event(Widget& w, MouseEvent& event) {

        if(event.type() == MouseEvent::Pressed) {
            if(!w.size_rect().contains(event.pos())) {
                mdd.fold_up();
                return false;
            }
        }
        return true;
    }

}
