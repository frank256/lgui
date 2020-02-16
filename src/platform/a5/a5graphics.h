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

#ifndef LGUI_A5_GRAPHICS_H
#define LGUI_A5_GRAPHICS_H

#include "../font.h"
#include "../color.h"
#include "lgui/lgui.h"


namespace lgui {

class Bitmap;

/** Class representing an Allegro 5 graphics context, providing drawing operations. Do not use this class
 *  directly, but rather use graphics. */
class A5Graphics {
    public:
        enum Blender { BLENDER_STD, BLENDER_ADD, BLENDER_MULTIPLY };

        A5Graphics();

        A5Graphics(const A5Graphics& other) = delete;
        A5Graphics(const A5Graphics &&other) = delete;
        A5Graphics operator=(const A5Graphics& other) = delete;

        int display_width() const;
        int display_height() const;
        void set_clip_rect(int x, int y, int w, int h);
        void get_clip_rect(int& x, int& y, int& w, int& h);
        void reset_clip_rect();

        void start_drawing_to_bmp(lgui::Bitmap& bmp);
        void end_drawing_to_bmp(lgui::Bitmap& bmp);
        void end_drawing_to_bmp_perm(lgui::Bitmap& bmp);

        void restore_drawing_to_backbuffer();

        void start_deferred_drawing();
        void end_deferred_drawing();

        void set_blender(Blender blender);

        void clear(lgui::Color col);

        void draw_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int flip=0);
        void draw_tinted_bmp(const lgui::Bitmap& bitmap, int dx, int dy, lgui::Color col,
                         int flip=0);
        void draw_bmp_region(const lgui::Bitmap& bitmap, int dx, int dy, int sx, int sy,
                         int sw, int sh, int flip=0);
        void draw_tinted_bmp_region(const lgui::Bitmap& bitmap, int dx, int dy, int sx, int sy,
                                int sw, int sh, lgui::Color col, int flip=0);

        void draw_scaled_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int dw, int dh, int flip=0) const;

        void draw_tinted_scaled_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int dw, int dh, lgui::Color col,
                                 int flip=0) const;
        void draw_tinted_bmp_region_rounded_corners(const lgui::Bitmap& bitmap, float dx, float dy,
                                                    float sx, float sy, float sw, float sh,
                                                    float crx, float cry, lgui::Color col);

        void draw_text(const A5Font& font, float x, float y, lgui::Color color, const std::string& text);
        void draw_textr(const A5Font& font, float x, float y, lgui::Color color, const std::string& text);
        void draw_textc(const A5Font& font, float x, float y, lgui::Color color, const std::string& text);


        static void _error_shutdown();

    protected:
        int moffsx, moffsy, mw, mh;
};

}
#endif // LGUI_A5_GRAPHICS_H

