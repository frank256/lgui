#ifndef LGUITEST_LAYOUTANIMATIONTEST_H
#define LGUITEST_LAYOUTANIMATIONTEST_H

#include "lgui/layout/flowlayout.h"
#include "lgui/layout/vboxlayout.h"
#include "lgui/layout/layouttransition.h"
#include "test_common.h"

class LayoutAnimationTestButton : public BlockButton {
    public:
        LayoutAnimationTestButton(const std::string& str, int dim)
                : BlockButton(str, dim) {}
};

class LayoutAnimationTestRow : public lgui::PaddedContainer {
    public:
        LayoutAnimationTestRow();
    protected:
        void draw_background(const lgui::DrawEvent& de) const override;
    private:
        void add_button();

        lgui::FlowLayout mlayout;
        std::vector<std::unique_ptr<LayoutAnimationTestButton>> mtest_buttons;
        lgui::PushButton madd_button;
};


class LayoutAnimationTest : public TestContainer {
    public:
        LayoutAnimationTest();

    private:
        lgui::VBoxLayout mlayout;
        std::vector<std::unique_ptr<LayoutAnimationTestRow>> mrows;
        lgui::LayoutTransition mlayout_transition;
};


#endif //LGUITEST_LAYOUTANIMATIONTEST_H
