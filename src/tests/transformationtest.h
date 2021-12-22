/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUITEST_TRANSFORMATIONTEST_H
#define LGUITEST_TRANSFORMATIONTEST_H

#include <memory>
#include <lgui/widgets/spinner.h>

#include "lgui/animation/animationfacilities.h"
#include "lgui/layout/alignlayout.h"
#include "lgui/layout/simpletablelayout.h"
#include "lgui/widgets/container.h"
#include "test_common.h"

#include "windowframetest.h"

class TransformationTestControls : public lgui::BasicContainer {
    public:
        TransformationTestControls();
        lgui::Signal<lgui::WidgetTransformationState> on_transformation_state_changed;
        lgui::Signal<> on_reset;

    private:
        void update();

        lgui::WidgetTransformationState mtransformation_state;

        lgui::Slider mtranslation_x;
        lgui::Slider mtranslation_y;
        lgui::Slider mrotation_z;
        lgui::Slider mrotation_y;
        lgui::Slider mrotation_x;
        lgui::Slider mscale_x;
        lgui::Slider mscale_y;
        lgui::TextLabel mtranslation_x_value_lbl;
        lgui::TextLabel mtranslation_y_value_lbl;
        lgui::TextLabel mrotation_z_value_lbl;
        lgui::TextLabel mrotation_y_value_lbl;
        lgui::TextLabel mrotation_x_value_lbl;
        lgui::TextLabel mscale_x_value_lbl;
        lgui::TextLabel mscale_y_value_lbl;
        lgui::TextLabel mtranslation_x_lbl;
        lgui::TextLabel mtranslation_y_lbl;
        lgui::TextLabel mrotation_z_lbl;
        lgui::TextLabel mrotation_y_lbl;
        lgui::TextLabel mrotation_x_lbl;
        lgui::TextLabel mscale_x_lbl;
        lgui::TextLabel mscale_y_lbl;
        lgui::PushButton mreset;
        lgui::SimpleTableLayout mlayout;
};

class TransformationTest : public lgui::BasicContainer {
    public:
        TransformationTest();
        lgui::MeasureResults measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) override;
    private:
        TransformationTestControls mctrls;
        lgui::VBoxLayout mlayout;
        WindowFrame mframe;
};


#endif //LGUITEST_TRANSFORMATIONTEST_H
