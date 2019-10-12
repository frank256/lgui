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

#include "simpletablelayout.h"

namespace lgui {

SimpleTableLayout::SimpleTableLayout(int ncols, int nrows)
    : mno_rows(nrows), mno_cols(ncols),
      mcol_spacing(0), mrow_spacing(0)
{
    ASSERT(nrows > 0);
    ASSERT(ncols > 0);
    mcol_info.resize(ncols);
    mrow_info.resize(nrows);
}

void SimpleTableLayout::add_item(int x, int y, const LayoutItemProxy& le)
{
    ASSERT(x >= 0 && x < mno_cols);
    ASSERT(y >= 0 && y < mno_rows);
    ASSERT(get_item_col_row(x, y) == nullptr);
    mitems.emplace_back(dtl::SimpleTableLayoutItem(x, y, le));
    added_elem(*le.elem());
}

void SimpleTableLayout::remove_item(ILayoutElement& elem)
{
    auto it = find_elem(elem);
    if(it != mitems.end()) {
        mitems.erase(it);
        removed_elem(elem);
        if(mtarget)
            mtarget->request_layout();
    }
}

bool SimpleTableLayout::remove_item(int x, int y)
{
    ASSERT(x >= 0 && x < mno_cols);
    ASSERT(y >= 0 && y < mno_rows);
    auto it = get_item_iterator_col_row(x, y);
    if (it != mitems.end()) {
        ILayoutElement* elem = it->layout_element();
        mitems.erase(it);
        removed_elem(*elem);
        if(mtarget)
            mtarget->request_layout();
        return true;
    }
    return false;
}

int SimpleTableLayout::column_stretch(int col) const
{
    ASSERT(col >= 0 && col < mno_cols);
    return mcol_info[col].stretch;
}

int SimpleTableLayout::row_stretch(int row) const
{
    ASSERT(row >= 0 && row < mno_rows);
    return mrow_info[row].stretch;
}

void SimpleTableLayout::set_column_stretch(int col, int stretch)
{
    ASSERT(col >= 0 && col < mno_cols);
    mcol_info[col].stretch = stretch;
    if(mtarget)
        mtarget->request_layout();
}

void SimpleTableLayout::set_row_stretch(int row, int stretch)
{
    ASSERT(row >= 0 && row < mno_rows);
    mrow_info[row].stretch = stretch;
    if(mtarget)
        mtarget->request_layout();
}

void SimpleTableLayout::set_column_spacing(int spacing)
{
    mcol_spacing = spacing;
    if(mtarget)
        mtarget->request_layout();
}

void SimpleTableLayout::set_row_spacing(int spacing)
{
    mrow_spacing = spacing;
    if(mtarget)
        mtarget->request_layout();
}

Align SimpleTableLayout::get_item_alignment(const ILayoutElement& elem) const
{
    auto it = find_elem(elem);
    if(it != mitems.end()) {
        return it->alignment();
    }
    return Align();
}

void SimpleTableLayout::set_item_alignment(const ILayoutElement& elem, Align align)
{
    auto it = find_elem(elem);
    if(it != mitems.end()) {
        if(it->alignment() != align) {
            it->set_alignment(align);
            if(mtarget)
                mtarget->request_layout();
        }
    }
}

MeasureResults lgui::SimpleTableLayout::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc)
{
    // FIXME: Doesn't work that way?
    mlast_wc = wc;
    mlast_hc = hc;

    int col_stretch_sum = 0,
        row_stretch_sum = 0;

    for(auto& rci : mcol_info) {
        rci.dim = 0;
        col_stretch_sum += rci.stretch;
    }
    for(auto& rci : mrow_info) {
        rci.dim = 0;
        row_stretch_sum += rci.stretch;
    }

    if(wc.mode() == SizeConstraintMode::NoLimits &&
       hc.mode() == SizeConstraintMode::NoLimits) {
        for(auto& li : mitems) {
            if(li.skip())
                continue;
            Size ms = li.measure(wc, hc);
            mcol_info[li.col()].dim = std::max(mcol_info[li.col()].dim, ms.w());
            mrow_info[li.row()].dim = std::max(mrow_info[li.row()].dim, ms.h());
        }
    }
    else {
        for(auto& li : mitems) {
            if(li.skip())
                continue;
            Size ms = li.min_size_hint();
            mcol_info.at(li.col()).dim = std::max(mcol_info[li.col()].dim, ms.w());
            mrow_info.at(li.row()).dim = std::max(mrow_info[li.row()].dim, ms.h());
        }
    }

    int min_width = 0,
        min_height = 0;

    for(auto& rci : mcol_info) {
        min_width += rci.dim;
    }
    for(auto& rci : mrow_info) {
        min_height += rci.dim;
    }

    int row_sp = std::max(mrow_spacing * (mno_rows - 1), 0);
    int col_sp = std::max(mrow_spacing * (mno_cols - 1), 0);

    int width = min_width + col_sp, height = min_height + row_sp;

    if(wc.mode() != SizeConstraintMode::NoLimits) {
        int space_avail_x = wc.value() - min_width - col_sp;
        if(space_avail_x > 0 && col_stretch_sum > 0) {
            for(auto& rci : mcol_info) {
                if(rci.stretch > 0) {
                    float prop = float(rci.stretch) / col_stretch_sum;
                    int add_w = prop * space_avail_x;
                    col_stretch_sum -= rci.stretch;
                    space_avail_x -= add_w;
                    rci.dim += add_w;
                    width += add_w;
                }
            }
        }
    }
    if(hc.mode() != SizeConstraintMode::NoLimits) {
        int space_avail_y = hc.value() - min_height - row_sp;
        if(space_avail_y > 0 && row_stretch_sum > 0) {
            for(auto& rci : mrow_info) {
                if(rci.stretch > 0) {
                    float prop = float(rci.stretch) / row_stretch_sum;
                    int add_h = prop * space_avail_y;
                    row_stretch_sum -= rci.stretch;
                    space_avail_y -= add_h;
                    rci.dim += add_h;
                    height += add_h;
                }
            }
        }
    }

    // TODO: do something for wc.mode == NoLimits xor hc.mode == NoLimits?

    return force_size_constraints(Size(width, height), wc, hc);
}

