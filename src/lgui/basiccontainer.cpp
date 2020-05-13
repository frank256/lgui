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

#include "basiccontainer.h"
#include "layout/layout.h"

#include "platform/graphics.h"
#include "drawevent.h"
#include "lgui/internal/focusmanager.h"

#include <limits>

//#define DRAW_OUTLINES

namespace lgui {

    BasicContainer::BasicContainer()
        : mlayout(nullptr)
    {
    }

    BasicContainer::~BasicContainer()
    {
        // remove layout first, in order not to
        // trigger relayouting in destructor
        if(mlayout) {
            set_layout(nullptr);
        }
        remove_all_children(); // prevent dangling pointers in children
                      // i.e. if parent is destroyed first
    }

    void BasicContainer::draw(const DrawEvent& de) const
    {
        draw_background(de);
        draw_children(de);
#ifdef DRAW_OUTLINES
        de.gfx().rect(0, 0, width(), height(), is_hovered() ? rgb(0, 1, 0) : rgb(0, 0.75, 0), 1);
        Rect ca = children_area();
        de.gfx().rect(ca, is_hovered() ? rgb(1, 1, 0) : rgb(0.75, 0.75, 0), 1);
#endif
    }

    void BasicContainer::draw_children(const DrawEvent& de) const
    {
        de.gfx().push_draw_area(children_area());
        for(const Widget* c : mchildren) {
            if(c->is_visible())
                draw_child(*c, de);
        }
        de.gfx().pop_draw_area();
    }

    void BasicContainer::set_children_area(const Rect& children_area)
    {
        if(mchildren_area != children_area) {
            mchildren_area = children_area;
            if(mlayout)
                request_layout();
        }
    }

    Rect BasicContainer::children_area() const
    {
        return mchildren_area;
    }


    Widget* BasicContainer::get_child_at(int x, int y)
    {
        // FIXME: Need to handle outside_children_area here?
        //        It's not really a flag for public children though...
        if(!children_area().contains(x, y))
            return nullptr;
        // reverse iteration as those at the end are considered to be "on-top"
        for(auto it = mchildren.rbegin(); it != mchildren.rend(); ++it) {
            Widget* c = *it;
            if(c->is_visible() && c->is_active() && c->is_inside(Position(x, y))) {
                return c;
            }
        }
        return nullptr;
    }

    void BasicContainer::add_child(Widget& widget)
    {
        ASSERT(widget.parent() == nullptr);
        // take precautions against double add
        bool found = false;
        for (auto& child : mchildren) {
            if(child == &widget) {
                found = true;
                break;
            }
        }
        if(!found) {
            mchildren.push_back(&widget);
            configure_new_child(widget);
            if (mlayout) {
                mlayout->_child_added_to_target(widget);
            }
            if (!widget.has_strong_style() && &widget.style() != &style())
                widget.set_style(&style());
            _emit_child_added(widget);
            // FIXME: Not sure about this... We do this to reset the needs_relayout flags in the hierarchy that has been
            // added. Otherwise, request_layout() would always have to be called manually after calling add_child on a
            // parent without layout to achieve clean interoperation between widgets with/-out layout.
            // Problem: If you forget on a parent level, calling it (without "true", which I dislike) will not have any
            // effect on levels below due to these flags being set. Therefore, we rather do it here automatically, I think.
            if (!mlayout && is_added_to_gui()) {
                request_layout();
            }
        }
    }

    void BasicContainer::remove_child(Widget& widget)
    {
        bool found = false;
        for(auto it = begin(); it != end(); ++it) {
            if(*it == &widget) {
                mchildren.erase(it);
                found = true;
                break;
            }
        }
        ASSERT(found);
        if(found) {
            if(mlayout) {
                mlayout->_child_removed_from_target(widget);
            }
            configure_child_to_be_removed(widget);
            _emit_child_removed(widget);
        }
    }

    void BasicContainer::remove_all_children()
    {
        container_t cc = mchildren; // clone container
        for(Widget* w : cc)
            remove_child(*w);
    }

    void BasicContainer::set_layout(Layout* layout)
    {
        if(mlayout)
            mlayout->_set_target(nullptr);
        mlayout = layout;
        if(layout) {
            layout->_set_target(this);
            request_layout();
        }
    }

    void BasicContainer::layout(const Rect& r) {
        set_layout_in_progress(true);
        set_rect(r);
        if(mlayout)
            mlayout->layout(Rect(0, 0, children_area().size())); // call this?
        else {
            for(Widget* c : mchildren) {
                if(c->needs_relayout())
                    c->_relayout();
            }
        }
        set_need_relayout(false);
        set_layout_in_progress(false);
        post_layout();
    }

    void BasicContainer::child_about_to_die(Widget& child)
    {
        remove_child(child);
    }


    void BasicContainer::resized(const Size& old_size)
    {
        (void) old_size;
        mchildren_area.set_size(size());
        mchildren_area.set_pos(0, 0);
        if(mlayout)
            request_layout();
    }


    void BasicContainer::style_changed()
    {
        for(Widget* child : mchildren) {
            if(!child->has_strong_style())
                child->set_style(&style());
        }
        request_layout();
    }

    void BasicContainer::visit_down(const std::function<void (Widget&)>& f)
    {
        f(*this);
        for (Widget* child : mchildren) {
            child->visit_down(f);
        }
    }

    MeasureResults BasicContainer::measure_children(SizeConstraint wc, SizeConstraint hc)
    {
        if(mlayout)
            return mlayout->measure(wc, hc);
        else {
            // get bounding box
            Rect bbx = children_bounding_box();
            bbx = Rect(0, 0, bbx.x2()+1, bbx.y2()+1);
            return force_size_constraints(bbx.size(), wc, hc);
        }
    }

    Size BasicContainer::min_size_hint_children() const
    {
        if(!mlayout) {
            Rect bbx = children_bounding_box();
            return Size(bbx.x2()+1, bbx.y2()+1);
        }
        else
            return mlayout->min_size_hint();
    }

    Rect BasicContainer::children_bounding_box() const
    {
        if(mchildren.empty())
            return Rect();
        int l, t, r, b;
        l = t = std::numeric_limits<int>::max();
        r = b = std::numeric_limits<int>::min();
        for(Widget*c : mchildren) {
            l = std::min(l, c->left());
            t = std::min(t, c->top());
            r = std::max(r, c->right());
            b = std::max(b, c->bottom());
        }
        return Rect(l, t, r-l, b-t);
    }

    void BasicContainer::_bring_child_to_front(Widget& child)
    {
        if(child.parent() == this) {
            if(mchildren.back() != &child) {
                mchildren.remove(&child);
                mchildren.push_back(&child);
            }
        }
    }

    void BasicContainer::_send_child_to_back(Widget& child)
    {
        if(child.parent() == this) {
            if(mchildren.front() != &child) {
                mchildren.remove(&child);
                mchildren.push_front(&child);
            }
        }
    }

}

