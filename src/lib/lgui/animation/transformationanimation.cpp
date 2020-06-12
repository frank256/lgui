#include "transformationanimation.h"

void lgui::TransformationAnimation::update_from_t(float t) {
    WidgetTransformationState diff = mend - mstart;

    WidgetTransformationState delta = mstart + diff * t;

    if (diff.scale != PointF()) {
        mtarget->set_scale(delta.scale);
    }
    if (diff.translation != PointF()) {
        mtarget->set_translation(delta.translation);
    }
    if (diff.translation_z != 0) {
        mtarget->set_translation_z(delta.translation_z);
    }
    if (diff.rotation_z != 0) {
        mtarget->set_rotation(delta.rotation_z);
    }
    if (diff.rotation_y != 0) {
        mtarget->set_rotation_y(delta.rotation_y);
    }
    if (diff.rotation_x != 0) {
        mtarget->set_rotation_x(delta.rotation_x);
    }
}

