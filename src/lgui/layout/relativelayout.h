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


#ifndef LGUI_RELATIVELAYOUT_H
#define LGUI_RELATIVELAYOUT_H

#include "layout.h"
#include "layoutitemcontainerbase.h"
#include <vector>
#include <unordered_map>

#include "relativelayouthelpers.h"

namespace lgui {


/** A layout that allows you to specify constraints on its items relative to  other items or the parent.
 *
 *  You can either use percentages of the parent's width and height to position children within the parent.
 *
 *  Or you can position items relative to each other, i.e. to the left, to the right, above or below
 *  or align them on the left, right, top, bottom respectively. See dtl::RelativeLayoutConstraints::Constraint.
 *
 *  When you specifiy constraints for both the left and right (or top and bottom) of an item,
 *  it will be resized ("stretched") to fit. If you do not want that, only specify one constraint, not both.
 *
 *  The dependencies specified here cannot be cyclic, so they have to form a directed-acyclic-graph (DAG).
 *  However, since the layout actually constructs one horizontal and one vertical graph, horizontal and
 *  vertical constraints are independent. So you *may* actually specify A left-of B and B above A without
 *  problems.
 *
 *  Usually, you'd want to specify at least one horizontal *and* one vertical constraint for each item.
 *
 *  You need to add items in order to be able to refer to them as dependencies. You can also add constraints
 *  later.
 *
 *  An initializer-list syntax can be used to add items to the %RelativeLayout and to specify constraints at
 *  the same time.
 *  Example:
 *  \code
 *  rel_layout.add_item(widget1,   { lgui::RelativeLayout::Constraint::AlignParentRight });
 *
 *  rel_layout.add_item(widget2, { { lgui::RelativeLayout::Constraint::AlignParentBottom },
 *                                 { lgui::RelativeLayout::Constraint::AlignLeft, widget1 },
 *                                 { lgui::RelativeLayout::Constraint::AlignRight, widget1 } });
 *
 *  rel_layout.add_constraint(widget1, lgui::RelativeLayout::Constraint::Above, widget2);
 *  \endcode
 * This way, `widget1` will be "anchored" to the parent's right and `widget2` to the parent's bottom.
 * `widget1` will be placed above `widget2` and `widget2` will be made exactly as wide as `widget1`
 * (you could also specify `lgui::RelativeLayout::Constraint::AlignParentRight` for `widget2`, too, instead
 *  of `{ lgui::RelativeLayout::Constraint::AlignRight, widget1 }`.
 *
 * @note Every item can only have one left-of, above, align-top, ... relative constraint!
 *      \n
 *      The add_constraints() and add_constraint() methods will *overwrite* any previous setting; there are,
 *      therefore, no separate set_... methods.
 *      \n
 *      %RelativeLayout is bad at dealing with width-for-height situations, as all horizontal measurements are
 *      done first, granting full vertical space at first. In the second pass, horizontal space is fixed and
 *      vertical space may be drastically reduced as opposed to the first measurement. This can cause glitches,
 *      e.g. with certain configurations of ScrollArea.
 */
class RelativeLayout : public LayoutItemContainerBase2<dtl::RelativeLayoutItem> {
    public:
        using Constraint = dtl::RelativeLayoutConstraints::Constraint;

        /** Helper class for passing constraints into RelativeLayout.
         *  See RelativeLayout::add_item() RelativeLayout::add_constraints(), and
         *  RelativeLayout::add_constraint().
         *  Use {} initializers instead of calling the c'tor explicitly.
         */
        class ConstraintArg {
                friend class RelativeLayout;

