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

#include "textfieldbuttontest.h"

#include "platform/stringfmt.h"

TextFieldButtonTest::TextFieldButtonTest()
    : mno_clicks(0)
{
    meditable_button_text.set_text("Test", false);
    meditable_button_text.set_max_length(40);

    meditable_button.set_text("Test");

    minactive_button.set_text("Test");
    minactive_button.set_active(false);

    mlbl1.set_text("Enter text for buttons: ");
    mlbl2.set_text("Inactive:");
    mlbl3.set_text("Inactive:");

    update_label_text();

    mfield2.set_text("Cannot change :(", false);
    mfield2.set_active(false);

    meditable_button_text.on_text_changed.connect([this](const std::string& text) {
        meditable_button.set_text(text);
        minactive_button.set_text(text);
        meditable_button.set_min_size();
        minactive_button.set_min_size();
    });
    meditable_button.on_activated.connect([this]() {
        mno_clicks++;
        update_label_text();
    });

    mlayout.add_item_lt(mlbl1, 0.0, 0.0);
    mlayout.add_item(meditable_button_text, { { lgui::RelativeLayout::Constraint::RightOf, mlbl1 },
                                              { lgui::RelativeLayout::Constraint::AlignParentTop },
                                              { lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.8 } });

    mlayout.add_item_lt(meditable_button, 0.0, 0.2);

    mlayout.add_item_lt(mno_clicks_lbl, 0.0, 0.4);

    mlayout.add_item_lt(mlbl2, 0.0, 0.6);

    mlayout.add_item(mfield2, { { lgui::RelativeLayout::Constraint::RightOf, mlbl2 },
                                { lgui::RelativeLayout::Constraint::AlignTop, mlbl2},
                                { lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.8 } });

    mlayout.add_item_lt(mlbl3, 0.0, 0.8);

    mlayout.add_item(minactive_button, { { lgui::RelativeLayout::Constraint::RightOf, mlbl3 },
                                         { lgui::RelativeLayout::Constraint::AlignTop, mlbl3 } });

    set_layout(&mlayout);
}

void TextFieldButtonTest::update_label_text() {
    mno_clicks_lbl.set_text(lgui::StringFmt("clicks: %1").arg(mno_clicks));
}
