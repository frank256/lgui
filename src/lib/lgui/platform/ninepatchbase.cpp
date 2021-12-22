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

#include <algorithm>

#include "ninepatchbase.h"

namespace lgui {

NinepatchBase::NinepatchBase(Bitmap& src, int offsx, int offsy, int w, int h)
        : mbmp(src) {
    read_rects_from_bmp(src, offsx, offsy, w, h);
    mpic = lgui::Rect(offsx + BORDER, offsy + BORDER, w - 2 * BORDER, h - 2 * BORDER);
}

NinepatchBase::NinepatchBase(Bitmap& src)
        : mbmp(src) {
    read_rects_from_bmp(src, 0, 0, src.w(), src.h());
    mpic = lgui::Rect(BORDER, BORDER, src.w() - 2 * BORDER, src.h() - 2 * BORDER);
}

NinepatchBase::NinepatchBase(NinepatchBase&& other) noexcept
        : mbmp(other.mbmp),
          mstretch(other.mstretch),
          mfill(other.mfill),
          mpic(other.mpic) {
}


int NinepatchBase::stretch_w(int cw) const {
    return std::max(cw - mfill.w() + mstretch.w(), mstretch.w());
}

int NinepatchBase::stretch_h(int ch) const {
    return std::max(ch - mfill.h() + mstretch.h(), mstretch.h());
}


static inline bool is_stretch_color(lgui::Color& c) {
    unsigned char r, g, b, a;
    lgui::col_get_rgba_i(c, r, g, b, a);
    return r == 0 && g == 0 && b == 0 && a == 255;
}

static inline bool is_padd_color(lgui::Color& c) {
    unsigned char r, g, b, a;
    lgui::col_get_rgba_i(c, r, g, b, a);
    return r == 0 && g == 0 && b == 0 && a == 255;
}


void NinepatchBase::read_rects_from_bmp(Bitmap& bmp, int offsx, int offsy, int w, int h) {
    bmp.lock_for_reading();

    // We have a black border at the left and top side telling us where to stretch,
    // and black border at the right and bottom side, telling us where to place the content.

    bool is_stretch = false;
    bool is_padd = false;

    lgui::Rect padd;

    int stretch_begin = offsx, stretch_d = 0;
    int padd_begin = offsx, padd_d = 0;

    for (int x = offsx; x < offsx + w; x++) {
        lgui::Color c1 = bmp.getpixel(x, offsy);
        lgui::Color c2 = bmp.getpixel(x, offsy + h - 1);
        if (is_stretch_color(c1)) {
            if (!is_stretch) {
                is_stretch = true;
                stretch_begin = x;
                stretch_d = 1;
            }
            else
                stretch_d++;
        }
        if (is_padd_color(c2)) {
            if (!is_padd) {
                is_padd = true;
                padd_begin = x;
                padd_d = 1;
            }
            else
                padd_d++;
        }
    }

    mstretch.set_pos_x(stretch_begin);
    mstretch.set_width(stretch_d);
    padd.set_pos_x(padd_begin);
    padd.set_width(padd_d);

    padd_begin = stretch_begin = offsy;
    stretch_d = padd_d = 0;
    is_stretch = is_padd = false;

    for (int y = offsy; y < offsy + bmp.h(); y++) {
        lgui::Color c1 = bmp.getpixel(offsx, y);
        lgui::Color c2 = bmp.getpixel(offsx + w - 1, y);
        if (is_stretch_color(c1)) {
            if (!is_stretch) {
                is_stretch = true;
                stretch_begin = y;
                stretch_d = 1;
            }
            else
                stretch_d++;
        }
        if (is_padd_color(c2)) {
            if (!is_padd) {
                is_padd = true;
                padd_begin = y;
                padd_d = 1;
            }
            else
                padd_d++;
        }
    }
    mstretch.set_pos_y(stretch_begin);
    mstretch.set_height(stretch_d);
    padd.set_pos_y(padd_begin);
    padd.set_height(padd_d);

    padd.translate(-offsx - BORDER, -offsy - BORDER);

    mfill = padd;

    bmp.unlock();

}

}
