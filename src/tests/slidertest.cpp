/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#include "slidertest.h"

#include "platform/graphics.h"
#include "platform/stringfmt.h"
#include "lgui/drawevent.h"

void ColorWidget::draw(const lgui::DrawEvent& de) const
{
    de.gfx().filled_rect(size_rect(), mcolor);
}


SliderTest::SliderTest()
:    mcolorw(lgui::rgb_i(0,0,0))
{
    for(int i = 0; i < 6; i++)
        mlbls.emplace_back(std::make_unique<lgui::TextLabel>());

    mlbls[0]->set_text("Red:");
    mlbls[1]->set_text("Green:");
    mlbls[2]->set_text("Blue:");
    mlbls[3]->set_text("Inactive:");
    mlbls[4]->set_text("Vertical");

    for(int i = 0; i < 4; i++) {
        msliders.emplace_back(std::make_unique<lgui::Slider>());
        msliders.back()->set_max_value(255);
    }

    msliders.emplace_back(std::make_unique<lgui::Slider>(lgui::Vertical));
    msliders.back()->set_min_value(0);
    msliders.back()->set_max_value(1000);

    msliders[3]->set_active(false);
    msliders[3]->set_value(70);



    msliders[0]->on_value_changed.connect([this](int v) {
        lgui::Color col = mcolorw.color();
        col = lgui::rgb(float(v)/255.0f, col.g, col.b);
        mcolorw.set_color(col);
    });
    msliders[1]->on_value_changed.connect([this](int v) {
        lgui::Color col = mcolorw.color();
        col = lgui::rgb(col.r, float(v)/255.0f, col.b);
        mcolorw.set_color(col);
    });
    msliders[2]->on_value_changed.connect([this](int v) {
        lgui::Color col = mcolorw.color();
        col = lgui::rgb(col.r, col.g, float(v)/255.0f);
        mcolorw.set_color(col);
    });
    msliders[4]->on_value_changed.connect([this](int v) {
        mlbls[5]->set_text(lgui::StringFmt("Value: %1").arg(v));
    });
    msliders[4]->set_value(42);

    // We should really introduce a baseline alignment option...

    const int voffset = mlbls[0]->min_size_hint().h() - msliders[0]->min_size_hint().h();
    int lbl_offset = 0;
    int slider_offset = 0;

    if (voffset > 0)
        slider_offset = voffset/2;
    else if (voffset < 0)
        lbl_offset = -voffset/2;

    for (int i = 0; i < 4; ++i) {
        float top = 0.1+float(i)*0.075;
        if (i == 3) top += 0.1;
        mlayout.add_item_lt({*msliders[i], {0, slider_offset, 0, 0}}, 0.15, top);
        mlayout.add_constraint(*msliders[i], { lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.5 });
        mlayout.add_item({*mlbls[i], {0, lbl_offset, 10, 0}},
                         { {lgui::RelativeLayout::Constraint::AlignTop, *msliders[i]},
                           {lgui::RelativeLayout::Constraint::LeftOf, *msliders[i]}});
    }

    mlayout.add_item({ mcolorw, {0, 0, 0, 0} },
                     { {lgui::RelativeLayout::Constraint::AlignTop, *msliders[0]},
                       {lgui::RelativeLayout::Constraint::AlignBottom, *msliders[2]},
                       {lgui::RelativeLayout::Constraint::AlignLeftParentPerc, 0.6},
                       {lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.75}
                     });

    mlayout.add_item({ *msliders[4], {0, 0, 20, 20} },
                     { {lgui::RelativeLayout::Constraint::AlignTopParentPerc, 0.1 },
                       {lgui::RelativeLayout::Constraint::AlignParentRight },
                       {lgui::RelativeLayout::Constraint::AlignBottomParentPerc, 0.6}
                     });
    mlayout.add_item({ *mlbls[5] },
                    { {lgui::RelativeLayout::Constraint::AlignTopParentPerc, 0.6 },
                      {lgui::RelativeLayout::Constraint::AlignParentRight }
                    });

    mlayout.add_item({ *mlbls[4] },
                    { {lgui::RelativeLayout::Constraint::Above, *msliders[4] },
                      {lgui::RelativeLayout::Constraint::AlignParentRight }
                    });

    set_layout(&mlayout);
}

