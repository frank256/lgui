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

#include "boxlayout.h"
#include "platform/error.h"

#ifdef DEBUG_LAYOUT
#define DBG(...) debug("BoxLayout: " __VA_ARGS__)
#else
#define DBG(...)
#endif

namespace lgui {

BoxLayout::BoxLayout(Orientation o)
        : morientation(o),
          mdefault_alignment(Align::Default) {}

MeasureResults BoxLayout::measure(SizeConstraint wc, SizeConstraint hc) {
    mlast_wc = wc;
    mlast_hc = hc;

    DBG("MEASURE CALLED for %s with wc %d (%d), hc %d (%d) on %s\n", morientation == Horizontal ? "horz" : "vert",
        wc.value(), wc.mode(), hc.value(), hc.mode(), mtarget ? mtarget->name().c_str() : "");

    int sum_primary = 0, max_secondary = 0;
    int stretch_sum = 0;

    TooSmallAccumulator too_small;

    SizeConstraintMode primary_dim_mode = morientation == Horizontal ? wc.mode() : hc.mode();
    SizeConstraint secondary_constraint = (morientation == Horizontal) ? hc : wc;
    if (secondary_constraint.mode() == SizeConstraintMode::Exactly)
        secondary_constraint = SizeConstraint(secondary_constraint.value(), SizeConstraintMode::Maximum);

    for (auto& li : mitems) {
        if (li.skip())
            continue;
        if (li.stretch() > 0 && primary_dim_mode != SizeConstraintMode::NoLimits) {
            // try min_size() here if not EXACTLY?
            stretch_sum += li.stretch();
        }
        else {
            // also done for stretch widgets if we're in NoLimits mode
            SizeConstraint measure_wc, measure_hc;
            if (morientation == Horizontal) {
                measure_wc = wc.adapted_for_child(sum_primary);
                measure_hc = secondary_constraint;
            }
            else {
                measure_hc = hc.adapted_for_child(sum_primary);
                measure_wc = secondary_constraint;
            }
            MeasureResults r = li.measure(measure_wc, measure_hc);
            Size s = too_small.consider(r);
            sum_primary += primary_dim(s);
            DBG("sum_primary: %d, pds: %d, sds: %d, wc: %d, hc: %d (after %s)\n", sum_primary, primary_dim(s),
                secondary_dim(s), wc.value(), hc.value(), li.name());
            max_secondary = std::max(max_secondary, secondary_dim(s));
            li.set_allotted_size(s);
        }
    }

    if (stretch_sum > 0 && primary_dim_mode != SizeConstraintMode::NoLimits) {
        int fixed_sum_primary = sum_primary;

        int prim_available;
        if (morientation == Horizontal)
            prim_available = wc.value();
        else
            prim_available = hc.value();

        sum_primary = prim_available; // We'll use all the space to stretch.

        prim_available -= fixed_sum_primary;

        DBG("prim-available: %d\n", prim_available);

        for (auto& li : mitems) {
            if (li.skip())
                continue;
            if (li.stretch() > 0) {
                float prop = float(li.stretch()) / stretch_sum;
                int primary = prop * prim_available;

                stretch_sum -= li.stretch();
                prim_available -= primary;

                DBG("primary: %d (prop: %f)\n", primary, prop);
                MeasureResults r;
                if (morientation == Horizontal) {
                    r = li.measure(SizeConstraint(primary, SizeConstraintMode::Exactly), secondary_constraint);
                }
                else {
                    r = li.measure(secondary_constraint, SizeConstraint(primary, SizeConstraintMode::Exactly));
                }
                Size s = too_small.consider(r);
                max_secondary = std::max(max_secondary, secondary_dim(s));
                DBG("allotted size: %d, %d for %s\n", s.w(), s.h(), li.name());
                li.set_allotted_size(s);
            }
        }

    }

    MeasureResults result = force_size_constraints(make_size(sum_primary, max_secondary), wc, hc, too_small);
    DBG("MEASURE returns %d, %d on %s\n", result.w(), result.h(), mtarget ? mtarget->name().c_str() : "");

    return result;
}

Size BoxLayout::min_size_hint() {
    int sum_primary = 0, max_secondary = 0;
    for (auto& li : mitems) {
        if (li.skip())
            continue;
        Size s = li.min_size_hint();
        sum_primary += primary_dim(s);
        max_secondary = std::max(max_secondary, secondary_dim(s));
    }
    return make_size(sum_primary, max_secondary);
}

void BoxLayout::do_layout(const Rect& r) {
    if (!mtarget)
        return;

    Size ts = r.size();
    SizeConstraint wc = SizeConstraint(ts.w(), SizeConstraintMode::Exactly);
    SizeConstraint hc = SizeConstraint(ts.h(), SizeConstraintMode::Exactly);

    // Need remeasure? (fixme: correct?)
    if (mtarget->needs_relayout() ||
        (wc != mlast_wc || hc != mlast_hc)) {
        measure(wc, hc);
    }

    int prim = 0;
    int target_sec = secondary_dim(ts);

    for (auto& li : mitems) {
        if (li.skip())
            continue;
        Size lis = li.allotted_size();

        int sec = 0;

        if (morientation == Horizontal) {
            li.layout(Rect(r.x() + prim, r.y() + sec, lis.w(), target_sec));
            prim += li.allotted_size().w();
        }
        else {
            li.layout(Rect(r.x() + sec, r.y() + prim, target_sec, lis.h()));
            prim += li.allotted_size().h();
        }
    }
}

void BoxLayout::add_item(const LayoutItemProxy& elem, int stretch) {
    mitems.emplace_back(dtl::BoxLayoutItem(morientation, elem, stretch, mdefault_alignment));

    added_elem(*elem.elem());
}

void BoxLayout::add_spacing(int spacing) {
    ASSERT(spacing > 0);

    // Construct an empty item with margin
    Margin m;
    if (morientation == Horizontal) {
        m.set_left(spacing / 2 + spacing % 2);
        m.set_right(spacing / 2);
    }
    else {
        m.set_top(spacing / 2 + spacing % 2);
        m.set_bottom(spacing / 2);
    }

    mitems.emplace_back(dtl::BoxLayoutItem(morientation, {m}, 0, 0));

    if (mtarget && update_on_child_add_remove())
        mtarget->request_layout();
}

void BoxLayout::add_stretch(int stretch) {
    ASSERT(stretch > 0);

    mitems.emplace_back(dtl::BoxLayoutItem(morientation, {Margin()}, stretch, 0));

    if (mtarget && update_on_child_add_remove())
        mtarget->request_layout();
}

void BoxLayout::set_default_alignment(int align) {
    ASSERT(align != Align::Default);
    mdefault_alignment = align;
}

}
