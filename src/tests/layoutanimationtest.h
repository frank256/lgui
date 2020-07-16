#ifndef LGUITEST_LAYOUTANIMATIONTEST_H
#define LGUITEST_LAYOUTANIMATIONTEST_H

#include "lgui/widgets/labels/wordwraptextlabel.h"
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
        void update_ungone_button_text();

        lgui::FlowLayout mlayout;
        std::vector<std::unique_ptr<LayoutAnimationTestButton>> mtest_buttons;
        lgui::PushButton madd_button, mungone_button;
        lgui::VBoxLayout mbutton_layout;
        int mgone_counter = 0;
};


class LayoutAnimationTest : public TestContainer {
    public:
        LayoutAnimationTest();

    private:
        lgui::VBoxLayout mlayout;
        std::vector<std::unique_ptr<LayoutAnimationTestRow>> mrows;
        lgui::LayoutTransition mlayout_transition;
        lgui::WordWrapTextLabel mexplanation;
};


#endif //LGUITEST_LAYOUTANIMATIONTEST_H
