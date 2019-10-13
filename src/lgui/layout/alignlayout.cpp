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

#include "alignlayout.h"

namespace lgui {

void AlignLayout::add_item(const LayoutItemProxy& elem)
{
    mitems.emplace_back(LayoutItem(elem));
    added_elem(*elem.elem());
}

MeasureResults AlignLayout::measure(SizeConstraint wc, SizeConstraint hc)
{
    mlast_wc = wc;
    mlast_hc = hc;

    TooSmallAccumulator too_small;

    Size max_content_size;

    for(auto& li : mitems) {
        if(li.skip())
            continue;

        SizeConstraint horz = get_child_constraint(wc);
        SizeConstraint vert = get_child_constraint(hc);

        MeasureResults r = li.measure(horz, vert);
        Size s = too_small.consider(r);

        li.set_allotted_size(s);

        max_content_size.set_w(std::max(max_content_size.w(), s.w()));
        max_content_size.set_h(std::max(max_content_size.h(), s.h()));
    }

    return MeasureResults(max_content_size.w(), max_content_size.h(), too_small.width_too_small(), too_small.height_too_small());
}

void AlignLayout::do_layout(const Rect& r)
{
    if(!mtarget)
        return;

    Size ts = r.size();
    SizeConstraint wc = SizeConstraint(ts.w(), SizeConstraintMode::Exactly);
    SizeConstraint hc = SizeConstraint(ts.h(), SizeConstraintMode::Exactly);

    if(mtarget->needs_relayout() ||
       (wc != mlast_wc || hc != mlast_hc)) {
        measure(wc, hc);
    }

    for(auto& li : mitems) {
        if(li.skip())
            continue;
        li.layout(li.allotted_rect());
    }
}

SizeConstraint AlignLayout::get_child_constraint(SizeConstraint c) {
    if (c.mode() == NoLimits)
        return c;
    return SizeConstraint(c.value(), SizeConstraintMode::Exactly);
}

}
