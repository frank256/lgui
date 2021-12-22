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

#include <cmath>

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_opengl.h>

#include "../bitmap.h"
#include "../font.h"
#include "../error.h"
#include "../graphics.h"
#include "../utf8.h"
#include "a5graphics.h"


namespace lgui {

A5Graphics::A5Graphics()
        : moffsx(0), moffsy(0) {
    ASSERT(al_get_current_display() != nullptr);
    mw = display_width();
    mh = display_height();
}

void A5Graphics::get_clip_rect(int& x, int& y, int& w, int& h) {
    al_get_clipping_rectangle(&x, &y, &w, &h);
}

void A5Graphics::set_clip_rect(int x, int y, int w, int h) {
    al_set_clipping_rectangle(x, y, w, h);
}

void A5Graphics::reset_clip_rect() {
    al_reset_clipping_rectangle();
}

void A5Graphics::clear(lgui::Color col) {
    al_clear_to_color(col);
}

void A5Graphics::flip() {
    al_flip_display();
}

int A5Graphics::display_width() const {
    ASSERT(al_get_current_display());
    return al_get_display_width(al_get_current_display());
}

int A5Graphics::display_height() const {
    ASSERT(al_get_current_display());
    return al_get_display_height(al_get_current_display());
}

void A5Graphics::draw_text(const A5Font& font, float x, float y, lgui::Color color, const std::string& text) {
    al_draw_text(font.mfnt, color, x, y, 0, text.c_str());
}

void A5Graphics::draw_textr(const A5Font& font, float x, float y, lgui::Color color, const std::string& text) {
    al_draw_text(font.mfnt, color, x - font.text_width(text), y, 0,
                 text.c_str());
}

void A5Graphics::draw_textc(const A5Font& font, float x, float y, lgui::Color color, const std::string& text) {
    al_draw_text(font.mfnt, color, x - font.text_width(text) / 2, y, 0,
                 text.c_str());
}

void A5Graphics::start_deferred_drawing() {
    al_hold_bitmap_drawing(true);
}

void A5Graphics::end_deferred_drawing() {
    al_hold_bitmap_drawing(false);
}

void A5Graphics::set_blender(Blender blender) {
    if (blender == BLENDER_ADD)
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
    else if (blender == BLENDER_MULTIPLY)
        al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);
    else if (blender == BLENDER_STD)
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
}

void A5Graphics::_error_shutdown() {
    if (al_get_current_display()) {
        al_destroy_display(al_get_current_display());
    }
}

void A5Graphics::start_drawing_to_bmp(lgui::Bitmap& bmp) {
    ASSERT(bmp.mbmp);
    al_set_target_bitmap(bmp.mbmp);
}

void A5Graphics::end_drawing_to_bmp(lgui::Bitmap& bmp) {
    ASSERT(bmp.mbmp);
    ASSERT(al_get_target_bitmap() == bmp.mbmp);
    restore_drawing_to_backbuffer();
}

void A5Graphics::end_drawing_to_bmp_perm(lgui::Bitmap& bmp) {
    ASSERT(bmp.mbmp);
    end_drawing_to_bmp(bmp);
    if (al_get_opengl_fbo(bmp.mbmp))
        al_remove_opengl_fbo(bmp.mbmp);
}

void A5Graphics::restore_drawing_to_backbuffer() {
    al_set_target_backbuffer(al_get_current_display());
}

void A5Graphics::draw_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int flip) {
    ASSERT(bitmap.mbmp);
    al_draw_bitmap(bitmap.mbmp, dx, dy, flip);
}

void A5Graphics::draw_tinted_bmp(const lgui::Bitmap& bitmap, int dx, int dy, lgui::Color col,
                                 int flip) {
    ASSERT(bitmap.mbmp);
    al_draw_tinted_bitmap(bitmap.mbmp, col, dx, dy, flip);
}

void A5Graphics::draw_bmp_region(const lgui::Bitmap& bitmap, int dx, int dy, int sx, int sy,
                                 int sw, int sh, int flip) {
    ASSERT(bitmap.mbmp);
    al_draw_bitmap_region(bitmap.mbmp, float(sx), float(sy), float(sw), float(sh),
                          float(dx), float(dy), flip);
}

void A5Graphics::draw_tinted_bmp_region(const lgui::Bitmap& bitmap, int dx, int dy, int sx, int sy,
                                        int sw, int sh, lgui::Color col, int flip) {
    ASSERT(bitmap.mbmp);
    al_draw_tinted_bitmap_region(bitmap.mbmp, col, float(sx), float(sy), float(sw), float(sh),
                                 float(dx), float(dy), flip);
}

void A5Graphics::draw_scaled_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int dw, int dh,
                                 int flip) const {
    ASSERT(bitmap.mbmp);
    al_draw_scaled_bitmap(bitmap.mbmp, 0, 0, bitmap.w(), bitmap.h(),
                          dx, dy, dw, dh, flip);
}


void A5Graphics::draw_tinted_scaled_bmp(const lgui::Bitmap& bitmap, int dx, int dy, int dw, int dh, lgui::Color col,
                                        int flip) const {
    ASSERT(bitmap.mbmp);
    al_draw_tinted_scaled_bitmap(bitmap.mbmp, col, 0, 0, bitmap.w(), bitmap.h(),
                                 dx, dy, dw, dh, flip);
}