Size SimpleTableLayout::min_size_hint()
{
    // ok, this is const...
    std::vector<int> rows(mno_rows, 0), cols(mno_cols, 0);

    for(auto& li : mitems) {
        if(li.skip())
            continue;
        Size ms = li.min_size_hint();
        cols[li.col()] = std::max(cols[li.col()], ms.w());
        rows[li.row()] = std::max(rows[li.row()], ms.h());
    }
    int w = -mcol_spacing, h = -mrow_spacing;

    for(auto c : cols) {
        w += c + mcol_spacing;
    }

    for(auto r : rows) {
        h += r + mrow_spacing;
    }

    return Size(std::max(w, 0), std::max(h, 0));
}



void SimpleTableLayout::do_layout(const Rect& r)
{
    if(!mtarget)
        return;
    // remeasure

    Size ts = r.size();
    SizeConstraint wc = SizeConstraint(ts.w(), SizeConstraintMode::Exactly);
    SizeConstraint hc = SizeConstraint(ts.h(), SizeConstraintMode::Exactly);

    if(mtarget->needs_relayout() ||
       (wc != mlast_wc || hc != mlast_hc)) {
        measure(wc, hc);
    }

    int px = -mcol_spacing;
    for(auto& rci : mcol_info) {
        px += mcol_spacing;
        rci.pos = px;
        px += rci.dim;
    }
    int py = -mrow_spacing;
    for(auto& rci : mrow_info) {
        py += mrow_spacing;
        rci.pos = py;
        py += rci.dim;
    }

    for(auto& li : mitems) {
        if(li.skip())
            continue;

        Rect cell(mcol_info[li.col()].pos, mrow_info[li.row()].pos,
                  mcol_info[li.col()].dim, mrow_info[li.row()].dim);

        SizeConstraint cwc(cell.w(), li.alignment().horz() == Align::HStretch
                                                    ? SizeConstraintMode::Exactly
                                                    : SizeConstraintMode::Maximum);

        SizeConstraint chc(cell.h(), li.alignment().vert() == Align::VStretch
                                                    ? SizeConstraintMode::Exactly
                                                    : SizeConstraintMode::Maximum);

        Size ms = li.measure(cwc, chc);
        li.set_allotted_size(ms);

        li.set_allotted_pos(do_alignment(cell, ms, li.alignment()));
        li.layout(li.allotted_rect().translated(r.pos()));
    }
}

dtl::SimpleTableLayoutItem*SimpleTableLayout::get_item_col_row(int col, int row)
{
    auto it = get_item_iterator_col_row(col, row);
    if (it != mitems.end())
        return &(*it);
    else
        return nullptr;
}

std::vector<dtl::SimpleTableLayoutItem>::iterator SimpleTableLayout::get_item_iterator_col_row(int col, int row)
{
    for (auto it = mitems.begin(); it != mitems.end(); ++it) {
        if (it->col() == col && it->row() == row)
            return it;
    }
    return mitems.end();
}

void SimpleTableLayout::resize(int ncols, int nrows) {
    for (const auto& it : mitems) {
        ASSERT(it.row() < nrows);
        ASSERT(it.col() < ncols);
    }
    mno_rows = nrows;
    mno_cols = ncols;
    mrow_info.resize(nrows);
    mcol_info.resize(ncols);
    if (mtarget) {
        mtarget->request_layout();
    }
}

}

