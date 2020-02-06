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

#ifndef LGUI_SIMPLETABLELAYOUT_H
#define LGUI_SIMPLETABLELAYOUT_H

#include <vector>
#include "layoutitemcontainerbase.h"
#include "layoutitem.h"

namespace lgui {

namespace dtl {

/** Class representing an item in a SimpleTableLayout. */
class SimpleTableLayoutItem : public LayoutItem {
    public:
        SimpleTableLayoutItem(int col, int row, const LayoutItemProxy& le)
         : LayoutItem(le), mrow(row), mcol(col) {}

        int row() const { return mrow; }
        int col() const{ return mcol; }

    private:
        int mrow, mcol;
};

}

/** A simple table layout. Specify the number of rows and columns beforehand and then add items into
 *  cells using row and column coordinates. Every column / row will get as wide / high as the max. minimum
 *  size hint of items added. Surplus space is distributed according to the stretch values you can set
 *  for individual rows and columns. E.g. setting stretch values of 2 and 3 for two columns means the first
 *  column gets 2/5 of the remaining width, the second column 3/5.
 *  It is also possible to change item alignment and to set cell spacing.
 *  This is called a simple table layout because cells spanning several rows/columns are not yet supported.
 */
class SimpleTableLayout : public LayoutItemContainerBase<dtl::SimpleTableLayoutItem, std::vector<dtl::SimpleTableLayoutItem>> {
    public:
        /** C'tor. Specify the number of columns and rows you want to allocate. */
        SimpleTableLayout(int ncols, int nrows);

        /** Add an item to the cell indicated by `x`,`y`.  */
        void add_item(int x, int y, const LayoutItemProxy& le);

        void remove_item(ILayoutElement& elem);

        /** Removes the item in a specific cell.
         * @return true if an item has been encoutered that has been successfully removed */
        bool remove_item(int x, int y);

        /** Set a stretch value on a column. Surplus space will be distributed to this column according
         *  to its share of the stretch total (sum). */
        void set_column_stretch(int col, int stretch);
        /** Set a stretch value on a row. Surplus space will be distributed to this row according
         *  to its share of the stretch total (sum). */
        void set_row_stretch(int row, int stretch);
        /** Return the stretch value set on the column. */
        int column_stretch(int col) const;
        /** Return the stretch value set on the row. */
        int row_stretch(int row) const;

        /** Return the row spacing, i.e. space between rows. */
        int row_spacing() const { return mrow_spacing; }
        /** Return the column spacing, i.e. space between columns. */
        int column_spacing() const { return mcol_spacing; }
        /** Sets the column spacing, i.e. space between columns. */
        void set_column_spacing(int spacing);
        /** Sets the row spacing, i.e. space between rows. */
        void set_row_spacing(int spacing);

        /** Sets the minimum width of a column. This will always be allocated for the column. */
        void set_col_min_width(int col, int min_width);
        /** Return the minimum width of a column. */
        int col_min_width(int col) const;
        /** Sets the minimum height of a row. This will always be allocated for the row. */
        void set_row_min_height(int row, int min_height);
        /** Return the minimum height of a row. */
        int row_min_height(int row) const;

        /** Return the alignment set for a specific item. */
        Align get_item_alignment(const ILayoutElement& elem) const ;
        /** Set the alignment for a specific item. */
        void set_item_alignment(const ILayoutElement& w, Align align);

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

        void do_layout(const Rect& r) override;

        /** Return the number of rows. */
        int no_rows() const { return mno_rows; }
        /** Return the number of columns. */
        int no_cols() const { return mno_cols; }

        /** Will resize the table. This is impossible if widgets are still occupying cells that would be removed by this
         *  operation. */
        void resize(int ncols, int nrows);

    private:
        dtl::SimpleTableLayoutItem* get_item_col_row(int col, int row);
        std::vector<dtl::SimpleTableLayoutItem>::iterator get_item_iterator_col_row(int col, int row);

        struct RowColInfo {
            int stretch;
            int dim;
            int min;
            int pos;
        };

        int mno_rows, mno_cols;
        int mcol_spacing, mrow_spacing;
        std::vector <RowColInfo> mcol_info, mrow_info;
        SizeConstraint mlast_wc, mlast_hc;
};

}

#endif // LGUI_SIMPLETABLELAYOUT_H
