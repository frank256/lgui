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
