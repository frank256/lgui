/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-20 frank256
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
#include "stringlistview.h"
#include "lgui/style/style.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"

#include "platform/graphics.h"

namespace lgui {

    StringListView::StringListView(StringListModel *model, const Font* font)
        : mpadding(style().get_list_padding()),
          mmodel(model),
          mselected_idx(-1), mitem_height(0), mmax_width(0),
          mindent(-1), mselect_on_hover_activate_on_click(false),
          mwrap_around_keys(false)
    {
        if(font)
            set_font(font);
        mitem_height = style().get_string_list_item_height(Widget::font());
        set_focusable(true);
        // don't forget to listen to model changes!
        if(mmodel)
            mmodel->add_listener(*this);
    }

    void StringListView::draw(const DrawEvent& de) const
    {
        if(mmodel) {
            int draw_begin_idx = 0, draw_end_idx = mmodel->no_items();
            if(pos_y() <= 0) {
                draw_begin_idx = std::max(-(pos_y() / mitem_height)-1, 0);
                int ph = page_height();
                if(ph > 0) {
                    draw_end_idx = std::min((-pos_y()+ph)/mitem_height+2,
                                            draw_end_idx);
                    draw_end_idx = std::max(draw_end_idx, 0);
                }
            }

            //for(int i = draw_begin_idx; i < draw_end_idx; i++)
            for(int i = draw_begin_idx; i < draw_end_idx; i++) {
                style().draw_string_list_item(de.gfx(),
                                       StyleArgs(*this, de, rect_for_item(i), false, false,
                                                 mselected_idx == i), mindent, mmodel->item_at(i));
            }
        }
    }

    void StringListView::set_selected_idx(int idx)
    {
        if(mmodel) {
            if(!mmodel->is_valid_index(idx))
                idx = -1;
            if(mselected_idx != idx) {
                mselected_idx = idx;
                on_selection_changed.emit(mselected_idx);
            }
        }
    }

    Rect StringListView::rect_for_item(int idx) const
    {
        if(mmodel && mmodel->is_valid_index(idx)) {
            int w = std::max(mmax_width, width() - mpadding.horz());
            return Rect (mpadding.left(), mpadding.top() + idx*mitem_height,
                           w+1, mitem_height+1);
        }
        else
            return Rect();
    }

    int StringListView::page_height() const
    {
        if(parent())
            return parent()->children_area().h();
        else
            return -1;
    }


    void StringListView::set_size(Size s)
    {
        // correctly dealing with the width?
        if(mmodel)
            Widget::set_size(Size(s.w(),
                                  mitem_height*mmodel->no_items()+mpadding.vert()));
        else
            Widget::set_size(s);
    }


    void StringListView::set_model(StringListModel *model)
    {
        if(mmodel != model) {
            if(mmodel) {
                about_to_invalidate_items();
                mmodel->remove_listener(*this);
            }
            mmodel = model;
            if(mmodel) {
                mmodel->add_listener(*this);
                items_invalidated();
            }
        }
    }

    void StringListView::about_to_add_items(int start_idx, int n)
    {
        (void) start_idx;
        (void) n;
    }

    void StringListView::about_to_remove_items(int start_idx, int n)
    {
        if(mselected_idx >= start_idx && mselected_idx < start_idx+n) {
            if(start_idx-1 >= 0)
                set_selected_idx(start_idx-1); // one before
            else {
                set_selected_idx(start_idx); // one after
            }
        }
        // keep selection
        else if(mselected_idx >= start_idx) {
            set_selected_idx(mselected_idx-n);
        }
    }

    void StringListView::about_to_invalidate_items()
    {
        set_selected_idx(-1);
    }

    void StringListView::items_added(int start_idx, int n)
    {
        ASSERT(mmodel);
        for(int i = start_idx; i < start_idx+n; i++) {
            int iw  = style().get_string_list_item_width(font(), mmodel->item_at(i));
            mmax_width = std::max(iw, mmax_width);
        }
        // keep selection
        if(mselected_idx >= start_idx) {
            set_selected_idx(mselected_idx+n);
        }
        request_layout();
    }

    void StringListView::items_removed(int start_idx, int n)
    {
        (void) start_idx; (void) n;
        ASSERT(mmodel);
        mmax_width = 0;
        for(int i = 0; i < mmodel->no_items(); i++) {
            int iw  = style().get_string_list_item_width(font(), mmodel->item_at(i));
            mmax_width = std::max(iw, mmax_width);
        }
        // We've mostly already dealt with selection in about_to_be_removed.
        if(mselected_idx >= mmodel->no_items())
            mselected_idx=mmodel->no_items()-1; // will set to -1
        request_layout();
    }

