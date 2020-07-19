/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-20 frank256
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

#ifndef LGUI_WIDGETTRANSFORMATION_H
#define LGUI_WIDGETTRANSFORMATION_H

#include <memory>
#include "platform/transform.h"
#include "widgettransformationstate.h"

namespace lgui {

namespace dtl {

class WidgetTransformationInternal {
    public:
        WidgetTransformationInternal();

        void set_translation(PointF translation);
        PointF translation() const { return mstate.translation; }

        void set_translation_z(float translation_z);
        float translation_z() const { return mstate.translation_z; }

        void set_pivot(PointF pivot);
        PointF pivot() const { return mpivot; }

        void set_scale(PointF scale);
        PointF scale() const { return mstate.scale; }

        void set_rotation(float rotation_z);
        float rotation() const { return mstate.rotation_z; }

        void set_rotation_x(float rotation_x);
        float rotation_x() const { return mstate.rotation_x; }
        void set_rotation_y(float rotation_y);
        float rotation_y() const { return mstate.rotation_y; }

        void set_state(const WidgetTransformationState& state);

        bool is_identity() const { return mis_identity; }

        const Transform& get_transform() const { return mtransform; }
        const Transform& get_inverse_transform() const { return minverse_transform; }

    private:
        void update_transform();
        bool _is_identity() const;

        WidgetTransformationState mstate;
        PointF mpivot;
        Transform mtransform, minverse_transform;
        bool mis_identity;
};

}

/** Maintains the transformation state of a widget. Currently supports translation, rotation around three axes, and
    scaling along the X and Y axes. Transformation are applied in a fixed order. Rotating and scaling occur around one
    pivot point that can be configured. Transformations are mainly intended for (animation) effects. They do not
    affect the layout space of a widget. For the rotations around the X and Y axes to look nice, you have to set up a
    perspective projection. Mouse tracking will work with transformation applied, however, perspective projection
    is not taken into account. So if you set up a perspective projection and have widgets translated on the Z-axis,
    mouse tracking will be off. It works if you stay on the z=0 plane before projection, though. Note that memory for
    transformations is only allocated if they are used (i.e. a property is modified). */
 class WidgetTransformation {
    public:

         /** Set the translation along the X and Y axes. */
        void set_translation(PointF translation) {
            maybe_alloc();
            mwt->set_translation(translation);
        }

         /** Return the translation along the X and Y axes. */
        PointF translation() const {
            return mwt ? mwt->translation() : PointF();
        }

         /** Set the translation along the Z axis. */
        void set_translation_z(float translation_z) {
            maybe_alloc();
            mwt->set_translation_z(translation_z);
        }

         /** Return the translation along the Z axis. */
        float translation_z() const {
            return mwt ? mwt->translation_z() : 0.0;
        }

         /** Set the pivot (X, Y) for scaling and rotating. */
        void set_pivot(PointF pivot) {
            maybe_alloc();
            mwt->set_pivot(pivot);
        }

         /** Return the pivot for scaling and rotating. */
        PointF pivot() const {
            return mwt ? mwt->pivot() : PointF();
        }

         /** Set the scale factors along the X and Y axes. */
        void set_scale(PointF scale) {
            maybe_alloc();
            mwt->set_scale(scale);
        }

         /** Return the scale factors along the X and Y axes. */
        PointF scale() const {
            return mwt ? mwt->scale() : PointF(1.0f, 1.0f);
        }

         /** Set the rotation around the Z axis (in degrees). */
        void set_rotation(float rotation_z) {
            maybe_alloc();
            mwt->set_rotation(rotation_z);
        }

         /** Return the rotation around the Z axis (in degrees). */
        float rotation() const {
            return mwt ? mwt->rotation() : 0.0;
        }

         /** Set the rotation around the X axis (in degrees). */
         void set_rotation_x(float rotation_x) {
            maybe_alloc();
            mwt->set_rotation_x(rotation_x);
        }

         /** Return the rotation around the X axis (in degrees). */
        float rotation_x() const {
            return mwt ? mwt->rotation_x() : 0.0;
        }

         /** Set the rotation around the Y axis (in degrees). */
        void set_rotation_y(float rotation_y) {
            maybe_alloc();
            mwt->set_rotation_y(rotation_y);
        }

         /** Return the rotation around the Y axis (in degrees). */
        float rotation_y() const {
            return mwt ? mwt->rotation_y() : 0.0;
        }

         /** Set all properties except the pivot at once. */
        void set_state(const WidgetTransformationState& state) {
            maybe_alloc();
            mwt->set_state(state);
        }

        /** Return whether the resulting transformation is the identity transformation. */
        bool is_identity() const { return mwt ? mwt->is_identity() : true; }

         /** Retrieve the resulting transformation. This transforms out of the local coordinate space. */
        const Transform& get_transform() const { return mwt ? mwt->get_transform() : Transform::get_identity(); }

         /** Retrieve the inverse resulting transformation if there is one. This transforms into the local coordinate space. */
        const Transform& get_inverse_transform() const {
            return mwt ? mwt->get_inverse_transform() : Transform::get_identity();
        }

         /** Reserve memory for transformation effects if necessary. You do not have to call this manually. */
         void maybe_alloc() {
             if (!mwt)
                 alloc();
         }

     private:
         void alloc();

         std::unique_ptr<dtl::WidgetTransformationInternal> mwt;
};

}

#endif //LGUI_WIDGETTRANSFORMATION_H
