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

#include "wrapwidget.h"
#include "platform/error.h"
#include "platform/graphics.h"
#include "lgui/drawevent.h"

namespace lgui {

    WrapWidget::WrapWidget(Widget* widget)
        : mcontent(widget)
    {
        if(widget)
            set_content(widget);
    }

    void WrapWidget::draw(const DrawEvent& de) const
    {
        if(mcontent) {
             // FIXME: draw backgr. when mcontent == nullptr?
            draw_background(de);
            de.gfx().push_draw_area(children_area(), false);
            mcontent->draw(DrawEvent(de.gfx(), de.draw_inactive() || mcontent->is_inactive(),
                           de.opacity() * mcontent->opacity()));
            de.gfx().pop_draw_area();
        }
    }

    Rect WrapWidget::children_area() const
    {
        if(mcontent)
            return lgui::Rect(mpadding.left_top_offs(),
                             mpadding.sub(size()));
        else
            return size_rect();
    }

    Widget* WrapWidget::get_child_at(int x, int y)
    {
        (void) x; (void) y;  // FIXME: check contains?
        return mcontent;
    }

    void WrapWidget::set_content(lgui::Widget* widget)
    {
        mcontent = widget;
        if(widget) {
            widget->set_pos(0, 0);
            configure_new_child(*widget);
            if(!widget->has_strong_style() && &widget->style() != &style())
                widget->set_style(&style());
            request_layout();
        }
    }

    void WrapWidget::style_changed()
    {
        if(mcontent && !mcontent->has_strong_style())
            mcontent->set_style(&style());
    }

    void WrapWidget::resized(const Size& old_size)
    {
        (void) old_size;
        if(mcontent) {
            mcontent->layout(Rect({0, 0}, mpadding.sub(size())));
        }
    }

    void WrapWidget::set_padding(const Padding& padding)
    {
        mpadding = padding;
        set_size(Size(width(), height())); // will set size of content
    }

    MeasureResults WrapWidget::measure(SizeConstraint wc, SizeConstraint hc)
    {
        if(!mcontent)
            return force_size_constraints(Size(mpadding.horz(), mpadding.vert()), wc, hc);
        else {
            MeasureResults r = mcontent->measure(wc.sub(mpadding.horz()), hc.sub(mpadding.vert()));
            return force_size_constraints(mpadding.add(r), wc, hc);
        }
    }

    Size WrapWidget::min_size_hint()
    {
        Size s;
        if(mcontent)
            s = mcontent->min_size_hint();
        return mpadding.add(s);
    }


    void WrapWidget::visit_down(const std::function<void (Widget&)>& f)
    {
        f(*this);
        if (mcontent)
            mcontent->visit_down(f);
    }

    void WrapWidget::child_about_to_die(Widget& child)
    {
        if(&child == mcontent)
            set_content(nullptr);
    }



}
