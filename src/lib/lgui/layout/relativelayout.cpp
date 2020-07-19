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

/*
The RelativeLayout has been inspired by the RelativeLayout of the Android operating system.
Here is the copyright notice from the relevant Android Java file:
-------------------------------------------------------------------------------------------
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
-------------------------------------------------------------------------------------------
*/


#include "relativelayout.h"
#include "lgui/platform/error.h"
#include <algorithm>
#include <cmath>

#ifdef DEBUG_LAYOUT
#define DBG(...) debug("RelativeLayout: " __VA_ARGS__)
#else
#define DBG(...)
#endif


namespace lgui {

RelativeLayout::RelativeLayout()
        : mnext_item_id(2), // 0 and 1 reserved
          mitems_changed(true) {}

void RelativeLayout::check_and_add_arg_to_constraints(dtl::RelativeLayoutConstraints& constraints,
                                                      const ConstraintArg& arg) const {
    if (dtl::RelativeLayoutConstraints::does_constraint_need_dependency_argument(arg.mc)) {
        ILayoutElement* dep_item = arg.mdep_elem;
        ASSERT(dep_item);
        if (dep_item) {
            auto it = std::find_if(mitems.begin(), mitems.end(),
                                   [dep_item](const dtl::RelativeLayoutItem& item) {
                                       return item.layout_element() == dep_item;
                                   });

            ASSERT_MSG(it != mitems.end(), "RelativeLayout: constraining layout element not present!");
            constraints.add_constraint(arg.mc, it->id());
        }
    }
    else if (dtl::RelativeLayoutConstraints::does_constraint_need_perc_argument(arg.mc)) {
        constraints.add_constraint(arg.mc, arg.mperc);
    }
    else {
        constraints.add_constraint(arg.mc);
    }
}

void RelativeLayout::add_item(const LayoutItemProxy& le, std::initializer_list<ConstraintArg> constraint_list) {
    dtl::RelativeLayoutConstraints constraints;
    for (const ConstraintArg& a : constraint_list) {
        check_and_add_arg_to_constraints(constraints, a);
    }
    do_add_item(le, constraints);
}

void RelativeLayout::add_item(const LayoutItemProxy& le, const std::vector<ConstraintArg>& constraint_list) {
    dtl::RelativeLayoutConstraints constraints;
    for (const ConstraintArg& a : constraint_list) {
        check_and_add_arg_to_constraints(constraints, a);
    }
    do_add_item(le, constraints);
}

void RelativeLayout::add_item(const LayoutItemProxy& le, const ConstraintArg& constraint) {
    dtl::RelativeLayoutConstraints constraints;
    check_and_add_arg_to_constraints(constraints, constraint);
    do_add_item(le, constraints);
}


void RelativeLayout::add_item(const LayoutItemProxy& le, RelativeLayout::Constraint c, ILayoutElement& dep_elem) {
    add_item(le, {c, dep_elem});
}

void RelativeLayout::add_item(const LayoutItemProxy& le, RelativeLayout::Constraint c) {
    add_item(le, ConstraintArg{c});
}

void RelativeLayout::add_item(ILayoutElement& le, RelativeLayout::Constraint c, float perc) {
    add_item(le, {c, perc});
}

void RelativeLayout::add_item_lt(const LayoutItemProxy& le, float left, float top) {
    add_item(le, {{Constraint::AlignLeftParentPerc, left},
            {Constraint::AlignTopParentPerc, top},
    });
}

void RelativeLayout::add_item_rb(const LayoutItemProxy& le, float right, float bottom) {
    add_item(le, {{Constraint::AlignRightParentPerc, right},
            {Constraint::AlignBottomParentPerc, bottom},
    });
}

void RelativeLayout::add_item_ltrb(const LayoutItemProxy& le, float left, float top, float right, float bottom) {
    add_item(le, {{Constraint::AlignLeftParentPerc, left},
            {Constraint::AlignTopParentPerc, top},
            {Constraint::AlignRightParentPerc, right},
            {Constraint::AlignBottomParentPerc, bottom},
    });
}

void RelativeLayout::remove_constraint(const ILayoutElement& le, RelativeLayout::Constraint c) {
    auto it = find_elem(le);
    ASSERT_MSG(it != mitems.end(), "Item not found in RelativeLayout.");
    if (it != mitems.end()) {
        mitems_changed = true;
        it->constraints().remove(c);
    }
}

void RelativeLayout::clear_constraints(const ILayoutElement& le) {
    auto it = find_elem(le);
    ASSERT_MSG(it != mitems.end(), "Item not found in RelativeLayout.");
    if (it != mitems.end()) {
        mitems_changed = true;
        it->constraints().clear_constraints();
    }
}


void RelativeLayout::add_constraint(const ILayoutElement& le, const ConstraintArg& a) {
    auto it = find_elem(le);
    ASSERT_MSG(it != mitems.end(), "Item not found in RelativeLayout.");
    if (it != mitems.end()) {
        check_and_add_arg_to_constraints(it->constraints(), a);
        mitems_changed = true;
    }
}


void RelativeLayout::add_constraint(const ILayoutElement& le, RelativeLayout::Constraint c,
                                    ILayoutElement& dep_elem) {
    add_constraint(le, {c, dep_elem});
}

void RelativeLayout::add_constraint(const ILayoutElement& le, RelativeLayout::Constraint c, float perc) {
    add_constraint(le, {c, perc});
}

void RelativeLayout::add_constraint(const ILayoutElement& le, RelativeLayout::Constraint c) {
    add_constraint(le, ConstraintArg{c});
}


void RelativeLayout::add_constraints(const ILayoutElement& le, std::initializer_list<ConstraintArg> contraint_list) {
    auto it = find_elem(le);
    ASSERT_MSG(it != mitems.end(), "Item not found in RelativeLayout.");
    if (it != mitems.end()) {
        for (const auto& a : contraint_list) {
            check_and_add_arg_to_constraints(it->constraints(), a);
        }
        mitems_changed = true;
    }
}

void RelativeLayout::add_constraints(const ILayoutElement& le, const std::vector<ConstraintArg>& constraint_list) {
    auto it = find_elem(le);
    ASSERT_MSG(it != mitems.end(), "Item not found in RelativeLayout.");
    if (it != mitems.end()) {
        for (const auto& a : constraint_list) {
            check_and_add_arg_to_constraints(it->constraints(), a);
        }
        mitems_changed = true;
    }
}

void RelativeLayout::do_add_item(const LayoutItemProxy& le, const dtl::RelativeLayoutConstraints& constraints) {
    // Let's see whether vector has been moved...
    void* data_ptr = mitems.data();
    mitems.emplace_back(dtl::RelativeLayoutItem(le, constraints, mnext_item_id++));
    void* new_data_ptr = mitems.data();
    if (data_ptr != new_data_ptr) {
        // Vector moved, pointers now invalid.
        recreate_id_map();
    }
    else {
        dtl::RelativeLayoutElementId id = mitems.back().id();
        mid_map.insert(std::make_pair(id, &mitems.back()));
    }
    mitems_changed = true;
}

void RelativeLayout::recreate_id_map() {
    mid_map.clear();
    mid_map.reserve(mitems.size());
    for (dtl::RelativeLayoutItem& item : mitems)
        mid_map.insert(std::make_pair(item.id(), &item));
}

void RelativeLayout::remove_all() {
    LayoutItemContainerBase::remove_all();
    mid_map.clear();
    mnext_item_id = 2;
    msorted_horz_items.clear();
    msorted_vert_items.clear();
    msorter.clear();
    mitems_changed = true;
}


MeasureResults RelativeLayout::measure(SizeConstraint wc, SizeConstraint hc) {
    mlast_wc = wc;
    mlast_hc = hc;

    if (mitems_changed) {
        update_sorted_items();
        mitems_changed = false;
    }

    int my_width = (wc.mode() != SizeConstraintMode::NoLimits) ? wc.value() : -1;
    int my_height = (hc.mode() != SizeConstraintMode::NoLimits) ? hc.value() : -1;
    bool wrap_content_width = wc.mode() != SizeConstraintMode::Exactly;
    bool wrap_content_height = hc.mode() != SizeConstraintMode::Exactly;
    bool correct_horizontal = false;
    bool correct_vertical = false;

    DBG("MEASURE called with my_width=%d, my_height=%d, wcw=%d, wch=%d\n",
        my_width, my_height, wrap_content_width, wrap_content_height);

    TooSmallAccumulator too_small;

    for (dtl::RelativeLayoutItem* hitem : msorted_horz_items) {
        DBG("item_name: %s\n", hitem->name());
        if (!hitem->skip()) {
            apply_horizontal_size_constraints(*hitem, my_width);
            MeasureResults r = measure_item_horizontally(*hitem, my_width, my_height);
            Size s = too_small.consider(r);
            DBG("measured horizontally: %d, %d\n", s.w(), s.h());
            hitem->set_allotted_size(s);
            correct_horizontal |= position_item_horizontally(*hitem, my_width, wrap_content_width);
        }
    }

    int width = 0;
    int height = 0;

    for (dtl::RelativeLayoutItem* vitem : msorted_vert_items) {
        DBG("item_name: %s\n", vitem->name());
        if (!vitem->skip()) {
            apply_vertical_size_constraints(*vitem, my_height);
            MeasureResults r = measure_item(*vitem, my_width, my_height);
            Size s = too_small.consider(r);
            vitem->set_allotted_size(s);
            correct_vertical |= position_item_vertically(*vitem, my_height, wrap_content_height);

            if (wrap_content_width) {
                width = std::max(width, vitem->rel_pos().right());
            }
            if (wrap_content_height) {
                height = std::max(height, vitem->rel_pos().bottom());
            }
            DBG("Measured item -> %d, %d -> %d, %d\n", s.w(), s.h(), width, height);
        }

    }
    // This might be done in do_layout, but there would be the need to synchronize then.

    if (wrap_content_width) {
        MeasureResult r = force_size_constraint(width, wc);
        width = too_small.consider_width(r);
        if (correct_horizontal) {
            for (dtl::RelativeLayoutItem* hitem : msorted_horz_items) {
                if (!hitem->skip()) {
                    if (hitem->constraints().get_constraint(dtl::RelativeLayoutConstraints::HCenterInParent))
                        center_item_horizontally(*hitem, width);
                    else if (hitem->constraints().get_constraint(dtl::RelativeLayoutConstraints::AlignParentRight)) {
                        int item_w = hitem->allotted_size().w();
                        hitem->rel_pos().set_left(width - item_w);
                        hitem->rel_pos().set_right(width);
                    }
                    hitem->set_allotted_pos(hitem->rel_pos().left_top_offs());
                }
            }
        }
    }
    else {
        width = my_width;
    }

    if (wrap_content_height) {
        MeasureResult r = force_size_constraint(height, hc);
        height = too_small.consider_height(r);
        if (correct_vertical) {
            for (dtl::RelativeLayoutItem* vitem : msorted_vert_items) {
                if (!vitem->skip()) {
                    if (vitem->constraints().get_constraint(dtl::RelativeLayoutConstraints::VCenterInParent))
                        center_item_vertically(*vitem, height);
                    else if (vitem->constraints().get_constraint(dtl::RelativeLayoutConstraints::AlignParentBottom)) {
                        int item_h = vitem->allotted_size().h();
                        vitem->rel_pos().set_top(height - item_h);
                        vitem->rel_pos().set_bottom(height);
                    }
                    vitem->set_allotted_pos(vitem->rel_pos().left_top_offs());
                }
            }
        }
    }
    else {
        height = my_height;
    }


//        for (dtl::RelativeLayoutItem& item : mitems) {
//            DBG("Item %d, %s, %d, %d, %d, %d\n", item.id(), item.name().c_str(), item.allotted_rect().x(),
//                     item.allotted_rect().y(), item.allotted_rect().w(), item.allotted_rect().h());
//        }

    DBG("MEASURE returning with %d, %d.\n", width, height);
    return MeasureResults(width, height);
}

Size RelativeLayout::min_size_hint() {
    if (mitems_changed) {
        update_sorted_items();
        mitems_changed = false;
    }

    for (dtl::RelativeLayoutItem* hitem : msorted_horz_items) {
        if (!hitem->skip()) {
            apply_horizontal_size_constraints(*hitem, -1);
            Size s = hitem->min_size_hint();
            hitem->set_allotted_size(s);
            position_item_horizontally(*hitem, -1, true);
        }
    }

    int width = 0;
    int height = 0;

    for (dtl::RelativeLayoutItem* vitem : msorted_vert_items) {
        if (!vitem->skip()) {
            apply_vertical_size_constraints(*vitem, -1);
            Size s = vitem->min_size_hint();
            vitem->set_allotted_size(s);
            position_item_vertically(*vitem, -1, true);
            width = std::max(width, vitem->rel_pos().right());
            height = std::max(height, vitem->rel_pos().bottom());
        }
    }
    return Size(width, height);
}

void RelativeLayout::do_layout(const Rect& r) {
    if (!mtarget)
        return;

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


void RelativeLayout::update_sorted_items() {
    DBG("Updating items...\n");
    msorted_horz_items.clear();
    msorted_vert_items.clear();
    msorter.clear();
    msorter.prepare_for(mitems.size());
    for (dtl::RelativeLayoutItem& item : mitems) {
        msorter.add(item);
    }
    msorter.retrieve_sorted_list(dtl::RelativeLayoutConstraints::get_horizontal_constraints(), msorted_horz_items);
    msorter.retrieve_sorted_list(dtl::RelativeLayoutConstraints::get_vertical_constraints(), msorted_vert_items);
}


static int perc_value(float perc, int v) {
    return std::round(v * perc);
}

void RelativeLayout::apply_horizontal_size_constraints(dtl::RelativeLayoutItem& item, int my_width) const {
    int left, right;
    left = right = dtl::RelativeLayoutPosition::REL_POS_NOT_SET;

    dtl::RelativeLayoutItem* dependency = nullptr;

    dependency = get_dependency(item, Constraint::LeftOf);
    if (dependency)
        right = dependency->rel_pos().left();

    dependency = get_dependency(item, Constraint::RightOf);
    if (dependency)
        left = dependency->rel_pos().right();

    dependency = get_dependency(item, Constraint::AlignLeft);
    if (dependency)
        left = dependency->rel_pos().left();

    dependency = get_dependency(item, Constraint::AlignRight);
    if (dependency)
        right = dependency->rel_pos().right();

    if (item.constraints().get_constraint(Constraint::AlignLeftParentPerc)) {
        float perc = item.constraints().get_perc_constraint(Constraint::AlignLeftParentPerc);
        if (my_width >= 0) {
            left = perc_value(perc, my_width);
        }
        else if (perc == 0.0)
            left = 0;
    }

    if (item.constraints().get_constraint(Constraint::AlignRightParentPerc)) {
        float perc = item.constraints().get_perc_constraint(Constraint::AlignRightParentPerc);
        if (my_width >= 0) {
            right = perc_value(perc, my_width);
        }
    }

    item.rel_pos().set_left(left);
    item.rel_pos().set_right(right);
}

void RelativeLayout::apply_vertical_size_constraints(dtl::RelativeLayoutItem& item, int my_height) const {
    int top, bottom;
    top = bottom = dtl::RelativeLayoutPosition::REL_POS_NOT_SET;

    dtl::RelativeLayoutItem* dependency = nullptr;

    dependency = get_dependency(item, Constraint::Above);
    if (dependency)
        bottom = dependency->rel_pos().top();

    dependency = get_dependency(item, Constraint::Below);
    if (dependency)
        top = dependency->rel_pos().bottom();

    dependency = get_dependency(item, Constraint::AlignTop);
    if (dependency)
        top = dependency->rel_pos().top();

    dependency = get_dependency(item, Constraint::AlignBottom);
    if (dependency)
        bottom = dependency->rel_pos().bottom();

    if (item.constraints().get_constraint(Constraint::AlignTopParentPerc)) {
        float perc = item.constraints().get_perc_constraint(Constraint::AlignTopParentPerc);
        if (my_height >= 0) {
            top = perc_value(perc, my_height);
        }
        else if (perc == 0.0)
            top = 0;
    }

    if (item.constraints().get_constraint(Constraint::AlignBottomParentPerc)) {
        float perc = item.constraints().get_perc_constraint(Constraint::AlignBottomParentPerc);
        if (my_height >= 0) {
            bottom = perc_value(perc, my_height);
        }
    }

    item.rel_pos().set_top(top);
    item.rel_pos().set_bottom(bottom);
}


MeasureResults RelativeLayout::measure_item_horizontally(dtl::RelativeLayoutItem& item, int my_width,
                                                         int my_height) {
    SizeConstraint wc = get_child_size_constraint(item.rel_pos().left(), item.rel_pos().right(), my_width);

    SizeConstraint hc;
    if (my_height < 0) {
        hc = SizeConstraint::no_limits();
    }
    else {
        hc = SizeConstraint(my_height, SizeConstraintMode::Maximum);
    }
    return item.measure(wc, hc);
}

MeasureResults RelativeLayout::measure_item(dtl::RelativeLayoutItem& item, int my_width, int my_height) {
    SizeConstraint wc = get_child_size_constraint(item.rel_pos().left(), item.rel_pos().right(), my_width);
    SizeConstraint hc = get_child_size_constraint(item.rel_pos().top(), item.rel_pos().bottom(), my_height);
    return item.measure(wc, hc);
}

// Return whether horizontal dimension needs correction (center / right alignment with size yet unknown).
bool RelativeLayout::position_item_horizontally(dtl::RelativeLayoutItem& item, int my_width,
                                                bool wrap_content_width) {
    bool need_correction = false;

    dtl::RelativeLayoutPosition& p = item.rel_pos();
    int width = item.allotted_size().w();

    if (!p.is_left_set() && p.is_right_set()) {
        p.set_left(p.right() - width);
    }
    else if (p.is_left_set() && !p.is_right_set()) {
        p.set_right(p.left() + width);
    }
    else if (!p.is_left_set() && !p.is_right_set()) {
        if (!wrap_content_width) {
            if (item.constraints().get_constraint(dtl::RelativeLayoutConstraints::HCenterInParent)) {
                center_item_horizontally(item, my_width);
            }
            else {
                p.set_left(0);
                p.set_right(p.left() + width);
            }
        }
        else {
            p.set_left(0);
            p.set_right(p.left() + width);
            need_correction = item.constraints().get_constraint(dtl::RelativeLayoutConstraints::HCenterInParent) ||
                              item.constraints().get_constraint(dtl::RelativeLayoutConstraints::AlignParentRight);
        }
    }
    item.set_allotted_pos(p.left_top_offs());
    return need_correction;
}

bool RelativeLayout::position_item_vertically(dtl::RelativeLayoutItem& item, int my_height,
                                              bool wrap_content_height) {
    bool need_correction = false;

    dtl::RelativeLayoutPosition& p = item.rel_pos();
    int height = item.allotted_size().h();

    if (!p.is_top_set() && p.is_bottom_set()) {
        p.set_top(p.bottom() - height);
    }
    else if (p.is_top_set() && !p.is_bottom_set()) {
        p.set_bottom(p.top() + height);
    }
    else if (!p.is_top_set() && !p.is_bottom_set()) {
        if (!wrap_content_height) {
            if (item.constraints().get_constraint(dtl::RelativeLayoutConstraints::VCenterInParent)) {
                center_item_vertically(item, my_height);
            }
            else {
                p.set_top(0);
                p.set_bottom(p.top() + height);
            }
        }
        else {
            p.set_top(0);
            p.set_bottom(p.top() + height);
            need_correction = item.constraints().get_constraint(dtl::RelativeLayoutConstraints::VCenterInParent) ||
                              item.constraints().get_constraint(dtl::RelativeLayoutConstraints::AlignParentBottom);
        }
    }
    item.set_allotted_pos(p.left_top_offs());
    return need_correction;
}

void RelativeLayout::center_item_horizontally(dtl::RelativeLayoutItem& item, int my_width) {
    int w = item.allotted_size().w();
    int left = (my_width - w) / 2;
    item.rel_pos().set_left(left);
    item.rel_pos().set_right(left + w);
}

void RelativeLayout::center_item_vertically(dtl::RelativeLayoutItem& item, int my_height) {
    int h = item.allotted_size().h();
    int top = (my_height - h) / 2;
    item.rel_pos().set_top(top);
    item.rel_pos().set_bottom(top + h);
}

void RelativeLayout::_purge_removed_widget(dtl::RelativeLayoutItem& item) {
    mid_map.erase(item.id());
    for (auto& li : mitems) {
        li.constraints().purge_id(item.id());
    }
    mitems_changed = true;
}


dtl::RelativeLayoutItem* RelativeLayout::get_dependency(const dtl::RelativeLayoutItem& item,
                                                        RelativeLayout::Constraint c) const {
    dtl::RelativeLayoutElementId id = item.constraints().get_constraint(c);
    dtl::RelativeLayoutItem* dep = get_item_for_id(id);
    if (dep) {
        while (dep->skip()) {
            id = dep->constraints().get_constraint(c);
            dep = get_item_for_id(id);
            if (!dep)
                return nullptr;
        }
        return dep;
    }
    return nullptr;
}

dtl::RelativeLayoutItem* RelativeLayout::get_item_for_id(dtl::RelativeLayoutElementId id) const {
    if (id > 1) {
        if (mid_map.count(id) > 0)
            return mid_map.at(id);
    }
    return nullptr;
}

SizeConstraint RelativeLayout::get_child_size_constraint(int child_start, int child_end, int parent_wh) {
    static const int NOT_SET = dtl::RelativeLayoutPosition::REL_POS_NOT_SET;

    int child_wh = 0;
    SizeConstraintMode child_mode;

    if (parent_wh < 0) {
        // No limits set for us.
        if (child_start != NOT_SET && child_end != NOT_SET) {
            child_wh = std::max(child_end - child_start, 0);
            child_mode = SizeConstraintMode::Exactly;
        }
        else {
            child_wh = 0;
            child_mode = SizeConstraintMode::NoLimits;
        }
        return SizeConstraint(child_wh, child_mode);
    }

    int begin = (child_start != NOT_SET) ? child_start : 0;
    int end = (child_end != NOT_SET) ? child_end : parent_wh;

    // Will be negative if parent_wh has been negative.
    child_wh = end - begin;

    if ((child_start != NOT_SET && child_end != NOT_SET)) {
        child_mode = SizeConstraintMode::Exactly;
    }
    else {
        child_mode = SizeConstraintMode::Maximum;
    }
    return SizeConstraint(child_wh, child_mode);
}

RelativeLayout::ConstraintArg::ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c, ILayoutElement& dep_elem)
        : mc(c), mdep_elem(&dep_elem) {
    ASSERT(dtl::RelativeLayoutConstraints::does_constraint_need_dependency_argument(c));
    ASSERT(!dtl::RelativeLayoutConstraints::does_constraint_need_perc_argument(c));
}

RelativeLayout::ConstraintArg::ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c, float perc)
        : mc(c), mdep_elem(nullptr), mperc(perc) {
    ASSERT(!dtl::RelativeLayoutConstraints::does_constraint_need_dependency_argument(c));
    ASSERT(dtl::RelativeLayoutConstraints::does_constraint_need_perc_argument(c));
}

RelativeLayout::ConstraintArg::ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c)
        : mc(c), mdep_elem(nullptr), mperc(0.0) {
    ASSERT(!dtl::RelativeLayoutConstraints::does_constraint_need_dependency_argument(c));
    ASSERT(!dtl::RelativeLayoutConstraints::does_constraint_need_perc_argument(c));
}


}
