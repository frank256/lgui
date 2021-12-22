/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUI_SLIDER_H
#define LGUI_SLIDER_H

#include "lgui/widget.h"
#include "lgui/signal.h"

namespace lgui {

/** A slider widget. */
class Slider : public lgui::Widget {
    public:
        Signal<int> on_value_changed;
        Signal<int, int> on_range_changed;

        explicit Slider(Orientation o = Horizontal);

        void draw(const DrawEvent& de) const override;

        Orientation orientation() const { return morientation; }

        int min_value() const { return mmin_value; }
        int max_value() const { return mmax_value; }
        int value() const { return mvalue; }
        int range() const { return mmax_value - mmin_value; }
        float fraction() const;
        int steps() const { return msteps; }

        void set_min_value(int min);
        void set_max_value(int max);
        void set_value(int v);
        void set_fraction(float f);
        void set_steps(int steps);

        /** Use this to customize how the slider looks, i.e. its handle and body dimensions / proportions.
         *  Normally, this is set through the style, but after this function has been called, this
         *  information will not be set when the style is changed. Use `reset_slider_metrics()` to make
         *  the slider conform to style-wide settings again.
         *  @arg handle_primary: extension of the handle in the dimension of the slider's orientation
         *  @arg handle_secondary: extension of the handle in the dimension orthogonal to the slider's
         *                         orientation; will also be the width (not length) of the slider
         *  @arg body_width: the width of the body (orthogonal to the slider's orientation)
        */
        void set_slider_metrics(int handle_primary, int handle_secondary, int body_width);

        /** Reset slider metrics to those from the style if modified. */
        void reset_slider_metrics();

        int handle_primary_dim() const { return mhandle_prim_dim; }
        int handle_secondary_dim() const { return mhandle_sec_dim; }
        int body_width() const { return mbody_width; }

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

    protected:
        void key_char(KeyEvent& event) override;
        void mouse_pressed(MouseEvent& event) override;
        void mouse_moved(MouseEvent& event) override;
        void mouse_dragged(MouseEvent& event) override;
        void mouse_left(MouseEvent& event) override;
        void mouse_released(MouseEvent& event) override;
        void mouse_wheel_down(MouseEvent& event) override;
        void mouse_wheel_up(MouseEvent& event) override;
        void resized(const Size& old_size) override;
        void style_changed() override;

    private:
        void update_handle();
        int total_slider_dist() const;
        int pos_to_value(int left_top_handle_pos);

        Orientation morientation;
        int mmin_value, mmax_value, mvalue, msteps;
        Rect mhandle_rect, mbody_rect;
        int mhandle_prim_dim, mhandle_sec_dim, mbody_width;
        Position mpressed_handle_offs, mlast_mouse_pos;
        bool mhandle_hovered, mhandle_dragged, mcustom_metrics;
};

class HorizontalSlider : public Slider {
    public:
        HorizontalSlider()
                : Slider(Horizontal) {}
};

class VerticalSlider : public Slider {
    public:
        VerticalSlider()
                : Slider(Vertical) {}
};

}

#endif // LGUI_SLIDER_H
