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

#ifndef LGUI_ROUNDEDRECTTEST_H
#define LGUI_ROUNDEDRECTTEST_H

#include "test_common.h"
#include "lgui/widgets/radiobutton.h"
#include "lgui/layout/relativelayout.h"
#include "lgui/buttongroup.h"

class RoundedRectTest : public TestContainer {
    public:
        RoundedRectTest();

        void update_info();

        void draw_background(const lgui::DrawEvent& de) const override;

        virtual void style_changed() override;

        virtual void post_layout() override;

    private:
        int mrx, mry, mw, mh, mthickness;
        lgui::HorizontalSlider mslider_w, mslider_rx, mslider_thickness;
        lgui::VerticalSlider mslider_h, mslider_ry;
        lgui::RadioButton mcorners_bt, mbracket_bt, mfilledc_bt, mgradient_bt,
                         mfilled_bracket_bt, mfilled_bracket_gradient_bt;
        lgui::ButtonGroup mbt_group;
        lgui::PaddedContainer mradios;
        lgui::TextLabel minfo_label;
        lgui::RelativeLayout mlayout;
        lgui::VBoxLayout mradio_layout;
        lgui::Color mdraw_color;
        int mdraw_mode;

};


#endif // LGUI_ROUNDEDRECTTEST_H
