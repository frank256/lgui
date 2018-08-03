/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#include "widgetpc.h"
#include "platform/error.h"
#include "lgui/drawevent.h"
#include "platform/graphics.h"

namespace lgui {


    WidgetPC::~WidgetPC()
    {
        clear_private_children();
    }

    void WidgetPC::visit_down(const std::function<void (Widget&)>& f)
    {
        f(*this);
        for(Widget* c : mprivate_children) {
            c->visit_down(f);
        }
    }

    Widget* WidgetPC::get_child_at(int x, int y)
    {
        bool ca_contains = children_area().contains(x, y);
        Position pos_sans_ca = Position(x, y) - children_area().pos();
        // reverse iteration as those at the end are considered to be "on-top"
        for(auto it = mprivate_children.rbegin();
            it != mprivate_children.rend(); ++it) {
            Widget* c = *it;
            if(c->is_visible() && c->is_active()) {
                if(c->is_outside_children_area()) {
                    if(c->rect().contains(x, y))
                        return c;
                }
                else {
                    if(ca_contains && c->rect().contains(pos_sans_ca))
                        return c;
                }
            }
        }
        return nullptr;
    }

    void WidgetPC::style_changed()
    {
        for(Widget* c : mprivate_children)
            if(!c->has_strong_style())
                c->set_style(&style());
        request_layout();
    }

    void WidgetPC::child_about_to_die(Widget& child)
    {
        remove_private_child(child);
    }

    void WidgetPC::draw_private_children(const DrawEvent& de, bool do_ca_pass, bool clip_ca) const
    {
        container_t::const_iterator first_outside_ca;
        if(do_ca_pass) {
            first_outside_ca = mprivate_children.end();
            de.gfx().push_draw_area(children_area(), clip_ca);
            for(auto cit = mprivate_children.begin(); cit != mprivate_children.end(); ++cit) {
                Widget* c = *cit;
                if(c->is_outside_children_area()) {
                    if(first_outside_ca == mprivate_children.end())
                        first_outside_ca = cit;
                    continue;
                }
                if(c->is_visible())
                    Widget::draw_child(*c, de);
            }
            de.gfx().pop_draw_area();
        }
        else
            first_outside_ca = mprivate_children.begin();

        for(auto cit = first_outside_ca;
            cit != mprivate_children.end(); ++cit) {
            Widget* c = *cit;
            if(!c->is_outside_children_area())
                continue;
            if(c->is_visible())
                Widget::draw_child(*c, de);
        }

    }

    void WidgetPC::add_private_child(Widget& widget)
    {
        bool found = false;
        for(container_t::const_iterator it = mprivate_children.begin();
            it != mprivate_children.end(); ++it) {
            if(*it == &widget) {
                found = true;
                break;
            }
        }
        if(!found) {
            mprivate_children.push_back(&widget);
            configure_new_child(widget);
            if(!widget.has_strong_style() && &widget.style() != &style())
                widget.set_style(&style());
        }
    }

    void WidgetPC::remove_private_child(Widget& widget)
    {
        bool found = false;
        for(auto it = mprivate_children.begin();
            it != mprivate_children.end(); ++it) {
            if(*it == &widget) {
                mprivate_children.erase(it);
                found = true;
                configure_child_to_be_removed(widget);
                break;
            }
        }
        ASSERT(found);
    }

    void WidgetPC::clear_private_children()
    {
        container_t cc = mprivate_children; // clone container
        for(Widget* w : cc)
            remove_private_child(*w);
    }

    void WidgetPC::_bring_child_to_front(Widget& child)
    {
        if(child.parent() == this && mprivate_children.back() != &child) {
            mprivate_children.remove(&child);
            mprivate_children.push_back(&child);
        }
    }

    void WidgetPC::_send_child_to_back(Widget& child)
    {
        if(child.parent() == this && mprivate_children.front() != &child) {
            mprivate_children.remove(&child);
            mprivate_children.push_front(&child);
        }
    }

}
