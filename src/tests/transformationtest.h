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