    void StringListView::items_invalidated()
    {
        ASSERT(mmodel);
        // selection?
        mmax_width = 0;
        items_added(0, mmodel->no_items());
        //request_layout(); // already in items_added
    }

    void StringListView::model_about_to_die()
    {
        mmodel = nullptr;
    }

    int StringListView::get_idx_from_pos(const Position& pos) const
    {
        if(mmodel && size_rect().contains(pos)) {
            int y  = pos.y();
            y /= mitem_height;
            if (y >= mmodel->no_items())
                y = mmodel->no_items()-1;
            if (y < 0)
                y = 0;
            return y;
        }
        else
            return -1;
    }


    Size StringListView::min_size_hint()
    {
        return Size(mmax_width + mpadding.horz(),
                    mitem_height * mmodel->no_items() + mpadding.vert());
    }

    MeasureResults StringListView::measure(SizeConstraint wc, SizeConstraint hc)
    {
        Size want_size = Size(mmax_width + mpadding.horz(),
                              mitem_height * mmodel->no_items() + mpadding.vert());
        return force_size_constraints(want_size, wc, hc);
    }

    void StringListView::emit_activated()
    {
        on_item_activated.emit(mselected_idx,
                               mmodel->item_at(mselected_idx));
    }

    void StringListView::mouse_moved(MouseEvent& event)
    {
        if(mselect_on_hover_activate_on_click) {
            int idx = get_idx_from_pos(event.pos());
            if(idx >= 0)
                set_selected_idx(idx);
        }
        event.consume();
    }

    void StringListView::mouse_pressed(MouseEvent& event)
    {
        int pressed_idx = get_idx_from_pos(event.pos());
        if(pressed_idx >= 0) {
            set_selected_idx(pressed_idx);
            if(!has_focus())
                focus();
        }
        event.consume();
    }

    void StringListView::mouse_clicked(MouseEvent& event)
    {
        if(mselect_on_hover_activate_on_click) {
            emit_activated();
            event.consume();
        }
    }

    void StringListView::mouse_dragged(MouseEvent& event)
    {
        int dragged_idx = get_idx_from_pos(event.pos());
        if(dragged_idx >= 0)
            set_selected_idx(dragged_idx);
        event.consume();
    }

    void StringListView::key_char(KeyEvent& event)
    {
        if(mmodel) {
            int ph = page_height();
            const int SKIP_PGUPDN_INDICES = ph / mitem_height;
            const int last_idx = mmodel->no_items() - 1;
            int sel = -1;
            switch(event.key_code()) {
                case Keycodes::KEY_UP:
                    if(mwrap_around_keys) {
                        sel = mselected_idx-1;
                        if(sel < 0)
                            sel = last_idx;
                    }
                    else {
                        if(mselected_idx > 0)
                            sel = mselected_idx-1;
                    }
                    break;
                case Keycodes::KEY_DOWN:
                    if(mwrap_around_keys) {
                        sel = mselected_idx+1;
                        if(sel > last_idx)
                            sel = 0;
                    }
                    else {
                        if(mselected_idx >= 0 && mselected_idx < last_idx)
                            sel = mselected_idx+1;
                    }
                    break;
                case Keycodes::KEY_PGUP:
                    if(SKIP_PGUPDN_INDICES > 0) {
                        if(mselected_idx > 0)
                            sel = std::max(0, mselected_idx-SKIP_PGUPDN_INDICES);
                    }
                    break;
                case Keycodes::KEY_PGDN:
                    if(SKIP_PGUPDN_INDICES > 0) {
                        if(mselected_idx >= 0)
                            sel = std::min(last_idx, mselected_idx+SKIP_PGUPDN_INDICES);
                    }
                    break;
                case Keycodes::KEY_HOME:
                    if(last_idx >= 0)
                        sel = 0;
                    break;
                case Keycodes::KEY_END:
                    sel = last_idx;
                    break;
                default: break;
            }
            if(last_idx >= 0 && sel >= 0 && sel <= last_idx) {
                set_selected_idx(sel);
                event.consume();
            }
        }
    }

    void StringListView::key_pressed(KeyEvent& event)
    {
        if(mmodel) {
            if(mselected_idx >= 0) {
                bool activate = false;
                switch(event.key_code()) {
                    case Keycodes::KEY_ENTER:
                    case Keycodes::KEY_ENTER_PAD:
                        activate = true;
                        break;
                    default: break;
                }
                if(activate) {
                    emit_activated();
                    event.consume();
                }
            }
        }
    }

}
