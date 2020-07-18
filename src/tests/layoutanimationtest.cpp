#include "layoutanimationtest.h"
#include "lgui/drawevent.h"
#include "lgui/platform/graphics.h"

LayoutAnimationTestRow::LayoutAnimationTestRow() {
    madd_button.set_text("Add");
    mungone_button.set_text("Restore");
    mbutton_layout.add_item(madd_button);
    mbutton_layout.add_item(mungone_button);
    mlayout.add_item(mbutton_layout);
    for (int i = 0; i < 6; ++i) {
        add_button();
    }
    madd_button.on_activated.connect([this]() {
        // Cannot handle more than one button being added simultaneously currently
        if (layout_transition() && !layout_transition()->is_transition_in_progress()) {
            add_button();
        }
    });
    mungone_button.on_activated.connect([this]() {
        for (Widget* w: *this) {
            if (w->is_gone()) {
                w->set_visible();
                w->set_active(true);
            }
        }
    });
    set_padding(lgui::Padding(10));
    set_layout(&mlayout);
}

void LayoutAnimationTestRow::add_button() {
    mtest_buttons.emplace_back(std::make_unique<LayoutAnimationTestButton>("", 64));
    LayoutAnimationTestButton& test_button = *mtest_buttons.back();
    test_button.set_start_dd(false);
    test_button.on_activated.connect([&test_button, this]() {
        test_button.set_active(false);
        if (test_button.mouse_button() == 2) {
            test_button.set_gone();
            cleanup();
        }
        else {
            mlayout.remove_item(test_button);
            cleanup();
        }
    });
    mlayout.add_item(test_button);
}

void LayoutAnimationTestRow::draw_background(const lgui::DrawEvent& de) const {
    de.gfx().rect(size_rect(), lgui::grey(1.0), 1);
}

void LayoutAnimationTestRow::cleanup() {
    lgui::erase_remove_if(mtest_buttons, [](auto& btn) {
        return !btn->is_added_to_gui();
    });
}

LayoutAnimationTest::LayoutAnimationTest() {
    mexplanation.set_text(
            "Left click removes a button, right click sets its visibility to GONE.\nClicking \"Restore\" makes all GONE blocks visible again.");
    mlayout.add_item(mexplanation);
    for (int i = 0; i < 3; ++i) {
        mrows.emplace_back(std::make_unique<LayoutAnimationTestRow>());
        mlayout.add_item({*mrows.back(), {0, 40, 0, 0}});
    }
    set_layout(&mlayout);
    mlayout_transition.set_root_widget(this);
}
