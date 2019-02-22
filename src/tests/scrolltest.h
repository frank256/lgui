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

#ifndef LGUI_SCROLLTEST_H
#define LGUI_SCROLLTEST_H

#include "test_common.h"
#include "lgui/widgets/scrollarea.h"
#include "lgui/widgets/textfield.h"
#include "lgui/widgets/spinner.h"
#include "lgui/widgets/checkbox.h"
#include "lgui/widgets/radiobutton.h"
#include "lgui/layout/alignlayout.h"
#include "lgui/layout/flowlayout.h"
#include "lgui/layout/relativelayout.h"
#include "lgui/buttongroup.h"
#include "lgui/widgets/labels/textlabel.h"

class MyScrollArea : public lgui::ScrollArea {
    public:

        virtual void draw(const lgui::DrawEvent& de) const override;

};

class ScrollTestContent : public lgui::Widget {
    public:
        ScrollTestContent(int w, int h);

        virtual void draw(const lgui::DrawEvent& de) const;
};

class SizeControl : public lgui::BasicContainer, public lgui::IWidgetListener {
    public:
        SizeControl(lgui::Widget& widgetRef, const std::string& label);

        virtual void size_changed_wl(Widget& w) override;

    protected:
        lgui::Spinner mwedit, mhedit;
    private:
        lgui::TextLabel mlabel;
        lgui::FlowLayout mlayout;
        lgui::Widget& mctrl;
};

class ScrollTest : public TestContainer {
    public:
        ScrollTest();

        void reset();

    private:
        static const constexpr int INITIAL_DIM = 100;

        MyScrollArea mscroll;
        ScrollTestContent mtest_content;
        SizeControl mscroll_size_control, mcontent_size_control;
        lgui::TextLabel mscroll_size_behavior_lbl, mcontent_size_behavior_lbl;
        lgui::CheckBox mactive;
        lgui::RadioButton mexpand, mshrink_with_content, mcontent_no_limits, mcontent_force_width,
                          mcontent_force_height, mcontent_max_width, mcontent_max_height;
        lgui::ButtonGroup msize_behavior, mcontent_size_behavior;
        lgui::VBoxLayout msize_behavior_options_layout, mcontent_size_behavior_options1_layout,
                         mcontent_size_behavior_options2_layout;
        lgui::AlignLayout mrelax_layout;
        lgui::RelativeLayout mlayout;
};
#endif // LGUI_SCROLLTEST_H
