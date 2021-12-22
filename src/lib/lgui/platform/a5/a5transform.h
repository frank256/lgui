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

#ifndef LGUI_A5TRANSFORM_H
#define LGUI_A5TRANSFORM_H

#include <allegro5/transformations.h>

#include "lgui/lgui_types.h"

namespace lgui {

/** Class representing an Allegro 5 transformation. Do not use this class directly, but rather use Transformation.
    These classes enable WidgetTransformation to work, usually, there will be no need to use them directly. */
class A5Transform {
    public:
        /** Set this transformation to be the identity transformation. */
        void set_identity() { al_identity_transform(&mtransform); }

        /** Modify the translation component of this transformation. Translation will happen last. */
        void translate_post(PointF offset) {
            al_translate_transform(&mtransform, offset.x(), offset.y());
        }

        /** Modify the translation component  of this transformation. Translation will happen last. */
        void translate_post(PointF offset, float z_offset) {
            al_translate_transform_3d(&mtransform, offset.x(), offset.y(), z_offset);
        }

        /** Contrary to translate_post, this function will add a translation that will occurr first. */
        void translate_pre(PointF offset);

        /** Set the rotation around the Z axis. */
        void set_rotation(float degrees);
        /** Set a rotation around the vector given. */
        void set_rotation_3d(float unit_x, float unit_y, float unit_z, float degrees);
        /** Set the X and Y scale of this transformation. */
        void set_scale(PointF scale);

        /** Retrieve the corresponding Allegro 5 transformation. */
        const ALLEGRO_TRANSFORM& a5_transform() const {
            return mtransform;
        }

        /** Add the transformation that is passed so that it will happen after the current transformation. */
        void compose_post(const A5Transform& other) {
            al_compose_transform(&mtransform, &other.mtransform);
        }

        /** Add the transformation that is passed so that it will happen before the current transformation. */
        void compose_pre(const A5Transform& other);

        /** Map a 2D point. */
        PointF map(PointF p) const {
            float dx = p.x(), dy = p.y();
            al_transform_coordinates(&mtransform, &dx, &dy);
            return {dx, dy};
        }

        /** Return whether the transformation is invertable. */
        bool is_invertable() const {
            return al_check_inverse(&mtransform, 1e-7);
        }

        /** Return the inverse transformation. */
        A5Transform get_inverse() const {
            A5Transform t(*this);
            t.invert();
            return t;
        }

        /** Invert the transformation. */
        void invert() {
            al_invert_transform(&mtransform);
        }

        /** Get a (static) identity transformation. */
        static const A5Transform& get_identity() {
            return midentity;
        }

        /** Return an identity transformation. */
        static A5Transform create_identity() {
            A5Transform t;
            t.set_identity();
            return t;
        }

    private:
        ALLEGRO_TRANSFORM mtransform;
        static A5Transform midentity;
};

}

#endif //LGUI_A5TRANSFORM_H
