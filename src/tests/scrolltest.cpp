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

#include "scrolltest.h"

#include "platform/graphics.h"
#include "lgui/drawevent.h"

void MyScrollArea::draw(const lgui::DrawEvent& de) const {
    ScrollArea::draw(de);
    de.gfx().rect(size_rect(), lgui::rgba_premult(1.0, 0.0, 1.0, 0.5f*de.opacity()), 1);
}

ScrollTestContent::ScrollTestContent(int w, int h)
{
    set_size(lgui::Size(w, h));
}

void ScrollTestContent::draw(const lgui::DrawEvent& de) const {
    de.gfx().filled_rect(size_rect(), lgui::rgba_premult(0.8, 0.8, 0.8, de.opacity()));
    de.gfx().rect(size_rect(), lgui::rgba_premult(0, 0, 1.0, de.opacity()), 1);
}

SizeControl::SizeControl(lgui::Widget& widget_ref, const std::string& label)
: mctrl(widget_ref)
{
    widget_ref.add_widget_listener(this);
    size_changed_wl(widget_ref);
    mlayout.set_max_on_row(1);
    mwedit.set_max_length(5);
    mhedit.set_max_length(5);
    mwedit.set_size(50, 0);
    mhedit.set_size(50, 0);
    mlabel.set_text(label);

    mlayout.add_item(mlabel);
    mlayout.add_item(mwedit);
    mlayout.add_item(mhedit);

    mwedit.on_value_changed.connect([this](int value) { mctrl.set_size(value, mctrl.height()); });
    mhedit.on_value_changed.connect([this](int value) { mctrl.set_size(mctrl.width(), value); });

    mwedit.set_min_value(0);
    mhedit.set_min_value(0);

    set_layout(&mlayout);
}

void SizeControl::size_changed_wl(lgui::Widget& w) {
    mwedit.set_value(w.width());
    mhedit.set_value(w.height());
}


ScrollTest::ScrollTest()
:   mtest_content(INITIAL_DIM , INITIAL_DIM ),
    mscroll_size_control(mscroll, "ScrollArea wxh"),
    mcontent_size_control(mtest_content, "TestContent wxh"),
    mactive("Active"),
    mexpand("Expand"),
    mshrink_with_content("Shrink with content"),
    mcontent_no_limits("No limits for content"),
    mcontent_force_width("Force width on content"),
    mcontent_force_height("Force height on content"),
    mcontent_max_width("Set max width on content"),
    mcontent_max_height("Set max height on content")
{
    mexpand.set_id(lgui::ScrollArea::Expand);
    mshrink_with_content.set_id(lgui::ScrollArea::ShrinkWithContent);

    msize_behavior.on_button_activated_id.connect([this](int id) {
        mscroll.set_size_behavior(lgui::ScrollArea::SelfSizeBehavior(id));
    });
    msize_behavior.add_button(&mexpand);
    msize_behavior.add_button(&mshrink_with_content);


    mcontent_no_limits.set_id(lgui::ScrollArea::ContentNoLimits);
    mcontent_force_width.set_id(lgui::ScrollArea::ContentForceWidth);
    mcontent_force_height.set_id(lgui::ScrollArea::ContentForceHeight);
    mcontent_max_width.set_id(lgui::ScrollArea::ContentMaxWidth);
    mcontent_max_height.set_id(lgui::ScrollArea::ContentMaxHeight);


    mcontent_size_behavior.on_button_activated_id.connect([this](int id) {
        mscroll.set_content_size_behavior(lgui::ScrollArea::ContentSizeBehavior(id));
    });

    mcontent_size_behavior.add_button(&mcontent_no_limits);
    mcontent_size_behavior.add_button(&mcontent_force_width);
    mcontent_size_behavior.add_button(&mcontent_force_height);
    mcontent_size_behavior.add_button(&mcontent_max_width);
    mcontent_size_behavior.add_button(&mcontent_max_height);


    mscroll_size_behavior_lbl.set_text("ScrollArea size behavior");
    mcontent_size_behavior_lbl.set_text("Content size behavior");

    mactive.set_checked(true);

    mscroll.set_name("MyScrollArea");
    mscroll.set_size(INITIAL_DIM , INITIAL_DIM );
    mscroll.set_content(&mtest_content);

    msize_behavior_options_layout.add_item(mscroll_size_behavior_lbl);
    msize_behavior_options_layout.add_item(mexpand);
    msize_behavior_options_layout.add_item(mshrink_with_content);

    mcontent_size_behavior_options1_layout.add_item(mcontent_size_behavior_lbl);
    mcontent_size_behavior_options1_layout.add_item(mcontent_no_limits);
    mcontent_size_behavior_options1_layout.add_item(mcontent_force_width);
    mcontent_size_behavior_options1_layout.add_item(mcontent_force_height);

    mcontent_size_behavior_options2_layout.add_item(mcontent_max_width);
    mcontent_size_behavior_options2_layout.add_item(mcontent_max_height);

    mlayout.add_item_lt(mcontent_size_control, 0, 0);
    mlayout.add_item(mscroll_size_control, { { lgui::RelativeLayout::Constraint::AlignParentRight },
                                             { lgui::RelativeLayout::Constraint::AlignParentTop } });
    mlayout.add_item_rb(mcontent_size_behavior_options2_layout, 1.0, 1.0);
    mlayout.add_item(mcontent_size_behavior_options1_layout, {
                         { lgui::RelativeLayout::Constraint::LeftOf, mcontent_size_behavior_options2_layout },
                         { lgui::RelativeLayout::Constraint::AlignParentBottom } });
    mlayout.add_item({ msize_behavior_options_layout, {0, 0, 30, 0} },
        { { lgui::RelativeLayout::Constraint::LeftOf, mcontent_size_behavior_options1_layout },
          { lgui::RelativeLayout::Constraint::AlignParentBottom } });
    mlayout.add_item({ mactive},
        { { lgui::RelativeLayout::Constraint::Above, msize_behavior_options_layout },
          { lgui::RelativeLayout::Constraint::AlignLeft, msize_behavior_options_layout } });

    // Wrap it in an extra layout context (HVTakeAll) so we do not get an ugly height-for-width problem.
    mlayout.add_item({ mscroll, { 0, 4 }, lgui::Align::HVTakeAll |  lgui::Align::Top | lgui::Align::Left},
                     { { lgui::RelativeLayout::Constraint::Below, mcontent_size_control },
                       { lgui::RelativeLayout::Constraint::Above, mactive } } );

    mactive.on_checked_changed.connect([this](bool e){mscroll.set_active(e);});

    set_name("ScrollTest");
    mscroll_size_control.set_name("Scroll size control");
    mscroll_size_control.set_active(false);
    mcontent_size_control.set_name("Content size control");
    set_layout(&mlayout);
}

void ScrollTest::reset() {
    mexpand.set_checked(true);
    mcontent_no_limits.set_checked(true);
    mactive.set_checked(true);
    mscroll.set_size(INITIAL_DIM , INITIAL_DIM );
    mtest_content.set_size(INITIAL_DIM , INITIAL_DIM );
}
