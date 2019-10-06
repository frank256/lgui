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

#ifndef LGUI_FLOWLAYOUT_H
#define LGUI_FLOWLAYOUT_H

#include "layoutitemcontainerbase.h"
#include <vector>

namespace lgui {
/** This simple flow layout will arrange widgets from top-to-bottom, left-to-right in
 *  a greedy manner. All widgets are offered all the horizontal space available unless
 *  set_single_row() has been called with true.
 *
 *  You can also configure the spacing between widgets, set a maximum number of widgets
 *  on one row or center the widgets horizontally.
 */
class FlowLayout : public LayoutItemContainerBase<LayoutItem, std::vector<LayoutItem>> {
    protected:
        void do_layout(const Rect& r) override;

    public:
        FlowLayout();

        ~FlowLayout() override = default;

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

        /** Add an item (widget or nested layout) to the layout. */
        void add_item(const LayoutItemProxy& elem);

        void remove_item(ILayoutElement& le);

        /** Return the horizontal spacing between widgets. */
        int horizontal_spacing() const { return mhorz_spacing; }

        /** Return the vertical spacing between widgets. */
        int vertical_spacing() const { return mvert_spacing; }

        /** Set the horizontal spacing between widgets. */
        void set_horizontal_spacing(int s);

        /** Set the vertical spacing between widgets. */
        void set_vertical_spacing(int s);

        /** Set whether the flow layout should keep its layout items on a single row. */
        void set_single_row(bool s);

        /** Return whether the flow layout keeps its layout items on a single row. */
        bool is_single_row() const { return msingle_row; }


        /** Return whether the flow layout centers its items horizontally on each row. */
        bool is_horizontally_centered() const { return mhcenter; }
        /** Set whether the flow layout will center its items horizontally on each row. */
        void set_horizontally_centered(bool hcenter);

        /** Return the maximum number of layout items allowed on one row. */
        int max_on_row() const { return mmax_on_row; }
        /** Set the maximum number of layout items allowed on one row. If set to 1, every item will reside
            on its own row. */
        void set_max_on_row(int mor);

    private:
        int mmax_on_row;
        int mhorz_spacing, mvert_spacing;
        bool mhcenter, msingle_row;
        SizeConstraint mlast_wc, mlast_hc;

        static void center_helper(std::vector<LayoutItem>& container,
                                  std::vector<LayoutItem>::iterator it,
                                  int offsx);

        static const int default_spacing = 4;
};

}

#endif // LGUI_FLOWLAYOUT_H
