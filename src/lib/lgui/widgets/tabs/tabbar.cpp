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

#include "tabbar.h"
#include "lgui/platform/graphics.h"
#include "lgui/mouseevent.h"

namespace lgui {

TabBar::TabBar()
        : mbt_left(Style::TabBarLeft),
          mbt_right(Style::TabBarRight),
          mselected_tab(nullptr),
          mselected_tab_idx(-1),
          mscrollx(0), mtabs_width(0),
          mscroll_shown(false),
          mno_left_line(false), malways_show_scroll_buttons(false) {
    set_padding(Padding(0));
    set_clipped(true);
    configure_new_child(mbt_left);
    configure_new_child(mbt_right);
    mbt_left.on_activated.connect([this]() { left_pressed(); });
    mbt_right.on_activated.connect([this]() { right_pressed(); });
    mbt_left.set_outside_children_area(true);
    mbt_right.set_outside_children_area(true);
}

void TabBar::draw(const DrawEvent& de) const {
    Rect r;
    if (mselected_tab) {
        r = mselected_tab->rect();
        r.translate(children_area().pos());
    }

    const Rect* sel_tab_rect = mselected_tab ? &r : nullptr;

    StyleArgs args(*this, de);
    style().draw_tab_bar_bg(de.gfx(), args, padding(), sel_tab_rect, mscroll_shown,
                            mscrollx > padding().left() && !mno_left_line);

    draw_children(de);

    style().draw_tab_bar_fg(de.gfx(), args, padding(), sel_tab_rect, mscroll_shown,
                            mscrollx > padding().left() && !mno_left_line);

    if (mbt_left.is_visible())
        draw_child(mbt_left, de);
    if (mbt_right.is_visible())
        draw_child(mbt_right, de);
}

void TabBar::add_tab(Tab& tab) {
    ASSERT(tab.tab_bar() == nullptr);
    tab.set_tab_bar(this);
    add_child(tab);
    mtabs.emplace_back(&tab);
    readjust_tabs();
    if (!mselected_tab)
        set_selected_tab(tab);
}

Tab& TabBar::add_tab(const std::string& caption) {
    Tab* tab = new Tab(caption);
    mmanaged_tabs.emplace_back(std::unique_ptr<Tab>(tab));
    add_tab(*tab);
    return *tab;
}

void TabBar::set_size(Size s) {
    int h = appropriate_height();
    PaddedContainer::set_size(Size(s.w(), h));
}

int TabBar::appropriate_height() const {
    int h = 0;
    for (auto tb : (*this)) {
        h = std::max(tb->height(), h);
    }
    // and compare with helper button
    h = std::max(h, style().get_helper_button_min_size(Style::TabBarLeft).h());
    h = std::max(h, style().get_helper_button_min_size(Style::TabBarRight).h());
    h += padding().vert();
    return h;
}

void TabBar::set_selected_tab(const Tab& tab) {
    if (mselected_tab != &tab) {
        int idx = find_index_for_tab(&tab);
        ASSERT(idx >= 0);
        set_selected_tab(idx);
    }
    else {
        // will be done by overload for others
        if (mselected_tab)
            ensure_tab_visible(*mselected_tab);
    }
}

void TabBar::set_selected_tab(int idx) {
    if (idx >= 0 && idx < signed(mtabs.size())) {
        Tab& new_tab = *mtabs[idx];
        if (mselected_tab)
            mselected_tab->set_selected(false);
        new_tab.set_selected(true);
        mselected_tab = &new_tab;
        mselected_tab_idx = idx;
        ensure_tab_visible(*mselected_tab);
        on_tab_selected.emit(mselected_tab);
    }
    else {
        // FIXME: emit what?
        mselected_tab_idx = -1;
        mselected_tab = nullptr;
    }
}

void TabBar::select_next_tab(bool move_focus) {
    int idx = mselected_tab_idx;
    if (idx >= 0) {
        idx++;
        if (idx >= signed(mtabs.size()))
            idx = 0;
        set_selected_tab(*mtabs[idx]);
        if (move_focus)
            mtabs[idx]->focus();
    }
}

void TabBar::select_previous_tab(bool move_focus) {
    int idx = mselected_tab_idx;
    if (idx >= 0) {
        idx--;
        if (idx < 0)
            idx = mtabs.size() - 1;
        set_selected_tab(*mtabs[idx]);
        if (move_focus)
            mtabs[idx]->focus();
    }
}

void TabBar::visit_down(const std::function<void(Widget&)>& f) {
    PaddedContainer::visit_down(f);
    mbt_left.visit_down(f);
    mbt_right.visit_down(f);
}

Widget* TabBar::get_child_at(PointF p) {
    // private children first
    if (mbt_left.is_visible() && mbt_left.is_inside(p)) {
        if (mbt_left.is_active()) {
            return &mbt_left;
        }
        return nullptr;
    }
    else if (mbt_right.is_visible() && mbt_right.is_inside(p)) {
        if (mbt_right.is_active())
            return &mbt_right;
        return nullptr;
    }
    return PaddedContainer::get_child_at(p);
}

void TabBar::child_about_to_die(Widget& child) {
    if (&child == &mbt_left || &child == &mbt_right)
        return;
    PaddedContainer::child_about_to_die(child);
}

void TabBar::mouse_wheel_down(MouseEvent& event) {
    select_next_tab();
    event.consume();
}

void TabBar::mouse_wheel_up(MouseEvent& event) {
    select_previous_tab();
    event.consume();
}

void TabBar::style_changed() {
    PaddedContainer::style_changed();
    mbt_left.set_style(&style());
    mbt_right.set_style(&style());
    layout_scroll_buttons();
    readjust_tabs();
}

Rect TabBar::children_area() const {
    Rect ca = PaddedContainer::children_area();
    ca.set_width(mtabs_width);
    ca.translate(-mscrollx, 0);
    return ca;
}

MeasureResults TabBar::measure(SizeConstraint wc, SizeConstraint hc) {
    TooSmallAccumulator too_small;
    int w = 0, h = 0;
    for (const auto& tb : (*this)) {
        MeasureResults tab_m = tb->measure(wc.adapted_for_child(), hc.adapted_for_child());
        w += tab_m.w();
        h = std::max(h, tab_m.h());
        too_small.consider(tab_m);
    }
    Size s = padding().add(Size(w, h));
    if (wc.mode() != SizeConstraintMode::NoLimits && wc.value() < w) {
        // need buttons
        Size ls = style().get_helper_button_min_size(Style::TabBarLeft);
        Size rs = style().get_helper_button_min_size(Style::TabBarRight);
        Size button_max(ls.w() + rs.w(), std::max(ls.h(), rs.h()));
        w = std::max(w, button_max.w());
        h = std::max(h, button_max.h());
        s = padding().add(Size(w, h));
    }
    return force_size_constraints(padding().add(s), wc, hc, too_small);
}

Size TabBar::min_size_hint() {
    int w = 0, h = 0;
    for (const auto& tb : (*this)) {
        Size tab_msh = tb->min_size_hint();
        w = std::max(w, tab_msh.w());
        h = std::max(h, tab_msh.h());
    }
    if (no_children() > 1) {
        Size ls = style().get_helper_button_min_size(Style::TabBarLeft);
        Size rs = style().get_helper_button_min_size(Style::TabBarRight);
        Size button_max(ls.w() + rs.w(), std::max(ls.h(), rs.h()));

        w = std::max(button_max.w(), w);
        h = std::max(button_max.h(), h);
    }

    return padding().add(Size(w, h));
}

void TabBar::resized(const Size& old_size) {
    readjust_tabs();
    layout_scroll_buttons();
    PaddedContainer::resized(old_size);
}

void TabBar::readjust_tabs() {
    int x = 0;
    for (auto tb : (*this)) {
        Size ts = tb->measure(SizeConstraint(0, SizeConstraintMode::NoLimits),
                              SizeConstraint(0, SizeConstraintMode::NoLimits));
        tb->set_size(ts);
        tb->set_pos(x, 0);
        x += tb->width();
    }
    mtabs_width = x;
    update_scrolling();
}

int TabBar::find_index_for_tab(const Tab* tab) const {
    for (int i = 0; i < signed(mtabs.size()); i++) {
        if (tab == mtabs[i])
            return i;
    }
    return -1;
}

void TabBar::update_scrolling() {
    bool buttons_needed = mtabs_width > width() || malways_show_scroll_buttons;

    set_show_scroll_buttons(buttons_needed);
    update_scroll_buttons_disabled();
}

void TabBar::update_scroll_buttons_disabled() {
    mbt_left.set_disabled(mscrollx <= 0);
    if (!mtabs.empty())
        mbt_right.set_disabled(mtabs.back()->rect().x2() - mscrollx <= visible_width());
    else
        mbt_right.set_disabled(true);

}

void TabBar::layout_scroll_buttons() {
    Size ls = style().get_helper_button_min_size(Style::TabBarLeft);
    Size rs = style().get_helper_button_min_size(Style::TabBarRight);
    ls.set_h(height() - padding().vert() - 1); // FIXME: make -1 a style setting
    rs.set_h(height() - padding().vert() - 1);
    mbt_left.set_size(ls);
    mbt_right.set_size(rs);
    mbt_right.set_pos(width() - mbt_right.width(), padding().top());
    mbt_left.set_pos(width() - mbt_right.width() - mbt_left.width(),
                     padding().top());
}

void TabBar::left_pressed() {
    // Make previous one that is invisible visible.
    for (int i = mtabs.size() - 1; i >= 0; i--) {
        if (mtabs[i]->pos_x() < mscrollx) {
            ensure_tab_visible(*mtabs[i]);
            break;
        }
    }
}

void TabBar::right_pressed() {
    // Make next one that is invisible visible.
    for (auto& tab : mtabs) {
        if (tab->pos_x() + tab->width() > mscrollx + visible_width()) {
            ensure_tab_visible(*tab);
            break;
        }
    }
}

void TabBar::ensure_tab_visible(const Tab& tab) {
    mno_left_line = false;
    if (width() <= 0 || height() <= 0)
        return;
    if (tab.pos_x() < mscrollx) {
        mscrollx = tab.pos_x();
        mno_left_line = true; // (partly) fix glitch
    }
    else if (tab.pos_x() + tab.width() > mscrollx + visible_width()) {
        mscrollx = tab.width() + tab.pos_x() - visible_width();
    }
    update_scroll_buttons_disabled();
}

int TabBar::visible_width() const {
    if (mscroll_shown)
        return width() - mbt_left.width() - mbt_right.width() - padding().horz();
    else
        return width() - padding().horz();
}

void TabBar::set_show_scroll_buttons(bool enable) {
    mscroll_shown = enable;
    mbt_left.set_visibility(enable ? Visibility::Visible : Visibility::Invisible);
    mbt_right.set_visibility(enable ? Visibility::Visible : Visibility::Invisible);
}

}
