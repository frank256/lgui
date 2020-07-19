#include <cmath>
#include "transformationtest.h"
#include "lgui/platform/stringfmt.h"

#include "lgui/drawevent.h"
#include "lgui/platform/graphics.h"

static std::string to_string(float x) {
    char buff[32];
    snprintf(buff, 32, "%.2f", x);
    return buff;
}

TransformationTestControls::TransformationTestControls()
        : mlayout(3, 8) {
    mtranslation_x_lbl.set_text("Translation X:");
    mtranslation_x.set_min_value(0);
    mtranslation_x.set_max_value(1000);
    mtranslation_x.set_value(1);
    mtranslation_x.on_value_changed.connect([this](int v) {
        float x = float(v) / 10 - 50;
        mtransformation_state.translation.set_x(x);
        mtranslation_x_value_lbl.set_text(to_string(x));
        update();
    });

    mtranslation_y_lbl.set_text("Translation Y:");
    mtranslation_y.set_min_value(0);
    mtranslation_y.set_max_value(1000);
    mtranslation_y.set_value(1);
    mtranslation_y.on_value_changed.connect([this](int v) {
        float y = float(v) / 10 - 50;
        mtransformation_state.translation.set_y(y);
        mtranslation_y_value_lbl.set_text(to_string(y));
        update();
    });

    mrotation_z_lbl.set_text("Rotation Z:");
    mrotation_z.set_min_value(0);
    mrotation_z.set_max_value(360);
    mrotation_z.set_value(1);
    mrotation_z.on_value_changed.connect([this](int v) {
        mtransformation_state.rotation_z = float(v);
        mrotation_z_value_lbl.set_text(to_string(v));
        update();
    });

    mrotation_y_lbl.set_text("Rotation Y:");
    mrotation_y.set_min_value(0);
    mrotation_y.set_max_value(360);
    mrotation_y.set_value(1);
    mrotation_y.on_value_changed.connect([this](int v) {
        mtransformation_state.rotation_y = float(v);
        mrotation_y_value_lbl.set_text(to_string(v));
        update();
    });

    mrotation_x_lbl.set_text("Rotation X:");
    mrotation_x.set_min_value(0);
    mrotation_x.set_max_value(360);
    mrotation_x.set_value(1);
    mrotation_x.on_value_changed.connect([this](int v) {
        mtransformation_state.rotation_x = float(v);
        mrotation_x_value_lbl.set_text(to_string(v));
        update();
    });

    auto transform_scale = [](int slider) {
        return 0.25 * std::exp(0.0277259 * float(slider));
    };

    mscale_x_lbl.set_text("Scale X:");
    mscale_x.set_min_value(0);
    mscale_x.set_max_value(100);
    mscale_x.set_value(1);
    mscale_x.on_value_changed.connect([this, &transform_scale](int v) {
        float x = transform_scale(v);
        mtransformation_state.scale.set_x(x);
        mscale_x_value_lbl.set_text(to_string(x));
        update();
    });

    mscale_y_lbl.set_text("Scale Y:");
    mscale_y.set_min_value(0);
    mscale_y.set_max_value(100);
    mscale_y.set_value(1);
    mscale_y.on_value_changed.connect([this, &transform_scale](int v) {
        float y = transform_scale(v);
        mtransformation_state.scale.set_y(y);
        mscale_y_value_lbl.set_text(to_string(y));
        update();
    });

    mreset.set_text("Reset");
    mreset.on_activated.connect([this]() {
        mscale_x.set_value(50);
        mscale_y.set_value(50);
        mrotation_z.set_value(0);
        mrotation_y.set_value(0);
        mrotation_x.set_value(0);
        mtranslation_x.set_value(500);
        mtranslation_y.set_value(500);
    });

    mreset.on_activated.emit();
    mreset.on_activated.connect(on_reset);

    mlayout.set_default_alignment(lgui::Align::VCenter | lgui::Align::Right);

    mlayout.add_item(0, 0, mtranslation_x_lbl);
    mlayout.add_item(1, 0, mtranslation_x);
    mlayout.add_item(2, 0, mtranslation_x_value_lbl);
    mlayout.add_item(0, 1, mtranslation_y_lbl);
    mlayout.add_item(1, 1, mtranslation_y);
    mlayout.add_item(2, 1, mtranslation_y_value_lbl);
    mlayout.add_item(0, 2, mrotation_z_lbl);
    mlayout.add_item(1, 2, mrotation_z);
    mlayout.add_item(2, 2, mrotation_z_value_lbl);
    mlayout.add_item(0, 3, mrotation_y_lbl);
    mlayout.add_item(1, 3, mrotation_y);
    mlayout.add_item(2, 3, mrotation_y_value_lbl);
    mlayout.add_item(0, 4, mrotation_x_lbl);
    mlayout.add_item(1, 4, mrotation_x);
    mlayout.add_item(2, 4, mrotation_x_value_lbl);
    mlayout.add_item(0, 5, mscale_x_lbl);
    mlayout.add_item(1, 5, mscale_x);
    mlayout.add_item(2, 5, mscale_x_value_lbl);
    mlayout.add_item(0, 6, mscale_y_lbl);
    mlayout.add_item(1, 6, mscale_y);
    mlayout.add_item(2, 6, mscale_y_value_lbl);
    mlayout.add_item(0, 7, mreset);

    mlayout.set_col_min_width(2, font().char_width_hint() * 7);
    mlayout.set_row_spacing(10);
    mlayout.set_column_spacing(5);
    mlayout.set_column_stretch(1, 1);
    set_layout(&mlayout);
}

void TransformationTestControls::update() {
    on_transformation_state_changed.emit(mtransformation_state);
}

static void setup_frame_size(lgui::Frame& frame, int min_width) {
    lgui::Size size_hint = frame.min_size_hint();
    min_width = std::max(min_width, frame.font().text_width(frame.title()) + 2 * frame.font().char_width_hint());

    size_hint.set_w(std::max(size_hint.w(), min_width));
    frame.set_size(size_hint);
    frame._relayout();
    frame.transformation().set_pivot(lgui::PointF(frame.size_rect().center()));
}

TransformationTest::TransformationTest()
        : mframe("Test") {
    mlayout.add_item(mctrls);
    mlayout.add_spacing(30);
    set_layout(&mlayout);

    add_child(mframe);

    setup_frame_size(mframe, 10 * font().char_width_hint());
    mframe.set_clipped(false);
    mframe.set_pos(300, 250);
    mframe.set_outside_children_area(true);

    mctrls.on_transformation_state_changed.connect([this](const lgui::WidgetTransformationState& state) {
        mframe.transformation().set_state(state);
    });
    mctrls.on_reset.connect([this] {
        mframe.set_pos(300, 250);
    });
}

lgui::MeasureResults TransformationTest::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) {
    return lgui::force_size_constraints(lgui::Size(wc.value(), hc.value()), wc, hc);
}

