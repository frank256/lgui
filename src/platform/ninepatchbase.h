/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

#ifndef LGUI_NINEPATCHBASE_H
#define LGUI_NINEPATCHBASE_H

#include "bitmap.h"
#include "lgui/lgui.h"


namespace lgui {

/** Base class for the platform specific 9-patch class. Do not use directly, but rather use NinePatch instead.
 */
class NinepatchBase {
    friend class Graphics;

    public:
        /** C'tor that will read a single bitmap. */
        NinepatchBase(Bitmap& src);

        /** C'tor that will read part of a bitmap. */
        NinepatchBase(Bitmap& src, int offsx, int offsy, int w, int h);

        NinepatchBase(const NinepatchBase& other) = delete;
        NinepatchBase operator=(const NinepatchBase& other) = delete;

        /** Move c'tor. */
        NinepatchBase(NinepatchBase&& other);

        /** Return the size of the 9-patch area that will be stretched. */
        lgui::Size stretch_size() const { return mstretch.size(); }

        /** Return the fill area offset. */
        lgui::Position content_offs() const { return mfill.pos(); }

        /** Return the fill area x-offset. */
        int content_x() const { return mfill.x1(); }
        /** Return the fill area y-offset. */
        int content_y() const { return mfill.y1(); }

        /** Return the size the content must have at least. */
        lgui::Size content_size_min() const { return mfill.size(); }

        /** Return how much padding there is to the right. */
        int content_padding_right() const {
            return mpic.w() - (mfill.w() + mfill.x());
        }
        /** Return how much padding there is at the bottom. */
        int content_padding_bottom() const {
            return mpic.h() - (mfill.h() + mfill.y());
        }

        /** Return the padding for the 9-patch, i.e. the parts not constituting the
            fill area. */
        lgui::Padding get_padding() const {
            return lgui::Padding(content_x(), content_y(),
                                content_padding_right(), content_padding_bottom());
        }

        /** Return how wide the whole 9-patch would be for content size cw. */
        int total_w(int cw) const {
            return stretch_w(cw) + unscaled_left_w() + unscaled_right_w();
        }
        /** Return how high the whole 9-patch would be for content size ch. */
        int total_h(int ch) const {
            return stretch_h(ch) + unscaled_top_h() + unscaled_bottom_h();
        }

        /** Return the size the whole 9-patch will take for a content_size. */
        lgui::Size total_size(const lgui::Size& content_size) const {
            return lgui::Size(total_w(content_size.w()),
                             total_h(content_size.h()));
        }

        /** Return the width the content should have for a resulting total width. */
        int content_for_total_size_w(int tw) const {
            return tw - unscaled_left_w() - unscaled_right_w() - (mstretch.w() - mfill.w());
        }

        /** Return the height the content should have for a resulting total height. */
        int content_for_total_size_h(int th) const {
            return th - unscaled_top_h() - unscaled_bottom_h() - (mstretch.h() - mfill.h());
        }

        /** Return the size the content should have for a resulting total size. */
        lgui::Size content_for_total_size(const lgui::Size& total) const {
            return lgui::Size(content_for_total_size_w(total.w()),
                             content_for_total_size_h(total.h()));
        }

    protected:
        /* Return sizes and positions of the unscaled parts,
         * i.e. those not in the center. These are offsets into the bitmap. */
        int unscaled_left_x() const { return mpic.x(); }
        int unscaled_top_y() const { return mpic.y(); }
        int unscaled_left_w() const { return mstretch.x() - mpic.x(); }
        int unscaled_top_h() const { return mstretch.y() - mpic.y(); }
        int unscaled_right_x() const { return mstretch.x2() + 1; }
        int unscaled_bottom_y() const { return mstretch.y2() + 1; }
        int unscaled_right_w() const { return mpic.w() - mstretch.w() - unscaled_left_w(); }
        int unscaled_bottom_h() const { return mpic.h() - mstretch.h() - unscaled_top_h(); }

        /* Calculate how large the stretch area has to be for content sizes. */
        int stretch_w(int cw) const;
        int stretch_h(int ch) const;

        void read_rects_from_bmp(Bitmap& bmp, int offsx, int offsy, int bmp_w, int bmp_h);

        const Bitmap& mbmp;
        lgui::Rect mstretch;
    private:
        lgui::Rect mfill;
        lgui::Rect mpic;

        static const int BORDER = 1; // for our pixels
};
}

#endif // LGUI_NINEPATCH_H
