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

#ifndef LGUI_BOXLAYOUT_H
#define LGUI_BOXLAYOUT_H

#include "layout.h"
#include <vector>

#include "layoutitemcontainerbase.h"
#include "layoutitem.h"

namespace lgui {

namespace dtl {

/** For internal use by BoxLayout. */
class BoxLayoutItem : public LayoutItem {
    public:
        BoxLayoutItem(Orientation orientation, const LayoutItemProxy& le, int stretch, Align default_alignment)
                : LayoutItem(le), morientation(orientation), mstretch(stretch) {
            if (alignment() == Alignment::Default)
                set_alignment(default_alignment);
        }

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override {
            if (!mle && mstretch > 0) {
                // empty stretch element: want everything
                return (morientation == Horizontal) ? Size(wc.value(), 0) : Size(0, hc.value());
            }
            else {
                return LayoutItem::measure(wc, hc);
            }
        }

        Size min_size_hint() override {
            if (!mle && mstretch > 0) {
                return Size();
            }
            else
                return LayoutItem::min_size_hint();
        }

        int stretch() const { return mstretch; }

    protected:
        Orientation morientation;
        int mstretch;
};

}

/** Box layout allows you stack widgets in horizontal or vertical direction. You may add
 *  stretching or spacing inbetween.
 *
 *  Assuming the horizontal variant, BoxLayout calculates the width of all widgets that
 *  have a 0 set as stretch and the width of all the spacings that have been added. It
 *  substracts this total ("fixed") width from the width of the parent/container.
 *  The remaining space is distributed among the widgets or pure stretch elements according
 *  to their stretch value. For each widget, its share is determined by the ratio of its
 *  stretch value over the sum of all stretch values.

 *  Example: Assume you have:
 *  - a widget A with stretch of 3
 *  - a pure stretch element S of 1
 *  - a widget B with stretch of 2
 *  - a spacing element P of 10px
 *  - and a widget C with stretch 0 and a width of 30px:
 *
 *  As a result of BoxLayout's operations,
 *  - C will have a width of 30px
 *  - P will have a width of 10px
 *  - of the remaining space,
 *    + A will get 3/6=1/2
 *    + S 1/6
 *    + and B 2/6=1/3
 *
 *  Specify alignment in the cross direction via LayoutItemProxy.
 *
 */

class BoxLayout : public LayoutItemContainerBase<dtl::BoxLayoutItem, std::vector<dtl::BoxLayoutItem>> {
    public:
        explicit BoxLayout(Orientation o = Horizontal);

        void do_layout(const Rect& r) override;
        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

        void add_item(const LayoutItemProxy& elem, int stretch = 0);

        /** Add fixed-width/height spacing. */
        void add_spacing(int spacing);
        /** Add a stretch element. */
        void add_stretch(int stretch);

        /** Set the default alignment that will be applied to items added after calling this method. */
        void set_default_alignment(int align);

    protected:
        Orientation morientation;

        inline int primary_dim(const Size& s) const {
            return morientation == Horizontal ? s.w() : s.h();
        }

        inline int secondary_dim(const Size& s) const {
            return morientation == Horizontal ? s.h() : s.w();
        }

        inline Size make_size(int primary, int secondary) const {
            if (morientation == Horizontal)
                return {primary, secondary};
            else
                return {secondary, primary};
        }

    private:
        SizeConstraint mlast_wc, mlast_hc;
        int mdefault_alignment;
};

}

#endif // LGUI_BOXLAYOUT_H
