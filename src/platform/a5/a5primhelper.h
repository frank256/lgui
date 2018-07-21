/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_A5_PRIMHELPER_H
#define LGUI_A5_PRIMHELPER_H

#include "../color.h"
#include <allegro5/allegro_primitives.h>
#include <vector>

namespace lgui {

enum class PrimType {
    PRIM_POINT_LIST=ALLEGRO_PRIM_POINT_LIST,
    PRIM_LINE_LIST=ALLEGRO_PRIM_LINE_LIST,
    PRIM_LINE_STRIP=ALLEGRO_PRIM_LINE_STRIP,
    PRIM_LINE_LOOP= ALLEGRO_PRIM_LINE_LOOP,
    PRIM_TRIANGLE_LIST=ALLEGRO_PRIM_TRIANGLE_LIST ,
    PRIM_TRIANGLE_STRIP= ALLEGRO_PRIM_TRIANGLE_STRIP,
    PRIM_TRIANGLE_FAN =ALLEGRO_PRIM_TRIANGLE_FAN
};

/** Helper to draw primitives using Allegro 5. Do not use directly, but use the Graphics class to draw
 *  everything. This class will draw without taking an offset into consideration. */
class A5PrimHelper
{
    public:
        static const constexpr int PRIM_CACHE_SIZE = ALLEGRO_VERTEX_CACHE_SIZE;
        static const constexpr float PRIM_QUALITY = ALLEGRO_PRIM_QUALITY;

        A5PrimHelper();
        ~A5PrimHelper();

        A5PrimHelper(const A5PrimHelper& other) = delete;
        A5PrimHelper(const A5PrimHelper &&other) = delete;
        A5PrimHelper operator=(const A5PrimHelper& other) = delete;

        void init();

        void rect(float x1, float y1, float x2, float y2, lgui::Color col, float thickness);
        void filled_rect(float x1, float y1, float x2, float y2, lgui::Color col);
        void filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, lgui::Color col);
        void rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col, float thickness);
        void filled_rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry,  lgui::Color col);
        void filled_rounded_rect_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                          const lgui::Color& col1, const lgui::Color& col2, lgui::GradientDirection dir);
        void filled_rect_gradient(float x1, float y1, float x2, float y2, const lgui::Color& col1, const lgui::Color& col2,
                                  lgui::GradientDirection dir);

        void rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col,
                                       float thickness, int corners);
        void filled_rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry,
                                              lgui::Color color, int corners);
        void rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color color,
                                  float thickness, lgui::OpenEdge oe);
        void rect_bracket(float x1, float y1, float x2, float y2, lgui::Color color, float thickness, lgui::OpenEdge oe);

        void filled_rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry,
                                         lgui::Color color, lgui::OpenEdge oe);
        void filled_rounded_rect_bracket_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                                  const lgui::Color& col1, const lgui::Color& col2, lgui::OpenEdge oe,
                                                  lgui::GradientDirection dir);

        void circle(float cx, float cy, float r, lgui::Color col, float thickness);
        void filled_circle(float cx, float cy, float r, lgui::Color col);
        void line(float x1, float y1, float x2, float y2, lgui::Color col, float thickness);
        void draw_visible_pixel(float px, float py, lgui::Color col);
        void draw_filled_pieslice(float cx, float cy, float r, float start_theta,
                                  float delta_theta, ALLEGRO_COLOR color);

        void draw_vertices(lgui::PrimType type, const PrimVertex* first, unsigned int start,
                           unsigned int end) const;
        void draw_vertices(lgui::PrimType type, const std::vector<PrimVertex>& verts, unsigned int start,
                           unsigned int end) const;

        void draw_vertices_indexed(lgui::PrimType type, const std::vector<PrimVertex>& verts,
                                   const std::vector<int>& indices, unsigned int n) const;

        static void calc_pie_slice(std::vector<PrimVertex>& dest, float cx, float cy, float rx, float ry,
                            float start_theta, float delta_theta, unsigned int num_segments);
        static void calc_arc(float* dest, int stride, float cx, float cy, float rx, float ry,
                      float start_theta, float delta_theta, float thickness, int num_points);
    private:
        ALLEGRO_VERTEX_DECL *mprim_vertex_decl;

};
}
#endif // LGUI_A5_PRIMHELPER_H
