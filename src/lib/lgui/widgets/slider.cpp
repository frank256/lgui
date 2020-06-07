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

#include "slider.h"
#include <cmath>
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "lgui/platform/keycodes.h"
#include "lgui/platform/graphics.h"
#include "lgui/style/style.h"

namespace lgui {

    Slider::Slider(Orientation o)
        : morientation(o), mmin_value(0),
          mmax_value(100), mvalue(0), msteps(10),
          mhandle_prim_dim(20), mhandle_sec_dim(20),
          mbody_width(6),
          mhandle_hovered(false),
          mhandle_dragged(false),
          mcustom_metrics(false)
    {
        style_changed(); // get SliderMetrics from Style
        if(morientation == Horizontal)
            mhandle_rect.set_width(mhandle_prim_dim);
        else
            mhandle_rect.set_height(mhandle_prim_dim);
        set_focusable(true);
    }

    void Slider::draw(const DrawEvent& de) const
    {
        StyleArgs body_args(*this, de, mbody_rect);

        StyleArgs handle_args(WidgetState(has_focus(), mhandle_hovered, de.draw_disabled(), false, mhandle_dragged),
                              mhandle_rect, font(), de.opacity());

        style().draw_slider_body(de.gfx(), body_args, mhandle_rect, morientation == Horizontal);
        style().draw_slider_handle(de.gfx(), handle_args, morientation == Horizontal);
    }

    float Slider::fraction() const
    {
        if(range() > 0)
            return float(mvalue) / float(range());
        else
            return 0.0;
    }

    void Slider::set_min_value(int min)
    {
        min = std::min(mmax_value, min);
        bool change_range = mmin_value != min;
        mmin_value = min;
        if(change_range)
            on_range_changed.emit(mmin_value, mmax_value);

        int nv = std::max(mvalue, mmin_value);
        if(nv != mvalue)
            set_value(nv);
    }

    void Slider::set_max_value(int max)
    {
        max = std::max(mmin_value, max);
        bool change_range = mmax_value != max;
        mmax_value = max;
        if(change_range)
            on_range_changed.emit(mmin_value, mmax_value);

        int nv = std::min(mvalue, mmax_value);
        if(nv != mvalue)
            set_value(nv);
    }

    void Slider::set_value(int v)
    {
        v = std::min(v, mmax_value);
        v = std::max(v, mmin_value);
        if(v != mvalue) {
            mvalue = v;
            update_handle();
            on_value_changed.emit(mvalue);
        }
    }

    void Slider::set_fraction(float f)
    {
        f = std::max(0.0f, f);
        f = std::min(1.0f, f);
        int nv = roundf(f*float(mmax_value-mmin_value)) + mmin_value;
        if(nv != mvalue)
            set_value(nv);
    }

    void Slider::set_steps(int steps)
    {
        steps = std::min(steps, range());
        steps = std::max(steps, 1);
        msteps = steps;
    }

    void Slider::set_slider_metrics(int handle_primary, int handle_secondary, int body_width)
    {
        mhandle_prim_dim = handle_primary;
        mhandle_sec_dim = handle_secondary;
        mbody_width = body_width;
        mcustom_metrics = true;
        request_layout();
    }

    void Slider::reset_slider_metrics()
    {
        mcustom_metrics = false;
        style_changed();
    }

    MeasureResults Slider::measure(SizeConstraint wc, SizeConstraint hc)
    {
        const int SLIDER_LENGTH = 400; // FIXME: arbitrary constant

        if(orientation() == Horizontal) {
            int w = wc.value();
            if(wc.mode() == SizeConstraintMode::NoLimits)
                w = SLIDER_LENGTH;
            return force_size_constraints(Size(w, mhandle_sec_dim), wc, hc);
        }
        else {
            int h = hc.value();
            if(hc.mode() == SizeConstraintMode::NoLimits)
                h = SLIDER_LENGTH;
            return force_size_constraints(Size(mhandle_sec_dim, h), wc, hc);
        }
    }

    Size Slider::min_size_hint()
    {
        // FIXME: better values?
        if(orientation() == Horizontal)
            return Size(mhandle_prim_dim, mhandle_sec_dim);
        else
            return Size(mhandle_sec_dim, mhandle_prim_dim);
    }


    void Slider::update_handle()
    {
        int p = roundf(fraction()*total_slider_dist());
        if(morientation == Horizontal)
            mhandle_rect.set_pos(p, 0);
        else
            mhandle_rect.set_pos(0, p);
        if(!mhandle_dragged) {
            if(!is_hovered())
                mhandle_hovered = false;
            else
                mhandle_hovered = mhandle_rect.contains(mlast_mouse_pos);
        }
    }

