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

#include "stackedcontainer.h"
#include "lgui/platform/error.h"
#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"

namespace lgui {

StackedContainer::StackedContainer()
        : mactive_widget(nullptr),
          mlayout_consider_active_only(false) {
    PaddedContainer::set_layout(&mlayout);
    // listening to self to catch add/remove
    add_widget_listener(this);
}


StackedContainer::~StackedContainer() {
    remove_widget_listener(this);
}

MeasureResults StackedContainer::measure(SizeConstraint wc, SizeConstraint hc) {
    if (mlayout_consider_active_only && mactive_widget) {
        const Padding& p = padding();
        return force_size_constraints(
                mactive_widget->measure(wc.sub(p.horz()), hc.sub(p.vert())).add_padding(padding()),
                wc, hc);
    }
    else {
        Size r(0, 0);
        TooSmallAccumulator ts;
        for (Widget* c : (*this)) {
            Size s = ts.consider(c->measure(wc.adapted_for_child(), hc.adapted_for_child()));
            r.set_w(std::max(r.w(), s.w()));
            r.set_h(std::max(r.h(), s.h()));
        }
        r = padding().add(r);
        //        debug("Measure StackedContainer %d, %d -> %d, %d\n", wc.value(), hc.value(),
        //                 r.w(), r.h());
        return force_size_constraints(r, wc, hc, ts);
    }
}

Size StackedContainer::min_size_hint() {
    if (mlayout_consider_active_only && mactive_widget) {
        return mactive_widget->min_size_hint() + padding();
    }
    else {
        Size r;
        for (Widget* c : (*this)) {
            Size s = c->min_size_hint();
            r.set_w(std::max(r.w(), s.w()));
            r.set_h(std::max(r.h(), s.h()));
        }
        r = padding().add(r);
        return r;
    }
}

void StackedContainer::set_active_widget(Widget* widget) {
    ASSERT(widget);
    if (widget == mactive_widget)
        return;

    bool found = false;
    for (Widget* w : *this) {
        bool is_active = (w == widget);
        w->set_visibility(is_active ? Visibility::Visible : Visibility::Invisible);
        w->set_active(is_active);
        if (is_active)
            found = true;
    }
    ASSERT(found);
    mactive_widget = widget;
    if (mlayout_consider_active_only)
        request_layout();
}

Widget* StackedContainer::get_child_at(PointF) {
    return mactive_widget;
}

void StackedContainer::draw_children(const DrawEvent& de) const {
    if (mactive_widget) {
        de.gfx().push_draw_area(children_area());
        if (mactive_widget->is_visible()) {
            draw_child(*mactive_widget, de);
        }
        de.gfx().pop_draw_area();
    }
}

void StackedContainer::child_added_wl(Widget& w, Widget& child) {
    ASSERT(&w == this);
    if (no_children() == 1) {
        set_active_widget(*begin());
    }
    else {
        // FIXME: correct? It's late...
        child.set_active(false);
        child.set_invisible();
    }
}

void StackedContainer::child_removed_wl(Widget& w, Widget& child) {
    ASSERT(&w == this);
    if (&child == mactive_widget) {
        mactive_widget = nullptr;
        if (no_children() > 0) {
            set_active_widget(*begin());
        }
    }
}

void StackedContainer::StackedLayout::do_layout(const Rect& r) {
    // exists to inform about the size
    if (!mtarget)
        return;
    for (Widget* w : (*mtarget)) {
        w->layout(Rect(0, 0, r.size()));
    }
}

bool StackedContainer::StackedLayout::_remove_widget_fnlh(Widget& w) {
    (void) w;
    return false;
}

void StackedContainer::StackedLayout::remove_all() {
    // This is never called since the whole layout is private; we do not store data in the layout anyway.
}

}
