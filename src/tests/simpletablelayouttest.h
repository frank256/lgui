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

#ifndef LGUI_SIMPLETABLELAYOUTTEST_H
#define LGUI_SIMPLETABLELAYOUTTEST_H

#include "test_common.h"

#include "lgui/layout/simpletablelayout.h"
#include "lgui/layout/hboxlayout.h"

#include "lgui/widgets/radiobutton.h"
#include "lgui/widgets/textfield.h"
#include "lgui/buttongroup.h"


class SimpleTableLayoutTest : public TestContainer {
    static const int W = 4, H = 8;

    public:
        SimpleTableLayoutTest ();
        ~SimpleTableLayoutTest();

        void button_clicked(lgui::AbstractButton* src);
        void text_changed(const std::string& text);
        void change_alignment(int alchange);

    private:
        lgui::VBoxLayout mouter_layout;
        lgui::HBoxLayout malign_horz_layout;
        lgui::TextField mtext_field;
        lgui::SimpleTableLayout mlayout;
        std::vector <lgui::PushButton*> mbuttons;
        lgui::PushButton* mlast_clicked = nullptr;
        lgui::RadioButton mleft, mright, mhcenter, mmatch_parent_h;
        lgui::ButtonGroup mhorz_group;
        lgui::TextLabel malign_horz_lbl;
        lgui::BasicContainer mtarget;
};

#endif // LGUI_SIMPLETABLELAYOUTTEST_H