    int Slider::total_slider_dist() const
    {
        if(morientation == Horizontal)
            return width() - mhandle_prim_dim;
        else
            return height() - mhandle_prim_dim;
    }

    int Slider::pos_to_value(int left_top_handle_pos)
    {
        int wh = total_slider_dist();
        double s = double(left_top_handle_pos) / wh;
        s *= range();
        s += mmin_value;
        return round(s);
    }

    void Slider::mouse_pressed(MouseEvent& event)
    {
        if(!mhandle_dragged &&
            (mhandle_hovered || mhandle_rect.contains(event.pos()))) {
            mhandle_dragged = mhandle_hovered = true;
            mpressed_handle_offs.set(event.x() - mhandle_rect.x1(),
                                     event.y() - mhandle_rect.y1());
        }
        else {
            Point p = event.pos();
            p -= Point(mhandle_rect.w()/2, mhandle_rect.h()/2);
            int v;
            if(morientation == Horizontal)
                v = pos_to_value(p.x());
            else
                v = pos_to_value(p.y());
            set_value(v);
            if(mhandle_rect.contains(event.pos())) {
                mhandle_dragged = mhandle_hovered = true;
                mpressed_handle_offs.set(event.x() - mhandle_rect.x1(),
                                         event.y() - mhandle_rect.y1());
            }
        }
        focus();
        event.consume();
    }

    void Slider::mouse_moved(MouseEvent& event)
    {
        if(!mhandle_dragged) {
            bool on_handle = mhandle_rect.contains(event.pos());
            if(!mhandle_hovered && on_handle) {
                mhandle_hovered = true;
            }
            else if(mhandle_hovered && !on_handle)
                mhandle_hovered = false;
        }
        mlast_mouse_pos = event.pos();
        event.consume();
    }

    void Slider::mouse_dragged(MouseEvent& event)
    {
        if(mhandle_dragged) {
            int where;
            if(morientation == Horizontal)
                where = event.x() - mpressed_handle_offs.x();
            else
                where = event.y() - mpressed_handle_offs.y();
            int s = pos_to_value(where);
            set_value(s);
            event.consume();
        }
        mlast_mouse_pos = event.pos();
    }

    void Slider::mouse_left(MouseEvent& event)
    {
        if(mhandle_hovered)
            mhandle_hovered = false;
        event.consume();
    }

    void Slider::mouse_released(MouseEvent& event)
    {
        if(mhandle_dragged) {
            mhandle_dragged = false;
            mhandle_hovered = mhandle_rect.contains(event.pos());
        }
        event.consume();
    }

    void Slider::mouse_wheel_down(MouseEvent& event)
    {
        if(mvalue < mmax_value) {
            set_value(mvalue+msteps);
        }
        event.consume();
    }

    void Slider::mouse_wheel_up(MouseEvent& event)
    {
        if(mvalue > mmin_value) {
            set_value(mvalue-msteps);
        }
        event.consume();
    }

    void Slider::key_char(KeyEvent& event)
    {
        switch(event.key_code()) {
            case Keycodes::KEY_HOME:
                set_value(mmin_value);
                event.consume();
                break;
            case Keycodes::KEY_END:
                set_value(mmax_value);
                event.consume();
                break;
            case Keycodes::KEY_UP:
            case Keycodes::KEY_LEFT:
                set_value(mvalue - msteps);
                event.consume();
                break;
            case Keycodes::KEY_DOWN:
            case Keycodes::KEY_RIGHT:
                set_value(mvalue + msteps);
                event.consume();
                break;
            default: break;
        }
    }

    void Slider::resized(const Size& old_size)
    {
        (void) old_size;
        if(morientation == Horizontal) {
            mhandle_rect.set_height(height());
            mbody_rect = Rect(mhandle_prim_dim/2, (height() - mbody_width) / 2, width() - mhandle_rect.w(), mbody_width);
        }
        else {
            mhandle_rect.set_width(width());
            mbody_rect = Rect((width() - mbody_width) / 2, mhandle_prim_dim/2, mbody_width, height() - mhandle_rect.h());
        }
        update_handle();
    }

    void Slider::style_changed()
    {
        if(!mcustom_metrics) {
            Style::SliderMetrics sm = style().get_slider_metrics();
            mhandle_prim_dim = sm.handle_primary_dim;
            mhandle_sec_dim = sm.handle_secondary_dim;
            mbody_width = sm.body_width;
        }
        request_layout();
    }

}

