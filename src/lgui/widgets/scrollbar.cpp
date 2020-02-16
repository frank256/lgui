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

#include "scrollbar.h"
#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "lgui/mouseevent.h"
#include "lgui/style/style.h"
#include <cmath>


namespace lgui {

    ScrollBar::ScrollBar(Orientation o)
        : morientation(o), mtotal_scroll(0),
          mwindow_wh(0), mbar_wh(0), mscroll_pos(-1), mstep(10),
          mbar_width(style().get_scroll_bar_width()),
          mhover_handle(false),
          mhandle_dragged(false)
    {
        // initialize to bar width and zero
        if(o == Vertical)
            set_size(mbar_width, 0);
        else
            set_size(0, mbar_width);
    }

    void ScrollBar::setup(int total_scroll, int wh, int init_pos, int step)
    {
        setup(total_scroll, wh, wh, init_pos, step);
    }

    void ScrollBar::setup(int total_scroll, int window_wh, int bar_wh, int init_pos, int step)
    {
        /*ASSERT(total_scroll > 0);
        ASSERT(wh > 0);
        ASSERT(step > 0);*/
        // fix asserts being triggered on init
        if(total_scroll > 0 && window_wh > 0 && bar_wh > 0 && step > 0) {
            mtotal_scroll = total_scroll;
            mbar_wh = bar_wh;
            mwindow_wh = window_wh;
            mstep = step;
            scroll_to(init_pos, true);
            if(morientation == Horizontal)
                set_size(mbar_wh, mbar_width);
            else
                set_size(mbar_width, mbar_wh);
        }
    }

    void ScrollBar::calc_handle()
    {
        double handle_pos = (mscroll_pos/double(mtotal_scroll))*mbar_wh;
        double handle_len = mwindow_wh / double(mtotal_scroll) * mbar_wh;
        if(handle_len < 1)
            handle_len = 4;
        if(handle_len > mbar_wh)
            handle_len = mbar_wh;

        if(morientation == Horizontal) {
            //set_size(mbar_wh, mbar_width); // why here?
            mhandle_rect.set_size(round(handle_len), mbar_width);
            mhandle_rect.set_pos(round(handle_pos), 0);
        }
        else {
            //set_size(mbar_width, mbar_wh); // why here?
            mhandle_rect.set_size(mbar_width, round(handle_len));
            mhandle_rect.set_pos(0, round(handle_pos));
            /*debug("\nhandle pos-y: %d (hp: %f, hl: %f, sp: %d, total_scroll: %d, mwh: %d), handle-h: %d", mhandle_rect.y(),
                     handle_pos, handle_len, mscroll_pos, mtotal_scroll, mwh,
                     mhandle_rect.h());*/
        }

        if(!mhandle_dragged) {
            if(!is_hovered())
                mhover_handle = false;
            else {
                mhover_handle = mhandle_rect.contains(mlast_mouse_pos);
            }
        }
    }

    void ScrollBar::draw(const DrawEvent& de) const
    {
        style().draw_scroll_bar_body(de.gfx(), StyleArgs(*this, de),
                                   morientation == Horizontal);
        StyleArgs args(WidgetState(has_focus(), mhover_handle, de.draw_inactive(),
                                   false, mhandle_dragged), mhandle_rect, font(), de.opacity());
        style().draw_scroll_bar_handle(de.gfx(), args, morientation == Horizontal);
    }

    void ScrollBar::mouse_pressed(MouseEvent& event)
    {
        if(!mhandle_dragged &&
            (mhover_handle || mhandle_rect.contains(event.pos()))) {
            mhandle_dragged = mhover_handle = true;
            mpressed_handle_offs.set(event.x()-mhandle_rect.x1(), event.y()-mhandle_rect.y1());
        }
        event.consume();
    }

    void ScrollBar::mouse_moved(MouseEvent& event)
    {
        if(!mhandle_dragged) {
            bool on_handle = mhandle_rect.contains(event.pos());
            if(!mhover_handle && on_handle) {
                mhover_handle = true;
            }
            else if(mhover_handle && !on_handle)
                mhover_handle = false;

        }
        mlast_mouse_pos = event.pos();
        event.consume();
    }

    void ScrollBar::mouse_dragged(MouseEvent &event)
    {
        if(mhandle_dragged) {
            int h;
            if(morientation == Horizontal)
                h = event.x() - mpressed_handle_offs.x();
            else
                h = event.y() - mpressed_handle_offs.y();
            double s = double(h) / mbar_wh * mtotal_scroll;
            scroll_to(int(s));
            event.consume();
        }
        mlast_mouse_pos = event.pos();
    }

    void ScrollBar::mouse_entered(MouseEvent& event)
    {
        event.consume();
    }


    void ScrollBar::mouse_left(MouseEvent& event)
    {
        if(mhover_handle)
            mhover_handle = false;
        event.consume();
    }

    void ScrollBar::mouse_released(MouseEvent& event)
    {
        if(mhandle_dragged)
            mhandle_dragged = false;

        event.consume();
    }

    void ScrollBar::mouse_wheel_down(MouseEvent& event)
    {
        if(mscroll_pos < mtotal_scroll-mwindow_wh) {
            scroll_to(mscroll_pos+mstep);
        }
        event.consume();
    }

    void ScrollBar::mouse_wheel_up(MouseEvent& event)
    {
        if(mscroll_pos > 0) {
            scroll_to(mscroll_pos-mstep);
        }
        event.consume();
    }

    void ScrollBar::style_changed()
    {
        mbar_width = style().get_scroll_bar_width();
        calc_handle();
    }

    void ScrollBar::scroll_to_begin() {
        int new_pos = 0;
        if(mscroll_pos != new_pos) {
            mscroll_pos = new_pos;
            calc_handle();
            on_scrolled.emit(mscroll_pos);
        }
    }

    void ScrollBar::scroll_to_end() {
        int new_pos = mtotal_scroll-mwindow_wh;;
        if (mscroll_pos != new_pos) {
            mscroll_pos = new_pos;
            calc_handle();
            on_scrolled.emit(mscroll_pos);
        }
    }

     void ScrollBar::scroll_to(int new_pos, bool always_reemit)
    {
        if(new_pos > mtotal_scroll-mwindow_wh)
            new_pos = mtotal_scroll-mwindow_wh;
        if(new_pos < 0)
            new_pos = 0;
        if(always_reemit || mscroll_pos != new_pos) {
            mscroll_pos = new_pos;
            calc_handle();
            on_scrolled.emit(mscroll_pos);
        }
    }

    Size ScrollBar::min_size_hint()
    {
        return Size(mbar_width, mbar_width);
    }

    MeasureResults ScrollBar::measure(SizeConstraint wc, SizeConstraint hc)
    {
        Size s = morientation == Vertical ? Size(mbar_width, mbar_wh) : Size(mbar_wh, mbar_width);
        return force_size_constraints(s, wc, hc);
    }

}
