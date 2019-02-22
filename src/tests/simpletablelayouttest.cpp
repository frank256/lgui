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

#include "simpletablelayouttest.h"


SimpleTableLayoutTest::SimpleTableLayoutTest()
    :   mlayout(W, H),
      mleft("Left"), mright("Right"), mhcenter("Center"), mmatch_parent_h("Match parent"),
      malign_horz_lbl("Horizontal alignment: ")
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if(rand() % 1000 < 800) {
                std::string text("Button: ");
                text += std::to_string(y) + ", " + std::to_string(x);
                auto *b = new lgui::PushButton(text);
                b->on_activated_src.connect(&SimpleTableLayoutTest::button_clicked, *this);
                mlayout.add_item(x, y, *b, lgui::Align::HCenter);
                mbuttons.push_back(b);
            }
        }
    }

    mlayout.set_column_spacing(5);
    mlayout.set_row_spacing(5);

    mtarget.set_layout(&mlayout);
    mtext_field.on_text_changed.connect(&SimpleTableLayoutTest::text_changed, *this);

    mleft.set_id(lgui::Align::Left);
    mright.set_id(lgui::Align::Right);
    mhcenter.set_id(lgui::Align::HCenter);
    mmatch_parent_h.set_id(lgui::Align::HMatchParent);

    mhorz_group.add_button(&mleft);
    mhorz_group.add_button(&mright);
    mhorz_group.add_button(&mhcenter);
    mhorz_group.add_button(&mmatch_parent_h);

    mhorz_group.on_button_activated_id.connect(&SimpleTableLayoutTest::change_alignment, *this);

    malign_horz_layout.set_default_alignment(lgui::Align::VCenter);
    malign_horz_layout.add_item(malign_horz_lbl);
    malign_horz_layout.add_stretch(1);
    malign_horz_layout.add_item(mleft);
    malign_horz_layout.add_stretch(1);
    malign_horz_layout.add_item(mhcenter);
    malign_horz_layout.add_stretch(1);
    malign_horz_layout.add_item(mright);
    malign_horz_layout.add_stretch(1);
    malign_horz_layout.add_item(mmatch_parent_h);

    mouter_layout.add_item(mtarget, 1);
    mouter_layout.add_item(malign_horz_layout);
    mouter_layout.add_item(mtext_field);
    set_layout(&mouter_layout);
}

SimpleTableLayoutTest::~SimpleTableLayoutTest() {
    for (auto b : mbuttons)
        delete b;
}

void SimpleTableLayoutTest::button_clicked(lgui::AbstractButton* src) {
    if (src) {
        mlast_clicked = static_cast<lgui::PushButton*>(src);
        mtext_field.set_text(mlast_clicked->text());

        lgui::Align a = mlayout.get_item_alignment(*src);
        mhorz_group.clear_checked_button();

        mhorz_group.check_button_id(a.horz());
        mtext_field.focus();
    }
}

void SimpleTableLayoutTest::text_changed(const std::string& text) {
    if (mlast_clicked)
        mlast_clicked->set_text(text);
}

void SimpleTableLayoutTest::change_alignment(int alchange) {
    if (mlast_clicked) {
        lgui::Align a = mlayout.get_item_alignment(*mlast_clicked);
        int mask = lgui::Align::HMask;
        if(alchange & lgui::Align::VMask)
            mask = lgui::Align::VMask;
        a = lgui::Align((a.align() & ~mask) | alchange);
        mlayout.set_item_alignment(*mlast_clicked, a);
    }
}