            public:
                ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c, ILayoutElement& dep_elem);
                ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c, float perc);
                ConstraintArg(dtl::RelativeLayoutConstraints::Constraint c);
            private:
                dtl::RelativeLayoutConstraints::Constraint mc;
                ILayoutElement* mdep_elem;
                float mperc;
        };


        RelativeLayout();

        /** Add an item with several constraints given as an initializer list. */
        void add_item(const LayoutItemProxy& le, std::initializer_list<ConstraintArg> constraint_list);
        /** Add an item with several constraints given as a vector. */
        void add_item(const LayoutItemProxy& le, const std::vector<ConstraintArg>& constraint_list);

        /** Add an item with one constraint. */
        void add_item(const LayoutItemProxy& le, const ConstraintArg& constraint);

        /** Add an item with one constraint relative to another item. */
        void add_item(const LayoutItemProxy& le, Constraint c, ILayoutElement& dep_elem);
        /** Add an item with one constraint relative to the parent's dimensions. */
        void add_item(ILayoutElement& le, Constraint c, float perc);
        /** Add an item with a constraint relative to the parent's dimensions. */
        void add_item(const LayoutItemProxy& le, Constraint c);

        /** Shortcut to add an item with two constraints: Constraint::AlignLeftParentPerc and
         *  Constraint::AlignTopParentPerc. */
        void add_item_lt(const LayoutItemProxy& le, float left, float top);
        /** Shortcut to add an item with two constraints: Constraint::AlignRightParentPerc and
         *  Constraint::AlignBottomParentPerc. */
        void add_item_rb(const LayoutItemProxy& le, float right, float bottom);
        /** Shortcut to add an item with all four ...Perc constraints. */
        void add_item_ltrb(const LayoutItemProxy& le, float left, float top, float right, float bottom);

        /** Unset a specific constraint for one item. */
        void remove_constraint(const ILayoutElement& le, Constraint c);
        /** Clear all constraints for one item. */
        void clear_constraints(const ILayoutElement& le);

        /** Add multiple constraints given as initializer list (may overwrite existing ones!). */
        void add_constraints(const ILayoutElement& le, std::initializer_list<ConstraintArg> contraint_list);
        /** Add multiple constraints given as a vector (may overwrite existing ones!). */
        void add_constraints(const ILayoutElement& le, const std::vector<ConstraintArg>& constraint_list);

        /** Add one constraint (an existing setting might be overwritten!). */
        void add_constraint(const ILayoutElement& le, const ConstraintArg& a);

        /** Add one constraint relative to another item (an existing setting might be overwritten!). */
        void add_constraint(const ILayoutElement& le, Constraint c, ILayoutElement& dep_elem);
        /** Add one constraint relative to the parent's dimensions (an existing setting might be overwritten!). */
        void add_constraint(const ILayoutElement& le, Constraint c, float perc);
        /** Add one constraint relative to the parent's dimensions (an existing setting might be overwritten!). */
        void add_constraint(const ILayoutElement& le, Constraint c);

        void remove_all() override;

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;
        void do_layout(const Rect& r) override;

    private:
        void update_sorted_items();
        void recreate_id_map();
        void apply_horizontal_size_constraints(dtl::RelativeLayoutItem& item, int my_width) const;
        void apply_vertical_size_constraints(dtl::RelativeLayoutItem& item, int my_height) const;

        void check_and_add_arg_to_constraints(dtl::RelativeLayoutConstraints& constraints,
                                              const ConstraintArg& arg) const;

        void do_add_item(const LayoutItemProxy& le, const dtl::RelativeLayoutConstraints& constraints);
        dtl::RelativeLayoutItem* get_dependency(const dtl::RelativeLayoutItem& item, Constraint c) const;
        dtl::RelativeLayoutItem* get_item_for_id(dtl::RelativeLayoutElementId id) const;

        static SizeConstraint get_child_size_constraint(int child_begin, int child_end, int parent_wh);
        static MeasureResults measure_item_horizontally(dtl::RelativeLayoutItem& item, int my_width,
                                                        int my_height);
        static MeasureResults measure_item(dtl::RelativeLayoutItem& item, int my_width, int my_height);
        static bool position_item_horizontally(dtl::RelativeLayoutItem& item, int my_width,
                                               bool wrap_content_width);
        static bool position_item_vertically(dtl::RelativeLayoutItem& item, int my_height,
                                             bool wrap_content_height);
        static void center_item_horizontally(dtl::RelativeLayoutItem& item, int width);
        static void center_item_vertically(dtl::RelativeLayoutItem& item, int my_height);

        void _purge_removed_widget(dtl::RelativeLayoutItem& item) override;

        std::unordered_map<dtl::RelativeLayoutElementId, dtl::RelativeLayoutItem*> mid_map;
        std::vector<dtl::RelativeLayoutItem*> msorted_horz_items, msorted_vert_items;
        dtl::RelativeLayoutItemSorter msorter;
        SizeConstraint mlast_wc, mlast_hc;

        dtl::RelativeLayoutElementId mnext_item_id;
        bool mitems_changed;
};


}

#endif // LGUI_RELATIVELAYOUT_H

