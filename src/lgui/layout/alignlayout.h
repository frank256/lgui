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

#ifndef ALIGNLAYOUT_H
#define ALIGNLAYOUT_H

#include <vector>
#include "layoutitemcontainerbase.h"
#include "layoutitem.h"

namespace lgui {
namespace dtl {
    class AlignLayoutItem : public LayoutItem {
        public:
            AlignLayoutItem(const LayoutItemProxy& proxy, const Align& align)
                : LayoutItem(proxy), malign(align)
            {}

            const Align& align() const { return malign; }

        private:
            Align malign;
    };
}

/** Layout to just align widgets inside the parent. This is more of a dummy layout, may be used to easily
 *  center one child, for example. It will always take all the space it can for the target widget. */
class AlignLayout : public lgui::LayoutItemContainerBase<dtl::AlignLayoutItem, std::vector<dtl::AlignLayoutItem> >
{
    public:
        AlignLayout() = default;

        void add_item(const LayoutItemProxy& elem, const Align& align);

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        void do_layout(const Rect& r) override;

    private:
        static SizeConstraint get_horz_constraint(SizeConstraint wc, Align align);
        static SizeConstraint get_vert_constraint(SizeConstraint wc, Align align);
        SizeConstraint mlast_wc, mlast_hc;
};

}

#endif // ALIGNLAYOUT_H
