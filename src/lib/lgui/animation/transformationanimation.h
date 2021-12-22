/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LGUI_TRANSFORMATIONANIMATION_H
#define LGUI_TRANSFORMATIONANIMATION_H

#include "lgui/widgettransformation.h"
#include "lgui/widgettransformationstate.h"
#include "lgui/lgui_types.h"
#include "valueanimation.h"

namespace lgui {

/** Convenience class to aniamte the transformation of a widget. This is especially useful if you want to animate several
    transformation properties at once. All properties will be animated indepedently.*/
class TransformationAnimation : public ValueAnimationBase {
    public:
        TransformationAnimation() : mtarget(nullptr) {}

        /** Set the target to animate. */
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
