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

#include "widgettransformation.h"

namespace lgui {

WidgetTransformation::WidgetTransformation()
        : mpivot(0, 0), mis_identity(true) {
    mtransform.set_identity();
    minverse_transform.set_identity();
}

void WidgetTransformation::set_translation(PointF translation) {
    if (mstate.translation != translation) {
        mstate.translation = translation;
        update_transform();
    }
}

void WidgetTransformation::set_translation_z(float translation_z) {
    if (mstate.translation_z != translation_z) {
        mstate.translation_z = translation_z;
        update_transform();
    }
}

void WidgetTransformation::set_pivot(PointF pivot) {
    if (mpivot != pivot) {
        mpivot = pivot;
        update_transform();
    }
}

void WidgetTransformation::set_scale(PointF scale) {
    if (mstate.scale != scale) {
        mstate.scale = scale;
        update_transform();
    }
}

void WidgetTransformation::set_rotation(float rotation_degrees) {
    if (mstate.rotation_z != rotation_degrees) {
        mstate.rotation_z = rotation_degrees;
        update_transform();
    }
}

void WidgetTransformation::set_rotation_x(float rotation_x) {
    if (mstate.rotation_x != rotation_x) {
        mstate.rotation_x = rotation_x;
        update_transform();
    }
}
void WidgetTransformation::set_rotation_y(float rotation_y) {
    if (mstate.rotation_y != rotation_y) {
        mstate.rotation_y = rotation_y;
        update_transform();
    }
}

void WidgetTransformation::update_transform() {
    mtransform.set_identity();
    mtransform.translate_post(-mpivot);
    mtransform.set_rotation_3d(1.0, 0.0, 0.0, mstate.rotation_x);
    mtransform.set_rotation_3d(0.0, 1.0, 0.0, mstate.rotation_y);
    mtransform.set_rotation(mstate.rotation_z);
    mtransform.set_scale(mstate.scale);
    mtransform.translate_post(mstate.translation + mpivot, mstate.translation_z);

    mis_identity = _is_identity();

    minverse_transform = mtransform.get_inverse();
}

bool WidgetTransformation::_is_identity() const {
    return mstate.is_identity();
}

void WidgetTransformation::set_state(const WidgetTransformationState& state) {
    mstate = state;
    // FIXME: change detection?
    update_transform();
}

}

