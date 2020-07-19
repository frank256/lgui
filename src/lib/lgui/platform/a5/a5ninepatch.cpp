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

#include <algorithm>

#include "../ninepatch.h"
#include "../graphics.h"

namespace lgui {

A5Ninepatch::A5Ninepatch(Bitmap& src, int offsx, int offsy, int w, int h)
        : NinepatchBase(src, offsx, offsy, w, h) {}


A5Ninepatch::A5Ninepatch(Bitmap& src)
        : NinepatchBase(src) {}

A5Ninepatch::A5Ninepatch(A5Ninepatch&& other) noexcept
        : NinepatchBase(std::forward<A5Ninepatch>(other)) {}

void A5Ninepatch::draw_tinted(const lgui::Color& col, float dx, float dy, const lgui::Size& content_size) const {
    int strw = stretch_w(content_size.w());
    int strh = stretch_h(content_size.h());

    // upper-left corner
    al_draw_tinted_bitmap_region(mbmp.mbmp, col, unscaled_left_x(), unscaled_top_y(),
                                 unscaled_left_w(), unscaled_top_h(),
                                 dx, dy, 0);

    // lower-left corner
    al_draw_tinted_bitmap_region(mbmp.mbmp, col, unscaled_left_x(), unscaled_bottom_y(),
                                 unscaled_left_w(), unscaled_bottom_h(),
                                 dx, dy + unscaled_top_h() + strh, 0);

    // upper-right corner
    al_draw_tinted_bitmap_region(mbmp.mbmp, col, unscaled_right_x(), unscaled_top_y(),
                                 unscaled_right_w(), unscaled_top_h(),
                                 dx + unscaled_left_w() + strw, dy, 0);

    // lower-right corner
    al_draw_tinted_bitmap_region(mbmp.mbmp, col, unscaled_right_x(), unscaled_bottom_y(),
                                 unscaled_right_w(), unscaled_bottom_h(),
                                 dx + unscaled_left_w() + strw, dy + unscaled_top_h() + strh, 0);

    // stretch center
    al_draw_tinted_scaled_bitmap(mbmp.mbmp, col, mstretch.x(), mstretch.y(),
                                 mstretch.w(), mstretch.h(),
                                 dx + unscaled_left_w(), dy + unscaled_top_h(), strw, strh, 0);

    // left side
    al_draw_tinted_scaled_bitmap(mbmp.mbmp, col, unscaled_left_x(), mstretch.y(),
                                 unscaled_left_w(), mstretch.h(),
                                 dx, dy + unscaled_top_h(), unscaled_left_w(), strh, 0);

    // top side
    al_draw_tinted_scaled_bitmap(mbmp.mbmp, col, mstretch.x(), unscaled_top_y(),
                                 mstretch.w(), unscaled_top_h(),
                                 dx + unscaled_left_w(), dy, strw, unscaled_top_h(), 0);

    // right side
    al_draw_tinted_scaled_bitmap(mbmp.mbmp, col, unscaled_right_x(), mstretch.y(),
                                 unscaled_right_w(), mstretch.h(),
                                 dx + unscaled_left_w() + strw, dy + unscaled_top_h(),
                                 unscaled_right_w(), strh, 0);

    // bottom side
    al_draw_tinted_scaled_bitmap(mbmp.mbmp, col, mstretch.x(), unscaled_bottom_y(),
                                 mstretch.w(), unscaled_bottom_h(),
                                 dx + unscaled_left_w(), dy + unscaled_top_h() + strh,
                                 strw, unscaled_bottom_h(), 0);
}


}
