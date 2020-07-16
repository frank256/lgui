#include "layoutanimationtest.h"
#include "lgui/drawevent.h"
#include "lgui/platform/graphics.h"

LayoutAnimationTestRow::LayoutAnimationTestRow() {
    mlayout.add_item(madd_button);
    for (int i = 0; i < 6; ++i) {
        add_button();
    }
    madd_button.on_activated.connect([this]() {
        // Cannot handle more than one button being added simultaneously currently
        if (layout_transition() && !layout_transition()->is_transition_in_progress()) {
            add_button();
        }
    });
    set_padding(lgui::Padding(10));
    set_layout(&mlayout);
}

void LayoutAnimationTestRow::add_button() {
    mtest_buttons.emplace_back(std::make_unique<LayoutAnimationTestButton>("", 64));
    LayoutAnimationTestButton& test_button = *mtest_buttons.back();
    test_button.set_start_dd(false);
    test_button.on_activated.connect([&test_button]() {
//        if (test_button.layout_transition()->is_transition_in_progress())
//            return;
        test_button.set_gone();
        test_button.set_active(false);
    });
    mlayout.add_item(test_button);
}

void LayoutAnimationTestRow::draw_background(const lgui::DrawEvent& de) const {
    de.gfx().rect(size_rect(), lgui::grey(1.0), 1);
}

LayoutAnimationTest::LayoutAnimationTest() {
    for (int i = 0; i < 3; ++i) {
        mrows.emplace_back(std::make_unique<LayoutAnimationTestRow>());
        mlayout.add_item({*mrows.back(), {0, 40, 0, 0}});
    }
    set_layout(&mlayout);
    mlayout_transition.set_root_widget(this);
}
