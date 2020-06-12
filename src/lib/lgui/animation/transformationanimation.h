#ifndef LGUI_TRANSFORMATIONANIMATION_H
#define LGUI_TRANSFORMATIONANIMATION_H

#include <lgui/widgettransformation.h>
#include "../widgettransformationstate.h"
#include "lgui/lgui_types.h"
#include "valueanimation.h"

namespace lgui {

class TransformationAnimation : public ValueAnimationBase {
    public:
        TransformationAnimation() : mtarget(nullptr) {}

        void set_target(WidgetTransformation& widget_transformation) {
            mtarget = &widget_transformation;
        }

        void set_start_scale(const PointF& start_scale) {
            mstart.scale = start_scale;
        }

        void set_end_scale(const PointF& end_scale) {
            mend.scale = end_scale;
        }

        void set_start_translation(const PointF& start_translation) {
            mstart.translation = start_translation;
        }

        void set_end_translation(const PointF& end_translation) {
            mend.translation = end_translation;
        }

        void set_start_translation_z(float start_translation_z) {
            mstart.translation_z = start_translation_z;
        }

        void set_end_translation_z(float end_translation_z) {
            mend.translation_z = end_translation_z;
        }

        void set_start_rotation_z(float start_rotation_z) {
            mstart.rotation_z = start_rotation_z;
        }

        void set_end_rotation_z(float end_rotation_z) {
            mend.rotation_z = end_rotation_z;
        }

        void set_start_rotation_y(float start_rotation_y) {
            mstart.rotation_y = start_rotation_y;
        }

        void set_end_rotation_y(float end_rotation_y) {
            mend.rotation_y = end_rotation_y;
        }

        void set_start_rotation_x(float start_rotation_x) {
            mstart.rotation_x = start_rotation_x;
        }

        void set_end_rotation_x(float end_rotation_x) {
            mend.rotation_x = end_rotation_x;
        }

    protected:
        void update_from_t(float t) override;

    private:
        WidgetTransformationState mstart, mend;
        WidgetTransformation* mtarget;
};

}

#endif //LGUI_TRANSFORMATIONANIMATION_H
