#ifndef LGUITEST_TRANSFORMATIONTEST_H
#define LGUITEST_TRANSFORMATIONTEST_H

#include <memory>

#include "lgui/animation/animationfacilities.h"
#include "lgui/layout/alignlayout.h"
#include "lgui/layout/hboxlayout.h"
#include "lgui/layout/vboxlayout.h"
#include "lgui/widgets/container.h"
#include "test_common.h"

class TestWidget : public lgui::Container {
    public:
        explicit TestWidget(const std::string& text);

    private:
        lgui::VBoxLayout mlayout;
        lgui::TextLabel mtext_label;
        BlockButton mbutton;
};

class TransformationTest : public lgui::BasicContainer {
    public:
        TransformationTest();
        void draw_background(const lgui::DrawEvent &de) const override;

    private:
        void initialize_transformed_widget();
        void animate();
        lgui::BasicContainer mtransformed_widgets;

        lgui::VBoxLayout mlayout;
        lgui::PushButton manimate_button;

        std::vector<std::unique_ptr<TestWidget>> mtest_widgets;
        lgui::AnimationFacilities manimation_facilities;
};



#endif //LGUITEST_TRANSFORMATIONTEST_H
