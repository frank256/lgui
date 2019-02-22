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

#include "container.h"
#include "lgui/style/style.h"
#include "lgui/drawevent.h"

namespace lgui {

    PaddedContainer::PaddedContainer()
        : mpadding(0)
    {}

    void PaddedContainer::set_padding(const Padding& padding)
    {
        mpadding = padding;
        update_children_area();
    }

    void PaddedContainer::resized(const Size& old_size)
    {
        (void) old_size;
        update_children_area();
    }

    void PaddedContainer::update_children_area()
    {
        set_children_area(Rect(mpadding.left_top_offs(), mpadding.sub(size())));
    }

    MeasureResults PaddedContainer::measure(SizeConstraint wc, SizeConstraint hc)
    {
        return force_size_constraints(mpadding.add(measure_children(wc.sub(mpadding.horz()), hc.sub(mpadding.vert()))),
                                      wc, hc);
    }

    Size PaddedContainer::min_size_hint()
    {
        return mpadding.add(min_size_hint_children());
    }

    Container::Container()
    {
        set_padding(style().get_container_padding());
    }

    void Container::draw_background(const DrawEvent& de) const
    {
        StyleArgs args(*this, de);
        style().draw_container_bg(de.gfx(), args);
    }

    void Container::style_changed()
    {
        // FIXME: sensible behavior to only enlarge, but not shrink the padding?
        Padding np = style().get_container_padding();
        const Padding& p = padding();

        if(p.left() > np.left())
            np.set_left(p.left());
        if(p.top() > np.top())
            np.set_top(p.top());
        if(p.right() > np.right())
            np.set_right(p.right());
        if(p.bottom() > np.bottom())
            np.set_bottom(p.bottom());

        //Size s;
        //s = p.sub(size());
        set_padding(np);
        //set_size(np.add(s));
        BasicContainer::style_changed(); // calls request_layout()
    }

}
