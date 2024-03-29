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

#ifndef LGUI_SORTEDFLOWLAYOUT_H
#define LGUI_SORTEDFLOWLAYOUT_H

#include "flowlayout.h"
#include <functional>

namespace lgui {

/** Provide a sorted flow layout: layout elements are sorted according to some criterion.
 *  Sorting only happens on insertion and delete, you have to call invalidate_order() if you
 *  want to trigger a resort.
 *  @see FlowLayout
 */
class SortedFlowLayout : public FlowLayout {
    public:
        /** Comparator type to compare two layout elements. These can be widgets or whole nested layouts.
         */
        using LayoutElementComparator = std::function<bool(const ILayoutElement* a, const ILayoutElement* b)>;

        SortedFlowLayout() = default;
        explicit SortedFlowLayout(LayoutElementComparator&& cmp)
                : mcomp_func(cmp) {}

        /** Set the comparator. */
        void set_comparator(LayoutElementComparator&& cmp);

        /** Trigger a resort. */
        void invalidate_order();

    protected:
        void added_elem(ILayoutElement& elem) override;
        void removed_elem(ILayoutElement& elem) override;

    private:
        LayoutElementComparator mcomp_func;
};

}
#endif // LGUI_SORTEDFLOWLAYOUT_H
