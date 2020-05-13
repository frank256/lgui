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

#include "flowlayout.h"

#include <vector>
#include <algorithm>
#include <limits>

#ifdef DEBUG_LAYOUT
#define DBG(...) debug("FlowLayout: " __VA_ARGS__)
#else
#define DBG(...)
#endif


namespace lgui {

FlowLayout::FlowLayout()
        : mmax_on_row(std::numeric_limits<int>::max()), mhorz_spacing(default_spacing),
          mvert_spacing(default_spacing), mhcenter(false),
          msingle_row(false) {}

void FlowLayout::do_layout(const Rect& r) {
    if (!mtarget)
        return;

    // remeasure?

    Size ts = r.size();
    SizeConstraint wc = SizeConstraint(ts.w(), SizeConstraintMode::Exactly);
    SizeConstraint hc = SizeConstraint(ts.h(), SizeConstraintMode::Exactly);

    if (mtarget->needs_relayout() ||
        (wc != mlast_wc || hc != mlast_hc)) {
        measure(wc, hc);
    }

    for (auto& li : mitems) {
        if (li.skip())
            continue;
        li.layout(li.allotted_rect().translated(r.pos()));
    }
}

void FlowLayout::center_helper(std::vector<LayoutItem>& container, std::vector<LayoutItem>::iterator it, int offsx) {
    // We add offsx to all the items starting with it, then going back
    // as long as y does not change.

    // Deal with last item here, since reverse_iterator will point to element
    // *before* it.
    LayoutItem& last_item = *it;
    last_item.set_allotted_pos(last_item.allotted_pos() + Position(offsx, 0));
    int line_y = last_item.allotted_pos().y();

    std::vector<LayoutItem>::reverse_iterator rit(it);
    while (rit != container.rend()) {
        LayoutItem& item = *rit;
        if (!item.skip()) {
            if (item.allotted_pos().y() != line_y)
                break;
            item.set_allotted_pos(item.allotted_pos() + Position(offsx, 0));
        }
        ++rit;
    }
}


MeasureResults FlowLayout::measure(SizeConstraint wc, SizeConstraint hc) {
    // FIXME: Doesn't work?
    mlast_wc = wc;
    mlast_hc = hc;

    DBG("MEASURE CALLED for with wc %d (%d), hc %d (%d) on %s\n", wc.value(), wc.mode(),
        hc.value(), hc.mode(), mtarget ? mtarget->name().c_str() : "");

    int longest_row = 0;
    int max_height_on_row = 0;
    int items_on_row = 0;
    int y = 0, x = 0, taken_h = 0;

    bool single_row = (is_single_row() || wc.mode() == SizeConstraintMode::NoLimits)
                      && !(mmax_on_row < std::numeric_limits<int>::max());

    SizeConstraint cwc = wc.adapted_for_child();
    SizeConstraint chc = hc.adapted_for_child();

    TooSmallAccumulator too_small;
    for (auto _it = mitems.begin(); _it != mitems.end(); ++_it) {
        LayoutItem& item = *_it;
        if (item.skip())
            continue;
        if (single_row)
            cwc = wc.adapted_for_child(x);
        else
            chc = hc.adapted_for_child(taken_h);

        MeasureResults r;
        Size s;
        if (single_row || items_on_row < mmax_on_row) {
            r = item.measure(cwc, chc);
            s = too_small.consider(r);
            DBG("Flow Measure1: %d, %d %s\n", s.w(), s.h(), item.name());
        }

        if (!single_row && (items_on_row >= mmax_on_row || (x + s.w() > wc.value()))) {
            // New row.
            if (mhcenter) {
                // This could be done in layout().
                int xtransl = (cwc.value() - x) / 2;
                center_helper(mitems, _it, xtransl);
            }
            if (x > 0) {
                x -= mhorz_spacing;
            }
            longest_row = std::max(longest_row, x);
            items_on_row = 0;
            y += max_height_on_row + mvert_spacing;
            taken_h = y;
            x = 0;
            max_height_on_row = 0;

            // Need to remeasure every time?
            chc = hc.adapted_for_child(taken_h);
            r = item.measure(cwc, chc);
            s = too_small.consider(r);
            DBG("Flow Measure2: %d, %d %s\n", s.w(), s.h(), item.name());
        }
        item.set_allotted_rect(Rect(x, y, s));
        DBG("Item rect %d, %d, %d, %d\n", x, y, s.w(), s.h());
        items_on_row++;
        x += s.w() + mhorz_spacing;
        max_height_on_row = std::max(max_height_on_row, s.h());
    }

    if (x > 0) {
        x -= mhorz_spacing;
    }

    if (msingle_row && mhcenter && !mitems.empty()) {
        int xtransl = (wc.adapted_for_child().value() - x) / 2;
        center_helper(mitems, mitems.begin() + (mitems.size() - 1), xtransl);
    }
    longest_row = std::max(longest_row, x);

    y += max_height_on_row; // FIXME: double add?
    DBG("MEASURE returns %d, %d on %s\n", longest_row, y,
        mtarget ? mtarget->name().c_str() : "");
    return force_size_constraints(Size(longest_row, y), wc, hc, too_small);
}

Size FlowLayout::min_size_hint() {
    int wmax = 0, hmax = 0;
    for (auto& it : mitems) {
        Size msh = it.min_size_hint();
        if (mmax_on_row == 1) {
            wmax = std::max(msh.w(), wmax);
            hmax += msh.h() + mvert_spacing;
        }
        else if (msingle_row) {
            wmax += msh.w() + mhorz_spacing;
            hmax = std::max(msh.h(), hmax);
        }
        else {
            // FIXME: This is obviously not what we want.
            wmax = std::max(msh.w(), wmax);
            hmax = std::max(msh.h(), hmax);
        }
    }
    return Size(wmax, hmax);
}

void FlowLayout::add_item(const LayoutItemProxy& elem) {
    mitems.emplace_back(LayoutItem(elem));
    added_elem(*elem.elem());
}

void FlowLayout::remove_item(ILayoutElement& le) {
    auto it = find_elem(le);
    if (it != mitems.end()) {
        mitems.erase(it);
        removed_elem(le);
    }
}

void FlowLayout::set_horizontal_spacing(int s) {
    mhorz_spacing = s;
    if (mtarget)
        mtarget->request_layout();
}

void FlowLayout::set_vertical_spacing(int s) {
    mvert_spacing = s;
    if (mtarget)
        mtarget->request_layout();
}

void FlowLayout::set_single_row(bool s) {
    msingle_row = s;
    if (mtarget)
        mtarget->request_layout();
}

void FlowLayout::set_horizontally_centered(bool hcenter) {
    mhcenter = hcenter;
    if (mtarget)
        mtarget->request_layout();
}

void FlowLayout::set_max_on_row(int mor) {
    mmax_on_row = mor;
    if (mtarget)
        mtarget->request_layout();
}

}