void A5Graphics::draw_tinted_bmp_region_rounded_corners(const Bitmap& bitmap, float dx, float dy, float sx, float sy,
                                                        float sw, float sh, float crx, float cry, lgui::Color col) {
    const int PRIM_CACHE_SIZE = A5PrimHelper::PRIM_CACHE_SIZE;

    float x1 = dx, y1 = dy, x2 = dx + sw, y2 = dy + sh;
    ALLEGRO_VERTEX vertex_cache[PRIM_CACHE_SIZE];

    int num_segments = A5PrimHelper::PRIM_QUALITY * sqrtf((crx + cry) / 2.0f) / 4;

    ASSERT(crx > 0);
    ASSERT(cry > 0);

    if (num_segments * 4 >= PRIM_CACHE_SIZE) {
        num_segments = (PRIM_CACHE_SIZE - 1) / 4;
    }

    al_calculate_arc(&(vertex_cache[0].x), sizeof(ALLEGRO_VERTEX), 0, 0, crx, cry, 0, ALLEGRO_PI / 2, 0,
                     num_segments + 1);

    int ii;

    for (ii = 0; ii < num_segments; ii++) {
        vertex_cache[ii + 1 * num_segments].x = x1 + crx - vertex_cache[num_segments - 1 - ii].x;
        vertex_cache[ii + 1 * num_segments].y = y1 + cry - vertex_cache[num_segments - 1 - ii].y;
        vertex_cache[ii + 1 * num_segments].u = sx + crx - vertex_cache[num_segments - 1 - ii].x;
        vertex_cache[ii + 1 * num_segments].v = sy + cry - vertex_cache[num_segments - 1 - ii].y;;

        vertex_cache[ii + 2 * num_segments].x = x1 + crx - vertex_cache[ii].x;
        vertex_cache[ii + 2 * num_segments].y = y2 - cry + vertex_cache[ii].y;
        vertex_cache[ii + 2 * num_segments].u = sx + crx - vertex_cache[ii].x;
        vertex_cache[ii + 2 * num_segments].v = sy + sh - cry + vertex_cache[ii].y;

        vertex_cache[ii + 3 * num_segments].x = x2 - crx + vertex_cache[num_segments - 1 - ii].x;
        vertex_cache[ii + 3 * num_segments].y = y2 - cry + vertex_cache[num_segments - 1 - ii].y;
        vertex_cache[ii + 3 * num_segments].u = sx + sw - crx + vertex_cache[num_segments - 1 - ii].x;
        vertex_cache[ii + 3 * num_segments].v = sy + sh - cry + vertex_cache[num_segments - 1 - ii].y;
    }
    for (ii = 0; ii < num_segments; ii++) {
        float xoffs = vertex_cache[ii].x;
        float yoffs = vertex_cache[ii].y;
        vertex_cache[ii].x = x2 - crx + xoffs;
        vertex_cache[ii].y = y1 + cry - yoffs;
        vertex_cache[ii].u = sx + sw - crx + xoffs;
        vertex_cache[ii].v = sy + cry - yoffs;
    }

    for (ii = 0; ii < 4 * num_segments; ii++) {
        vertex_cache[ii].color = col;
        vertex_cache[ii].z = 0;
    }

    al_draw_prim(vertex_cache, nullptr, bitmap.mbmp, 0, 4 * num_segments, ALLEGRO_PRIM_TRIANGLE_FAN);
}

void A5Graphics::use_transform(const Transform& transform) {
    al_use_transform(&transform.a5_transform());
}

static int render_glyph(const ALLEGRO_FONT* f, const ALLEGRO_COLOR& color,
                        int32_t prev_ch, int32_t ch, float xpos, float ypos, const Rect& clip_rect) {
    ALLEGRO_GLYPH glyph;
    memset(&glyph, 0, sizeof(ALLEGRO_GLYPH));

    al_get_glyph(f, prev_ch, ch, &glyph);

    float dx = xpos + glyph.offset_x + glyph.kerning;
    if (dx + glyph.w <= clip_rect.x1())
        return glyph.advance;
    float dy = ypos + glyph.offset_y;

    if (glyph.bitmap != nullptr) {
        Rect clipped(dx, dy, glyph.w, glyph.h);
        clipped.clip_to(clip_rect);
        al_draw_tinted_bitmap_region(
                glyph.bitmap, color,
                glyph.x + clipped.x1() - dx, glyph.y + clipped.y1() - dy, clipped.w(), clipped.h(),
                clipped.x1(), clipped.y1(),
                0
        );
    }

    return glyph.advance;
}

void A5Graphics::draw_text_clipped_to_rect(const A5Font& font, float x, float y, lgui::Color color,
                                           const Rect& clip_rect, const std::string& text) {
    if (y >= clip_rect.y2())
        return;
    size_t pos = 0;
    int last_cp = -1;
    float xd = x;
    int cp;
    while ((cp = utf8::get_cp_next(text, pos)) >= 0 && xd < clip_rect.x2()) {
        xd += render_glyph(font.mfnt, color, last_cp, cp, xd, y, clip_rect);
        last_cp = cp;
    }
}

}
