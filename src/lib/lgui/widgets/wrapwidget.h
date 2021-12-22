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

#ifndef LGUI_WRAPWIDGET_H
#define LGUI_WRAPWIDGET_H

#include "lgui/widget.h"
#include "lgui/iwidgetlistener.h"

namespace lgui {

/** Wraps another widget - allows to write a class that contains one other widget which is responsible for
 *  behavior. Adds its own padding to the size of the widget. Will take the size of the wrapped widget
 *  (resized to a size minus padding) plus padding when resized.
 */
class WrapWidget : public Widget {
    public:
        explicit WrapWidget(Widget* widget = nullptr);

        void draw(const DrawEvent& de) const override;
        virtual void draw_background(const DrawEvent& de) const { (void) de; }
        Rect children_area() const override;
        Widget* get_child_at(PointF p) override;

        void set_content(Widget* widget);

        const Padding& padding() const { return mpadding; }
        void set_padding(const Padding& padding);

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

        void visit_down(const std::function<void(Widget&)>& f) override;

    protected:
        void child_about_to_die(Widget& child) override;
        void style_changed() override;
        void resized(const Size& old_size) override;

    private:
        Widget* mcontent;
        Padding mpadding;
};

}

#endif // LGUI_WRAPWIDGET_H
