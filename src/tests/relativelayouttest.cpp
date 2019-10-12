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

#include "relativelayouttest.h"


RelativeLayoutTest::RelativeLayoutTest()
        : mbt1("Button1"), mbt2("Button2"), mbt3("Button3"), mbt4("Button4"), mbig_button("TEST") {
    using Constraint = lgui::RelativeLayout::Constraint;

    mbt1.set_name("Bt1");
    mbt2.set_name("Bt2");
    mbt3.set_name("Bt3");
    mbt4.set_name("Bt4");

    mlayout.add_item(mbt1, {{Constraint::AlignParentTop}});
    mlayout.add_item(mbt2, {{Constraint::AlignLeft, mbt1}, {Constraint::Below, mbt1}});
    mlayout.add_item(mbt3, {{Constraint::RightOf, mbt2}, {Constraint::Below, mbt2}});
    mlayout.add_item({mbt4, lgui::Align::VStretch}, {{Constraint::AlignRight, mbt3},
            {Constraint::Below, mbt3}, {Constraint::AlignParentBottom}});
    mlayout.add_item_ltrb({mbig_button, lgui::Align::HVStretch}, 0.7, 0.3, 0.9, 0.4);
    set_layout(&mlayout);
}
