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

#include <allegro5/transformations.h>
#include "a5transform.h"

#include <cmath>

namespace lgui {

A5Transform A5Transform::midentity = A5Transform::create_identity();

void A5Transform::translate_pre(PointF offset) {
    mtransform.m[3][0] += mtransform.m[0][0] * offset.x() + mtransform.m[1][0] * offset.y();
    mtransform.m[3][1] += mtransform.m[0][1] * offset.x() + mtransform.m[1][1] * offset.y();
    // Do we need that part?
    //mtransform.m[3][2] += mtransform.m[0][2] * offset.x() + mtransform.m[1][2] * offset.y();
}

void A5Transform::compose_pre(const A5Transform& other) {
#define E(x, y)                        \
      (mtransform.m[0][y] * other.mtransform.m[x][0] +  \
       mtransform.m[1][y] * other.mtransform.m[x][1] +  \
       mtransform.m[2][y] * other.mtransform.m[x][2] +  \
       mtransform.m[3][y] * other.mtransform.m[x][3])   \

    const ALLEGRO_TRANSFORM tmp = {{
            {E(0, 0), E(0, 1), E(0, 2), E(0, 3)},
            {E(1, 0), E(1, 1), E(1, 2), E(1, 3)},
            {E(2, 0), E(2, 1), E(2, 2), E(2, 3)},
            {E(3, 0), E(3, 1), E(3, 2), E(3, 3)}
    }};

    mtransform = tmp;
#undef E
}

void A5Transform::set_rotation(float degrees) {
    al_rotate_transform(&mtransform, degrees / 180.0 * M_PI);
}

void A5Transform::set_rotation_3d(float unit_x, float unit_y, float unit_z, float degrees) {
    al_rotate_transform_3d(&mtransform, unit_x, unit_y, unit_z, degrees / 180.0 * M_PI);
}

void A5Transform::set_scale(PointF scale) {
    al_scale_transform(&mtransform, scale.x(), scale.y());
}

}