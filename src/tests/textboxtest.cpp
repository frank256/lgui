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

#include "textboxtest.h"

TextBoxTest::TextBoxTest()
    : mactive2("Active"),
      mread_only2("Read only"),
      minactive_chkbx("Inactive checkbox")
{
    mtext_box1.set_text(mstatic_text);
    mtext_box2.set_text(mstatic_text);
    mtext_box3.set_text(mstatic_text);
    mtext_box4.set_text(mstatic_text);

    mlbl1.set_text("WrapMode::None");
    mtext_box1.set_wrap_mode(lgui::TextBox::WrapMode::None);

    mlbl2.set_text("WrapMode::Characters");
    mtext_box2.set_wrap_mode(lgui::TextBox::WrapMode::Characters);
    mtext_box2.set_read_only(true);

    mlbl3.set_text("WrapMode::WholeWords");
    mtext_box3.set_wrap_mode(lgui::TextBox::WrapMode::WholeWords);

    mlbl4.set_text("WrapMode::FittingWords");
    mtext_box4.set_wrap_mode(lgui::TextBox::WrapMode::FittingWords);

    mactive2.set_checked(true);
    mactive2.on_checked_changed.connect([this](bool c) {mtext_box2.set_active(c);});

    mread_only2.set_checked(true);
    mread_only2.on_checked_changed.connect([this](bool c) {mtext_box2.set_read_only(c);});
    minactive_chkbx.set_active(false);

    mwidth_slider.set_max_value(100);
    mwidth_slider.set_value(80);

    auto width_slider_changed = [this](int w) {
        float wf = (float(w) / 100.0) * 0.5;
        mlayout.add_constraint(mtext_box1, lgui::RelativeLayout::Constraint::AlignRightParentPerc, wf);
        mlayout.add_constraint(mtext_box2, lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.5 + wf);
        mlayout.add_constraint(mtext_box3, lgui::RelativeLayout::Constraint::AlignRightParentPerc, wf);
        mlayout.add_constraint(mtext_box4, lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.5 + wf);
        request_layout();
    };

    mwidth_slider.on_value_changed.connect(width_slider_changed);

    mheight_slider.set_max_value(100);
    mheight_slider.set_value(80);

    auto height_slider_changed = [this](int h) {
        float hf = (float(h) / 100.0) * 0.5;
        mlayout.add_constraint(mtext_box1, lgui::RelativeLayout::Constraint::AlignBottomParentPerc, hf);
        mlayout.add_constraint(mtext_box2, lgui::RelativeLayout::Constraint::AlignBottomParentPerc, hf);
        mlayout.add_constraint(mtext_box3, lgui::RelativeLayout::Constraint::AlignBottomParentPerc, 0.5 + hf);
        mlayout.add_constraint(mtext_box4, lgui::RelativeLayout::Constraint::AlignBottomParentPerc, 0.5 + hf);
        request_layout();
    };

    mheight_slider.on_value_changed.connect(height_slider_changed);


    mlayout.add_item_lt(mlbl1, 0.0, 0.0);
    mlayout.add_item_lt(mlbl2, 0.5, 0.0);
    mlayout.add_item_lt(mlbl3, 0.0, 0.5);
    mlayout.add_item_lt(mlbl4, 0.5, 0.5);

    mlayout.add_item(mwidth_slider, { { lgui::RelativeLayout::Constraint::AlignParentLeft },
                                      { lgui::RelativeLayout::Constraint::AlignParentBottom }  });
    mlayout.add_item(mheight_slider, { { lgui::RelativeLayout::Constraint::AlignParentRight },
                                       { lgui::RelativeLayout::Constraint::AlignParentTop },
                                       { lgui::RelativeLayout::Constraint::AlignParentBottom } });

    mlayout.add_constraint(mwidth_slider, { lgui::RelativeLayout::Constraint::LeftOf, mheight_slider });


    mlayout.add_item({ mtext_box1, lgui::Margin(0, 0, 20, 20) },
                     { {lgui::RelativeLayout::Constraint::Below, mlbl1},
                       {lgui::RelativeLayout::Constraint::AlignLeft, mlbl1}});

    mlayout.add_item({mtext_box2, lgui::Margin(0, 0, 20, 20) },
                     { {lgui::RelativeLayout::Constraint::Below, mlbl2},
                       {lgui::RelativeLayout::Constraint::AlignLeft, mlbl2}});

    mlayout.add_item({mtext_box3, lgui::Margin(0, 0, 20, 20) },
                     {{lgui::RelativeLayout::Constraint::Below, mlbl3},
                      {lgui::RelativeLayout::Constraint::AlignLeft, mlbl3}});

    mlayout.add_item({mtext_box4, lgui::Margin(0, 0, 20, 20) },
                     {{lgui::RelativeLayout::Constraint::Below, mlbl4},
                     {lgui::RelativeLayout::Constraint::AlignLeft, mlbl4}});

    mlayout.add_item(minactive_chkbx, {{lgui::RelativeLayout::Constraint::Above, mlbl4} });

    mlayout.add_item(mactive2, {{lgui::RelativeLayout::Constraint::AlignLeft, mlbl2},
                                {lgui::RelativeLayout::Constraint::AlignTop, minactive_chkbx}});

    mlayout.add_constraint(minactive_chkbx, {lgui::RelativeLayout::Constraint::RightOf, mactive2});

    mlayout.add_item(mread_only2, {{lgui::RelativeLayout::Constraint::Above, mactive2},
                                   {lgui::RelativeLayout::Constraint::AlignLeft, mactive2}});

    // Initialize additional constraints.
    width_slider_changed(mwidth_slider.value());
    height_slider_changed(mheight_slider.value());

    set_layout(&mlayout);
}

void TextBoxTest::post_layout()
{
    // Please do not react to these changes...
    mlayout.set_update_on_child_resize(false);

    if (mtext_box2.right() > mheight_slider.pos_x()) {
        mtext_box2.set_size(mheight_slider.pos_x() - mtext_box2.pos_x(), mtext_box2.height());
    }
    if (mtext_box4.right() > mheight_slider.pos_x()) {
        mtext_box4.set_size(mheight_slider.pos_x() - mtext_box4.pos_x(), mtext_box4.height());
    }
    if (mtext_box4.bottom() > mwidth_slider.pos_y()) {
        mtext_box4.set_size(mtext_box4.width(), mwidth_slider.pos_y() - mtext_box4.pos_y());
    }
    if (mtext_box3.bottom() > mwidth_slider.pos_y()) {
        mtext_box3.set_size(mtext_box3.width(), mwidth_slider.pos_y() - mtext_box3.pos_y());
    }

    if (mtext_box2.bottom() > mread_only2.pos_y()) {
        mtext_box2.set_size(mtext_box2.width(), mread_only2.pos_y() - mtext_box2.pos_y());
    }

    mlayout.set_update_on_child_resize(true);
}




