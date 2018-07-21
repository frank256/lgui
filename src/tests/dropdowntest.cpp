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

#include "dropdowntest.h"

#include "platform/stringfmt.h"

DropDownTest::DropDownTest() {

    mlbl1.set_text("Using the same model");
    mlbl2.set_text("Using the same model & inactive");
    mlbl3.set_text("Resized");
    mlbl4.set_text("Internal model: Many items");

    mmodel.add_item("Rome");
    mmodel.add_item("Paris");
    mmodel.add_item("Madrid");
    mmodel.add_item("Athens");
    mmodel.add_item("Stockholm");

    mdrop_down1.set_model(&mmodel);
    mdrop_down1.set_selected(0);

    mdrop_down2.set_model(&mmodel);
    mdrop_down2.set_selected(1);
    mdrop_down2.set_active(false);

    mdrop_down3.set_model(&mmodel);
    mdrop_down3.set_selected(2);

    for(int i = 1; i <= 300; i++) {
        mdrop_down4.model().add_item(lgui::StringFmt("Item %1").arg(i));
    }

    mlayout.add_item_lt(mlbl1, 0.0, 0.0);
    mlayout.add_item(mdrop_down1, { {lgui::RelativeLayout::Constraint::AlignLeft, mlbl1},
                                    {lgui::RelativeLayout::Constraint::Below, mlbl1}});

    mlayout.add_item_lt(mlbl2, 0.5, 0.0);
    mlayout.add_item(mdrop_down2, { {lgui::RelativeLayout::Constraint::AlignLeft, mlbl2},
                                    {lgui::RelativeLayout::Constraint::Below, mlbl2}});

    mlayout.add_item_lt(mlbl3, 0.0, 0.4);
    mlayout.add_item(mdrop_down3, { {lgui::RelativeLayout::Constraint::AlignLeft, mlbl3},
                                    {lgui::RelativeLayout::Constraint::Below, mlbl3},
                                    {lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.4}});

    mlayout.add_item_lt(mlbl4, 0.5, 0.4);
    mlayout.add_item(mdrop_down4, { {lgui::RelativeLayout::Constraint::AlignLeft, mlbl4},
                                    {lgui::RelativeLayout::Constraint::Below, mlbl4}});

    set_layout(&mlayout);
}
