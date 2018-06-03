/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

#include "listboxtest.h"

#include "platform/stringfmt.h"


ListBoxTest::ListBoxTest()
{
    minsert.set_text("Insert");
    mappend.set_text("Append");
    mremove.set_text("Remove selected");

    mlbl1.set_text("List box");
    mlbl2.set_text("Inactive");
    mlbl3.set_text("Another list box");

    minsert.on_activated.connect([this]() {
        // FIXME: trim whitespace
        if(!minput.text().empty())
            mmodel.insert_item(minput.text(), mlistbox1.selected_idx());
    });
    mappend.on_activated.connect([this]() {
        // FIXME: trim whitespace
        if(!minput.text().empty())
            mmodel.add_item(minput.text());
    });
    mremove.on_activated.connect([this]() {
        mmodel.remove_item(mlistbox1.selected_idx());
    });

    mlistbox2.set_active(false);

    for(int i = 1; i <= 10; i++) {
        mmodel.add_item(lgui::StringFmt("Item %1").arg(i));
    }
    mlistbox1.set_model(&mmodel);
    mlistbox2.set_model(&mmodel);
    mlistbox3.set_model(&mmodel);

    mlayout.add_item_ltrb(mlistbox1, 0.025, 0.1, 0.3, 0.4);
    mlayout.add_item_ltrb(mlistbox2, 0.375, 0.1, 0.65, 0.4);
    mlayout.add_item_ltrb(mlistbox3, 0.725, 0.1, 1.0, 0.4);

    mlayout.add_item(mlbl1, {{lgui::RelativeLayout::Constraint::AlignLeft, mlistbox1},
                             {lgui::RelativeLayout::Constraint::Above, mlistbox1}});

    mlayout.add_item(mlbl2, {{lgui::RelativeLayout::Constraint::AlignLeft, mlistbox2},
                             {lgui::RelativeLayout::Constraint::Above, mlistbox2}});

    mlayout.add_item(mlbl3, {{lgui::RelativeLayout::Constraint::AlignLeft, mlistbox3},
                             {lgui::RelativeLayout::Constraint::Above, mlistbox3}});

    mlayout.add_item({ mremove, { 0, 10} }, {{lgui::RelativeLayout::Constraint::AlignLeft, mlistbox1},
                               {lgui::RelativeLayout::Constraint::Below, mlistbox1}});

    mlayout.add_item(minput, {{lgui::RelativeLayout::Constraint::AlignLeft, mlistbox1},
                              {lgui::RelativeLayout::Constraint::AlignTopParentPerc, 0.6},
                              {lgui::RelativeLayout::Constraint::AlignRightParentPerc, 0.4}});

    mlayout.add_item({ minsert, {10, 0}}, {{lgui::RelativeLayout::Constraint::AlignTop, minput},
                               {lgui::RelativeLayout::Constraint::RightOf, minput}});

    mlayout.add_item({ mappend, {10, 0}}, {{lgui::RelativeLayout::Constraint::AlignTop, minput},
                               {lgui::RelativeLayout::Constraint::RightOf, minsert}});

    mlistbox1.on_item_activated.connect([this](int idx, const std::string& str){
        on_entry_activated.emit(idx, str);
    });
    mlistbox3.on_item_activated.connect([this](int idx, const std::string& str){
        on_entry_activated.emit(idx, str);
    });


    set_layout(&mlayout);
}
