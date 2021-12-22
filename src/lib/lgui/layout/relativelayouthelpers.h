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


#ifndef LGUI_RELATIVELAYOUTHELPERS_H
#define LGUI_RELATIVELAYOUTHELPERS_H

#include <array>
#include <vector>
#include <unordered_map>
#include "layoutitem.h"


namespace lgui {

namespace dtl {

using RelativeLayoutElementId = unsigned long;


class RelativeLayoutConstraints {
    public:
        RelativeLayoutConstraints();

        /** Types of constraints to be used for RelativeLayout.
         *  Those ending with ...Perc take a float value (called `p`) between 0.0 and 1.0 as parameters;
         *  AlignLeft, AlignRight, AlignTop, AlignBottom, LeftOf, RightOf, Above, and Below take another item
         *  as parameter (called `dep`). The others go without parameters. */
        enum Constraint {
                          /** The left edge of the item should be fixed at p% of the parent width. */
                          AlignLeftParentPerc,
                          /** The right edge of the item should be fixed at p% of the parent width. */
                          AlignRightParentPerc,
                          /** The top edge of the item should be fixed at p% of the parent height. */
                          AlignTopParentPerc,
                          /** The bottom edge of the item should be fixed at p% of the parent height. */
                          AlignBottomParentPerc,
                          /** The item should be centered horizontally within its parent. */
                          HCenterInParent,
                          /** The item should be centered vertically within its parent. */
                          VCenterInParent,
                          /** The left edge of the item should be aligned with the left edge of dep. */
                          AlignLeft,
                          /** The right edge of the item should be aligned with the right edge of dep. */
                          AlignRight,
                          /** The top edge of the item should be aligned with the top edge of dep. */
                          AlignTop,
                          /** The bottom edge of the item should be aligned with the bottom edge of dep. */
                          AlignBottom,
                          /** The right edge of the item should be aligned with the left edge of dep. */
                          LeftOf,
                          /** The left edge of the item should be aligned with the right edge of dep. */
                          RightOf,
                          /** The bottom edge of the item should be aligned with the top edge of dep. */
                          Above,
                          /** The top edge of the item should be aligned with the bottom edge of dep. */
                          Below,
                          /** (internal, do not use) */
                          NoSavedConstraints,
                          // Extra constraints for convenience which are translated into others:
                          /** Shortcut for AlignLeftParentPerc with p=0.0 */
                          AlignParentLeft,
                          /** Shortcut for AlignRightParentPerc with p=1.0 */
                          AlignParentRight,
                          /** Shortcut for AlignTopParentPerc with p=0.0 */
                          AlignParentTop,
                          /** Shortcut for AlignBottomParentPerc with p=1.0 */
                          AlignParentBottom,
                          /** Apply both HCenterInParent and VCenterInParent. */
                          CenterInParent
                        };

        void add_constraint(Constraint c);
        void add_constraint(Constraint c, float perc);
        void add_constraint(Constraint c, RelativeLayoutElementId dependency);
        RelativeLayoutElementId get_constraint(Constraint c) const;
        float get_perc_constraint(Constraint c) const;

        void remove(Constraint c);
        void clear_constraints();
        void purge_id(RelativeLayoutElementId id);

        using ConstraintFilterSet = std::array<Constraint, 7>;
        static const ConstraintFilterSet& get_horizontal_constraints() { return HORIZONTAL_CONSTRAINTS; }
        static const ConstraintFilterSet& get_vertical_constraints() { return VERTICAL_CONSTRAINTS; }

        static bool does_constraint_need_argument(Constraint c);
        static bool does_constraint_need_dependency_argument(Constraint c);
        static bool does_constraint_need_perc_argument(Constraint c);
    private:

        using ConstraintsArray = std::array<RelativeLayoutElementId, NoSavedConstraints>;
        ConstraintsArray mconstraints;
        std::array<float, 4> mpercs;

        static const ConstraintFilterSet HORIZONTAL_CONSTRAINTS, VERTICAL_CONSTRAINTS;
};

class RelativeLayoutPosition : public Padding {
    public:
        RelativeLayoutPosition() :
            Padding(REL_POS_NOT_SET) {}
        bool is_left_set() const { return left() != REL_POS_NOT_SET; }
        bool is_right_set() const { return right() != REL_POS_NOT_SET; }
        bool is_top_set() const { return top() != REL_POS_NOT_SET; }
        bool is_bottom_set() const { return bottom() != REL_POS_NOT_SET; }

        static const int REL_POS_NOT_SET;
};

// Wrapper for a layout item and an ID, starting at 2
class RelativeLayoutItem : public LayoutItem {
    public:

        RelativeLayoutItem(const LayoutItemProxy& le, const RelativeLayoutConstraints& constraints,
                           RelativeLayoutElementId id)
            : LayoutItem(le), mconstraints(constraints), mid(id) {}

        RelativeLayoutElementId id() const { return mid; }
        const RelativeLayoutConstraints& constraints() const { return mconstraints; }
        RelativeLayoutConstraints& constraints() { return mconstraints; }

        const RelativeLayoutPosition& rel_pos() const { return mrel_pos; }
        RelativeLayoutPosition& rel_pos() { return mrel_pos; }

    private:
        RelativeLayoutPosition mrel_pos;
        RelativeLayoutConstraints mconstraints;
        RelativeLayoutElementId mid;
};


class RelativeLayoutItemSorter {
    public:
        void clear();
        void prepare_for(size_t no_items);
        // Insert all at once?
        void add(RelativeLayoutItem& item);
        void retrieve_sorted_list(const RelativeLayoutConstraints::ConstraintFilterSet& filter,
                                  std::vector<RelativeLayoutItem*>& insert_into);
    private:
        struct GraphNode {
            GraphNode(RelativeLayoutItem* item) {
                this->item = item;
            }

            RelativeLayoutItem* item;
            std::vector <GraphNode*> dependencies;
            std::vector <GraphNode*> dependents;
        };
        std::unordered_map <RelativeLayoutElementId, GraphNode> mmap;

        void clear_graph();
        void build_graph(const RelativeLayoutConstraints::ConstraintFilterSet& filter);
        std::vector<GraphNode*> collect_roots();
};

}
}
#endif // LGUI_RELATIVELAYOUTHELPERS_H
