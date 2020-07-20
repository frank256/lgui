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

#include "radiotest.h"

#include "lgui/platform/stringfmt.h"

RadioTest::RadioTest()
        : mradio1("Test1"),
          mradio2("Test2"),
          mradio3("Test3"),
          mradio4("Test4 (disabled)"),
          mradio5("Test5"),
          mlabel("Activated: 0") {
    mradio1.set_id(1);
    mradio2.set_id(2);
    mradio3.set_id(3);
    mradio4.set_id(4);
    mradio5.set_id(5);
    mradio4.set_disabled(true);
    mlayout.add_item(mradio1);
    mlayout.add_item(mradio2);
    mlayout.add_item(mradio3);
    mlayout.add_item(mradio4);
    mlayout.add_item(mradio5);
    mlayout.add_item(mlabel);
    mgroup.add_button(&mradio1);
    mgroup.add_button(&mradio2);
    mgroup.add_button(&mradio3);
    mgroup.add_button(&mradio4);
    mgroup.add_button(&mradio5);

    mlayout.set_max_on_row(1);

    set_layout(&mlayout);

    mgroup.on_button_activated_id.connect([this](int id) {
        mlabel.set_text(lgui::StringFmt("Activated: %1").arg(id));
    });
}
