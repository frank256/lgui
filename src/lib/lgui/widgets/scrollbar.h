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

#ifndef LGUI_SCROLLBAR_H
#define LGUI_SCROLLBAR_H

#include "lgui/widget.h"
#include "lgui/signal.h"

namespace lgui {

/** A scroll bar class. Currently does not display buttons.
 *  You normally don't want to use this directly, but rather use a ScrollArea. */
class ScrollBar : public Widget {
        friend class ScrollArea;

    public:
        explicit ScrollBar(Orientation o = Vertical);

        void draw(const DrawEvent& de) const override;

        void setup(int total_scroll, int wh, int init_pos, int step);
        void setup(int total_scroll, int window_wh, int bar_wh, int init_pos, int step);

        Signal<int> on_scrolled;
        int scroll_pos() const { return mscroll_pos; }

        void scroll_to(int new_pos, bool always_reemit = false);
        void scroll_to_begin();
        void scroll_to_end();

        Size min_size_hint() override;
        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;

    protected:
        void mouse_pressed(MouseEvent& event) override;
        void mouse_released(MouseEvent& event) override;
        void mouse_moved(MouseEvent& event) override;
        void mouse_dragged(MouseEvent& event) override;
        void mouse_entered(MouseEvent& event) override;
        void mouse_left(MouseEvent& event) override;
        void mouse_wheel_down(MouseEvent& event) override;
        void mouse_wheel_up(MouseEvent& event) override;

        void style_changed() override;

    private:
        void calc_handle();

        Orientation morientation;
        int mtotal_scroll, mwindow_wh, mbar_wh, mscroll_pos, mstep;
        int mbar_width;
        lgui::Rect mhandle_rect;
        bool mhover_handle, mhandle_dragged;
        Position mpressed_handle_offs, mlast_mouse_pos;
};

}

#endif // LGUI_SCROLLBAR_H
