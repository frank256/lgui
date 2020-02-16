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


#include "relativelayouthelpers.h"

#include <limits>
#include <algorithm>
#include "platform/error.h"

namespace lgui {

namespace dtl {

const RelativeLayoutConstraints::ConstraintFilterSet RelativeLayoutConstraints::HORIZONTAL_CONSTRAINTS {
    {
        RelativeLayoutConstraints::AlignLeftParentPerc,
        RelativeLayoutConstraints::AlignRightParentPerc,
        RelativeLayoutConstraints::HCenterInParent,
        RelativeLayoutConstraints::AlignLeft,
        RelativeLayoutConstraints::AlignRight,
        RelativeLayoutConstraints::LeftOf,
        RelativeLayoutConstraints::RightOf
    }
};

const RelativeLayoutConstraints::ConstraintFilterSet RelativeLayoutConstraints::VERTICAL_CONSTRAINTS {
    {
        RelativeLayoutConstraints::AlignTopParentPerc,
        RelativeLayoutConstraints::AlignBottomParentPerc,
        RelativeLayoutConstraints::VCenterInParent,
        RelativeLayoutConstraints::AlignTop,
        RelativeLayoutConstraints::AlignBottom,
        RelativeLayoutConstraints::Above,
        RelativeLayoutConstraints::Below
    }
};

const int RelativeLayoutPosition::REL_POS_NOT_SET = std::numeric_limits<int>::max();

bool RelativeLayoutConstraints::does_constraint_need_argument(RelativeLayoutConstraints::Constraint c)
{
    return c < NoSavedConstraints && (c != HCenterInParent && c != VCenterInParent);
}

bool RelativeLayoutConstraints::does_constraint_need_dependency_argument(RelativeLayoutConstraints::Constraint c)
{
    return c >= AlignLeft && c <= Below;
}

bool RelativeLayoutConstraints::does_constraint_need_perc_argument(RelativeLayoutConstraints::Constraint c)
{
    return c >= AlignLeftParentPerc && c <= AlignBottomParentPerc;
}

RelativeLayoutConstraints::RelativeLayoutConstraints()
{
    clear_constraints();
}

void RelativeLayoutConstraints::add_constraint(RelativeLayoutConstraints::Constraint c)
{
    ASSERT(!does_constraint_need_argument(c));
    // Translate the extra constraints.
    switch (c) {
        case AlignParentLeft:
            c = AlignLeftParentPerc;
            mpercs[c]  = 0.0;
            break;
        case AlignParentTop:
            c = AlignTopParentPerc;
            mpercs[c]  = 0.0;
            break;
        case AlignParentRight:
            c = AlignRightParentPerc;
            mpercs[c]  = 1.0;
            break;
        case AlignParentBottom:
            c = AlignBottomParentPerc;
            mpercs[c]  = 1.0;
            break;
        case CenterInParent:
            c = HCenterInParent; // set below
            mconstraints[VCenterInParent] = true;
            break;
        default:
            break;
    }
    mconstraints[c] = 1;
}

void RelativeLayoutConstraints::add_constraint(RelativeLayoutConstraints::Constraint c, float perc)
{
    ASSERT(does_constraint_need_perc_argument(c));
    mconstraints[c] = 1;
    mpercs[c - Constraint::AlignLeftParentPerc] = perc;
}

void RelativeLayoutConstraints::add_constraint(RelativeLayoutConstraints::Constraint c, RelativeLayoutElementId id)
{
    ASSERT(does_constraint_need_dependency_argument(c));
    ASSERT(id > 1);
    mconstraints[c] = id;
}

RelativeLayoutElementId RelativeLayoutConstraints::get_constraint(RelativeLayoutConstraints::Constraint c) const
{
    return mconstraints[c];
}

float RelativeLayoutConstraints::get_perc_constraint(RelativeLayoutConstraints::Constraint c) const
{
    ASSERT(does_constraint_need_perc_argument(c));
    return mpercs[c - Constraint::AlignLeftParentPerc];
}

void RelativeLayoutConstraints::purge_id(RelativeLayoutElementId id)
{
    for (auto& c : mconstraints) {
        if (c == id)
            c = 0;
    }
}

void RelativeLayoutConstraints::clear_constraints()
{
    for (auto& c : mconstraints)
        c = 0;

    for (auto& p : mpercs)
        p = 0.0;
}

void RelativeLayoutConstraints::remove(RelativeLayoutConstraints::Constraint c)
{
    mconstraints[c] = 0;
}

void RelativeLayoutItemSorter::clear()
{
    mmap.clear();
}

void RelativeLayoutItemSorter::prepare_for(size_t no_items)
{
    mmap.reserve(no_items);
}

void RelativeLayoutItemSorter::add(RelativeLayoutItem& item)
{
    mmap.insert(std::make_pair(item.id(), GraphNode(&item)));
}

void RelativeLayoutItemSorter::retrieve_sorted_list(const RelativeLayoutConstraints::ConstraintFilterSet& filter,
                                                    std::vector<RelativeLayoutItem*>& insert_into)
{
    size_t no_inserted = 0;
    clear_graph();
    build_graph(filter);
    std::vector<RelativeLayoutItemSorter::GraphNode*> roots = collect_roots();
    while (!roots.empty()) {
        GraphNode* root = roots.back();
        roots.pop_back();

        insert_into.push_back(root->item);
        ++no_inserted;
        auto& dependents = root->dependents;
        for (auto& successor : dependents) {
            std::vector <GraphNode*>& dependencies = successor->dependencies;
            auto it = std::find(dependencies.begin(), dependencies.end(), root);
            ASSERT(it != dependencies.end());
            dependencies.erase(it);
            if (dependencies.empty())
                roots.push_back(successor);
        }
    }
    // Cyclic?
    ASSERT(no_inserted == mmap.size());
}

void RelativeLayoutItemSorter::clear_graph()
{
    for (auto& p : mmap) {
        p.second.dependencies.clear();
        p.second.dependents.clear();
    }
}

void RelativeLayoutItemSorter::build_graph(const RelativeLayoutConstraints::ConstraintFilterSet& filter)
{
    using Constraint = RelativeLayoutConstraints::Constraint;
    for (auto& p : mmap) {
        GraphNode& node = p.second;
        const RelativeLayoutConstraints& constraints = node.item->constraints();
        for (Constraint c : filter) {
            RelativeLayoutElementId id = constraints.get_constraint(c);
            if (id > 1) {
                GraphNode& dep = mmap.at(id);
                if (&dep == &node)
                    continue;
                dep.dependents.push_back(&node);
                node.dependencies.push_back(&dep);
            }
        }
    }

}

std::vector<RelativeLayoutItemSorter::GraphNode*> RelativeLayoutItemSorter::collect_roots()
{
    std::vector<RelativeLayoutItemSorter::GraphNode*> roots;
    for (auto& p : mmap) {
        if (p.second.dependencies.empty()) {
            roots.push_back(&p.second);
        }
    }
    return roots;
}

}

}
