#ifndef LGUI_WIDGETTRANSFORMATIONSTATE_H
#define LGUI_WIDGETTRANSFORMATIONSTATE_H

#include "lgui/lgui_types.h"

namespace lgui {

struct WidgetTransformationState {
    PointF translation;
    float translation_z;
    float rotation_z, rotation_y, rotation_x;
    PointF scale;

    WidgetTransformationState()
            : translation(0, 0), translation_z(0), rotation_z(0), rotation_y(0), rotation_x(0),
              scale(1.0, 1.0) {}

    WidgetTransformationState(PointF translation, float translation_z, float rotation_z,
                              float rotation_y, float rotation_x, PointF scale)
            : translation(translation), translation_z(translation_z), rotation_z(rotation_z),
              rotation_y(rotation_y), rotation_x(rotation_x), scale(scale) {}

    bool is_identity() const {
        return rotation_z == 0.0 && rotation_y == 0.0 && rotation_x == 0.0
               && translation == PointF{0.0f, 0.0f}
               && translation_z == 0 && scale == PointF(1.0f, 1.0f);
    }

    void operator*=(float scalar) {
        scale *= scalar;
        translation *= scalar;
        translation_z *= scalar;
        rotation_z *= scalar;
        rotation_y *= scalar;
        rotation_x *= scalar;
    }

    friend WidgetTransformationState operator+(const WidgetTransformationState& a,
                                               const WidgetTransformationState& b) {
        return {a.translation + b.translation,
                a.translation_z + b.translation_z,
                a.rotation_z + b.rotation_z,
                a.rotation_y + b.rotation_y,
                a.rotation_x + b.rotation_x,
                a.scale + b.scale};
    }

    friend WidgetTransformationState operator-(const WidgetTransformationState& a,
                                               const WidgetTransformationState& b) {
        return {a.translation - b.translation,
                a.translation_z - b.translation_z,
                a.rotation_z - b.rotation_z,
                a.rotation_y - b.rotation_y,
                a.rotation_x - b.rotation_x,
                a.scale - b.scale};
    }

    friend WidgetTransformationState operator*(float scalar,
                                               const WidgetTransformationState& a) {
        WidgetTransformationState b = a;
        b*= scalar;
        return b;
    }

    friend WidgetTransformationState operator*(const WidgetTransformationState& a, float scalar) {
        WidgetTransformationState b = a;
        b*= scalar;
        return b;
    }

};

}

#endif //LGUI_WIDGETTRANSFORMATIONSTATE_H
