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

#include "roundedrecttest.h"

#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "platform/stringfmt.h"
#include "lgui/style/style.h"

RoundedRectTest::RoundedRectTest()
:   mthickness(5),
    mcorners_bt("Corners"),
    mbracket_bt("Brackets"),
    mfilledc_bt("Filled cs."),
    mgradient_bt("Gradient"),
    mfilled_bracket_bt("Filled bracket"),
    mfilled_bracket_gradient_bt("Filled bracket gradient"),
    mdraw_mode(0)
{
    mrx = mry = 5;
    mw = mh = 100;

    mslider_w.on_value_changed.connect([this](int w) { mw = w; update_info(); });
    mslider_h.on_value_changed.connect([this](int h) { mh = h; update_info(); });
    mslider_rx.on_value_changed.connect([this](int rx) { mrx = rx; update_info(); });
    mslider_ry.on_value_changed.connect([this](int ry) { mry = ry; update_info(); });
    mslider_thickness.on_value_changed.connect([this](int t) { mthickness = t; update_info(); });

    mcorners_bt.on_activated.connect([this]() { mdraw_mode = 0; });
    mbracket_bt.on_activated.connect([this]() { mdraw_mode = 1; });
    mfilledc_bt.on_activated.connect([this]() { mdraw_mode = 2; });
    mgradient_bt.on_activated.connect([this]() { mdraw_mode = 3; });
    mfilled_bracket_bt.on_activated.connect([this]() { mdraw_mode = 4; });
    mfilled_bracket_gradient_bt.on_activated.connect([this]() { mdraw_mode = 5; });

    mbt_group.add_button(&mcorners_bt);
    mbt_group.add_button(&mbracket_bt);
    mbt_group.add_button(&mfilledc_bt);
    mbt_group.add_button(&mgradient_bt);
    mbt_group.add_button(&mfilled_bracket_bt);
    mbt_group.add_button(&mfilled_bracket_gradient_bt);

    mcorners_bt.set_checked(true);

    mslider_thickness.set_max_value(20);
    mslider_thickness.set_value(5);
    mslider_rx.set_max_value(100);
    mslider_ry.set_max_value(100);

    mradio_layout.add_item(mcorners_bt);
    mradio_layout.add_item(mbracket_bt);
    mradio_layout.add_item(mgradient_bt);
    mradio_layout.add_item(mfilledc_bt);
    mradio_layout.add_item(mfilled_bracket_bt);
    mradio_layout.add_item(mfilled_bracket_gradient_bt);
    mradios.set_padding(lgui::Padding(10));
    mradios.set_layout(&mradio_layout);

    mlayout.add_item(mradios, { { lgui::RelativeLayout::Constraint::AlignParentRight } });

    mlayout.add_item(mslider_thickness, { { lgui::RelativeLayout::Constraint::AlignParentBottom},
                                          { lgui::RelativeLayout::Constraint::AlignLeft, mradios },
                                          { lgui::RelativeLayout::Constraint::AlignRight, mradios } });

    mlayout.add_constraint(mradios, lgui::RelativeLayout::Constraint::Above, mslider_thickness);

    mlayout.add_item(mslider_w, { {lgui::RelativeLayout::Constraint::AlignParentLeft },
                                  {lgui::RelativeLayout::Constraint::AlignParentBottom } });

    mlayout.add_constraint(mslider_w, lgui::RelativeLayout::Constraint::LeftOf, mradios);


    mlayout.add_item(mslider_h, { {lgui::RelativeLayout::Constraint::AlignParentRight },
                                  {lgui::RelativeLayout::Constraint::AlignParentTop } });
    mlayout.add_constraint(mslider_h, lgui::RelativeLayout::Constraint::Above, mradios);

    mlayout.add_item(mslider_ry, { { lgui::RelativeLayout::Constraint::LeftOf, mslider_h},
                                   { lgui::RelativeLayout::Constraint::AlignTop, mslider_h },
                                   { lgui::RelativeLayout::Constraint::AlignBottom, mslider_h }});

    mlayout.add_item(mslider_rx, { { lgui::RelativeLayout::Constraint::Above, mslider_w },
                                   { lgui::RelativeLayout::Constraint::AlignLeft, mslider_w },
                                   { lgui::RelativeLayout::Constraint::AlignRight, mslider_w }});

    mlayout.add_item(minfo_label, { { lgui::RelativeLayout::Constraint::Above, mslider_rx },
                                    { lgui::RelativeLayout::Constraint::AlignLeft, mslider_rx },
                                    { lgui::RelativeLayout::Constraint::AlignRight, mslider_rx }});

    set_layout(&mlayout);

    update_info();
    style_changed();
}

void RoundedRectTest::update_info()
{
    minfo_label.set_text(lgui::StringFmt("w: %1, h: %2, rx: %3, ry: %4 thickness: %5")
    .arg(mw, 3).arg(mh, 3).arg(mrx, 3).arg(mry, 3).arg(mthickness, 3));
}

void RoundedRectTest::draw_background(const lgui::DrawEvent& de) const
{
    //opacity = 0.4;
    const int px = 10, py = 10;
    int x = px, y = py;
    for(int i = 0; i < 16; i++) {
        if(i % 4 == 0 && i > 0) {
            x = px;
            y += mh+py;
        }
        float offs = 0.5;

        switch(mdraw_mode) {
            default:
            case 0:
                de.gfx().rounded_rect_spec_corners(x+offs, y+offs, x+mw+offs, y+mh+offs, mrx, mry,
                                                   mdraw_color, mthickness, i);
                break;
            case 1:
                de.gfx().rounded_rect_bracket(x+offs, y+offs, x+offs+mw, y+offs+mh, mrx, mry,
                                              mdraw_color, mthickness, lgui::OpenEdge(i%4));
                break;
            case 2:
                de.gfx().filled_rounded_rect_spec_corners(x+offs, y+offs, x+mw+offs, y+mh+offs, mrx, mry,
                                                          mdraw_color, i);
                break;
            case 3:
                de.gfx().filled_rounded_rect_gradient(x+offs, y+offs, x+mw+offs, y+mh+offs, mrx, mry,
                                                      lgui::rgba_premult(1.0, 0.0, 0.0, de.opacity()),
                                                      lgui::rgba_premult(0.0, 1.0, 0.0, de.opacity()),
                                                      lgui::GradientDirection(i%4));
                break;
            case 4:
                de.gfx().filled_rounded_rect_bracket(x+offs, y+offs, x+mw+offs, y+mh+offs, mrx, mry,
                                                     mdraw_color, lgui::OpenEdge(i%4));
                break;
            case 5:
                de.gfx().filled_rounded_rect_bracket_gradient(x+offs, y+offs, x+mw+offs, y+mh+offs, mrx, mry,
                                                              lgui::rgba_premult(1.0, 0.0, 0.0, de.opacity()),
                                                              lgui::rgba_premult(0.0, 1.0, 00, de.opacity()),
                                                              lgui::OpenEdge(i%4), lgui::GradientDirection(i/4));
                break;
        }
        x+=mw+px;
    }
}

void RoundedRectTest::style_changed() {
    TestContainer::style_changed();
    mdraw_color = style().button_text_color(lgui::WidgetState(false, false, false), 1.0);
}

void RoundedRectTest::post_layout() {
    // -10 for the spacing.
    mslider_w.set_max_value(mslider_w.width() / 4 - 10);
    mslider_h.set_max_value(mslider_h.height() / 4 - 10);
    mslider_w.set_value(mslider_w.max_value());
    mslider_h.set_value(mslider_h.max_value());
}
