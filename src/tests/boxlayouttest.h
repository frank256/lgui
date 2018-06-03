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

#ifndef LGUI_BOXLAYOUTTEST_H
#define LGUI_BOXLAYOUTTEST_H

#include "test_common.h"
#include "lgui/widgets/labels/wordwraptextlabel.h"
#include "lgui/layout/hboxlayout.h"

#include <memory>

class BoxLayoutTestWidget : public lgui::Widget {
    public:
        BoxLayoutTestWidget(const std::string& str)
            : mstr(str)
        {}

        void draw(const lgui::DrawEvent& de) const override;

        lgui::Size min_size_hint()  override { return lgui::Size(50, 50); }
        lgui::MeasureResults measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) override
        {
            return lgui::force_size_constraints(lgui::Size(/*std::max(hc.value(), 50)*/ 50, 50), wc, hc);
        }
    private:
        std::string mstr;
};

class BoxLayoutTest : public TestContainer {
    public:
        BoxLayoutTest();

        virtual void draw_background(const lgui::DrawEvent& de) const override;

    private:
        std::vector <std::unique_ptr<lgui::BasicContainer>> mcontainers;
        std::vector <std::unique_ptr<lgui::VBoxLayout>> mlayouts;
        std::vector <std::unique_ptr<BoxLayoutTestWidget>> mwidgets;
        std::vector <std::unique_ptr<lgui::WordWrapTextLabel>> mww_labels;
        lgui::HBoxLayout mlayout;
};


#endif // LGUI_BOXLAYOUTTEST_H
