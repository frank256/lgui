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

#include "tabtest.h"

TabTest::TabTest()
    : minactive_tab("Inactive")
{
    mradio_test.set_clipped(true);
    mradio_test2.set_clipped(true);
    mradio_test3.set_clipped(true);
    mradio_test4.set_clipped(true);

    mtab_text1.set_text("Blah 1");
    mtab_text2.set_text("Blah 2");
    mtab_text3.set_text("Blah 3");
    mtab_text4.set_text("Blah 4");
    mtab_text5.set_text("Blah 5");
    mtab_text6.set_text("Blah 6");
    mtab_text7.set_text("Blah 7");
    mtab_text8.set_text("Blah 8");

    minactive_tab.set_active(false);

    mtabs1.add_tab("Radio test", mradio_test);
    mtabs1.add_tab("Foo", mtab_text1);
    mtabs1.set_always_show_scroll_buttons(true);
    mlbl1.set_text("Always show scroll buttons");

    mtabs2.add_tab("Radio test", mradio_test2);
    mtabs2.add_tab("Foo", mtab_text2);
    mtabs2.add_tab("Foo 3", mtab_text3);
    mlbl2.set_text("Show buttons when needed");

    mtabs3.add_tab("Radio test", mradio_test3);
    mtabs3.add_tab("Foo", mtab_text4);
    mtabs3.add_tab("Foo 3", mtab_text5);
    mtabs3.add_tab(minactive_tab, mtab_text6);
    mlbl3.set_text("Another TabWidget");

    mtabs4.add_tab("Radio test", mradio_test4);
    mtabs4.add_tab("Foo", mtab_text7);
    mtabs4.add_tab("Foo 3", mtab_text8);
    mtabs4.set_active(false);
    mtabs4.select_tab(2);
    mlbl4.set_text("Inactive");


    mlayout.add_item_ltrb(mtabs1, 0.025, 0.1, 0.3, 0.5);
    mlayout.add_item_ltrb(mtabs2, 0.375, 0.1, 0.65, 0.5);
    mlayout.add_item_ltrb(mtabs3, 0.725, 0.1, 1.0, 0.5);

    mlayout.add_item(mlbl1, {{lgui::RelativeLayout::Constraint::AlignLeft, mtabs1},
                             {lgui::RelativeLayout::Constraint::Above, mtabs1}});

    mlayout.add_item(mlbl2, {{lgui::RelativeLayout::Constraint::AlignLeft, mtabs2},
                             {lgui::RelativeLayout::Constraint::Above, mtabs2}});

    mlayout.add_item(mlbl3, {{lgui::RelativeLayout::Constraint::AlignLeft, mtabs3},
                             {lgui::RelativeLayout::Constraint::Above, mtabs3}});


    mlayout.add_item_ltrb(mtabs4, 0.325, 0.6, 0.6, 0.9);
    mlayout.add_item(mlbl4, {{lgui::RelativeLayout::Constraint::AlignLeft, mtabs4},
                             {lgui::RelativeLayout::Constraint::Above, mtabs4}});

    set_layout(&mlayout);
}
