#include <cmath>
#include "transformationtest.h"
#include "lgui/platform/stringfmt.h"

#include "lgui/drawevent.h"
#include "lgui/platform/graphics.h"

TestWidget::TestWidget(const std::string& text)
        : mbutton("Test", 64) {
    mtext_label.set_text(text);
    mlayout.add_item(mtext_label);
    mlayout.add_item(mbutton);
    set_layout(&mlayout);
}

TransformationTest::TransformationTest() {
    initialize_transformed_widget();
    manimate_button.set_text("Animate");
    mlayout.add_stretch(1);
    mlayout.add_item({mtransformed_widgets, lgui::Align::HVStretch});
    mlayout.add_item(manimate_button);
    mlayout.add_stretch(1);
    set_layout(&mlayout);
    manimate_button.on_activated.connect([this]() {
        animate();
    });
}

void TransformationTest::initialize_transformed_widget() {
    const int n = 16;
    for (int i = n - 1; i >= 0; --i) {
        char tmp[64];
        snprintf(tmp, 64, "Agent %03d", i);
        const std::string s = std::string(tmp);
        mtest_widgets.emplace_back(std::make_unique<TestWidget>(s));
        TestWidget& widget = *mtest_widgets.back();
        mtransformed_widgets.add_child(widget);
        widget.set_size(widget.measure_max({128, 128}));
        widget.transformation().set_pivot(lgui::PointF(widget.size_rect().center()));
        widget.set_opacity(0.5);
        float angle = (360.0 / n) * i - 90;
        float angle_rad = angle / 180.0 * ALLEGRO_PI;
        float r = 200;
        widget.transformation().set_rotation_y(angle);
        widget.transformation().set_translation_z(-r-sinf(angle_rad - ALLEGRO_PI / 2.0) * r);
        widget.transformation().set_translation({-cosf(angle_rad - -ALLEGRO_PI / 2.0) * r, 0});
//        widget.transformation().set_translation({(30+64)*i, 0});
//        widget.transformation().set_translation({-(30+64)*i, 0});
//        widget.transformation().set_translation_z(200 * (i));
    }
    mtransformed_widgets.transformation().set_rotation_x(-20);
}

void TransformationTest::draw_background(const lgui::DrawEvent& de) const {
    de.gfx().rect(mtransformed_widgets.rect(), lgui::grey(1.0), 1);
}

void TransformationTest::animate() {
    manimation_facilities.animate_value<float>()
            .from(0)
            .to(360.0)
            .with_duration(5.0)
            .with_value_setter([this](float t) {
                int n = mtest_widgets.size();
                for (int i = n - 1; i >= 0; --i) {
                    float angle = ((360.0 / n)) * i - 90 + t;
                    float angle_rad = angle / 180.0 * ALLEGRO_PI;
                    float r = 200;
                    Widget& widget = *mtest_widgets.at(i);
                    widget.transformation().set_rotation_y(angle);
                    widget.transformation().set_translation_z(-r-sinf(angle_rad - ALLEGRO_PI / 2.0) * r);
                    widget.transformation().set_translation({-cosf(angle_rad - -ALLEGRO_PI / 2.0) * r, 0});
                }
            })
            .build()
            .start();
}

