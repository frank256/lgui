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

#include "../primhelper.h"
#include "../error.h"

#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#else
#include <cmath>
#endif



static const ALLEGRO_VERTEX_ELEMENT _prim_vertex_elems[] = {
   {ALLEGRO_PRIM_POSITION, ALLEGRO_PRIM_FLOAT_2, offsetof(lgui::PrimVertex, x)},
   {ALLEGRO_PRIM_COLOR_ATTR, 0, offsetof(lgui::PrimVertex, color)},
   {0, 0, 0}
};


static inline void assign_gradient_colors(const lgui::Color* col1, const lgui::Color* col2, lgui::GradientDirection dir,
                                          const lgui::Color** tl_col, const lgui::Color** tr_col, const lgui::Color** bl_col, const lgui::Color** br_col) {
    if(dir == lgui::GradientDirection::RightToLeft || dir == lgui::GradientDirection::BottomToTop) {
        *tl_col = col2;
        *br_col = col1;
    }
    else {
        *tl_col = col1;
        *br_col = col2;
    }
    if(dir == lgui::GradientDirection::LeftToRight || dir == lgui::GradientDirection::RightToLeft) {
        *bl_col = *tl_col;
        *tr_col = *br_col;
    }
    else {
        *bl_col = *br_col;
        *tr_col = *tl_col;
    }
}

namespace lgui {

A5PrimHelper::A5PrimHelper()
    : mprim_vertex_decl(nullptr)
{
    init();
}

A5PrimHelper::~A5PrimHelper()
{
    if(mprim_vertex_decl) {
        al_destroy_vertex_decl(mprim_vertex_decl);
        mprim_vertex_decl = nullptr;
    }
}

void A5PrimHelper::init()
{
    if (!mprim_vertex_decl)
        mprim_vertex_decl = al_create_vertex_decl(_prim_vertex_elems, sizeof(PrimVertex));
}

void A5PrimHelper::rect(float x1, float y1, float x2, float y2, lgui::Color col, float thickness)
{
    al_draw_rectangle(x1, y1, x2, y2,
                      col, thickness);
}

void A5PrimHelper::rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col, float thickness)
{
    al_draw_rounded_rectangle(x1, y1, x2, y2,
                              rx, ry, col, thickness);
}

void A5PrimHelper::filled_rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col)
{
    al_draw_filled_rounded_rectangle(x1, y1, x2, y2, rx, ry, col);
}


void A5PrimHelper::circle(float cx, float cy, float r, lgui::Color col, float thickness)
{
    al_draw_circle(cx, cy, r, col, thickness);
}

void A5PrimHelper::filled_circle(float cx, float cy, float r, lgui::Color col)
{
    al_draw_filled_circle(cx, cy, r, col);
}

void A5PrimHelper::line(float x1, float y1, float x2, float y2, lgui::Color col, float thickness)
{
    al_draw_line(x1, y1, x2, y2, col, thickness);
}

void A5PrimHelper::filled_rect(float x1, float y1, float x2, float y2, lgui::Color col)
{
    al_draw_filled_rectangle(x1, y1, x2, y2, col);
}

void A5PrimHelper::filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, lgui::Color col)
{
    al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, col);
}


void A5PrimHelper::filled_rounded_rect_gradient(float x1, float y1, float x2, float y2, float rx, float ry, const lgui::Color& col1,
                                            const lgui::Color& col2, lgui::GradientDirection dir)
{
    if (rx <= 0 || ry <= 0) {
       filled_rect_gradient(x1, y1, x2, y2, col1, col2, dir);
       return;
    }


    PrimVertex vertex_cache[PRIM_CACHE_SIZE];
    int ii;
    const float scale = 1.0;
    int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

    ASSERT(rx >= 0);
    ASSERT(ry >= 0);


    if (num_segments * 4+2 >= PRIM_CACHE_SIZE) {
       num_segments = (PRIM_CACHE_SIZE - 2) / 4;
    }

    calc_arc(&(vertex_cache[1].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

    const lgui::Color* tl_col;
    const lgui::Color* br_col;
    const lgui::Color* tr_col;
    const lgui::Color* bl_col;

    assign_gradient_colors(&col1, &col2, dir, &tl_col, &tr_col, &bl_col, &br_col);

    vertex_cache[0].x = (x1 + x2) / 2.0;
    vertex_cache[0].y = (y1 + y2) / 2.0;
    vertex_cache[0].color = lgui::rgba((col1.r+col2.r) / 2.0,
                                       (col1.g+col2.g) / 2.0,
                                       (col1.b+col2.b) / 2.0,
                                       (col1.a+col2.a) / 2.0);

    for (ii = 0; ii < num_segments; ii++) {
       vertex_cache[1 + ii + 1 * num_segments].x = x1 + rx - vertex_cache[num_segments - ii].x;
       vertex_cache[1 + ii + 1 * num_segments].y = y1 + ry - vertex_cache[num_segments - ii].y;
       vertex_cache[1 + ii + 1 * num_segments].color = *tl_col;

       vertex_cache[1 + ii + 2 * num_segments].x = x1 + rx - vertex_cache[ii+1].x;
       vertex_cache[1 + ii + 2 * num_segments].y = y2 - ry + vertex_cache[ii+1].y;
       vertex_cache[1 + ii + 2 * num_segments].color = *bl_col;

       vertex_cache[1 + ii + 3 * num_segments].x = x2 - rx + vertex_cache[num_segments - ii].x;
       vertex_cache[1 + ii + 3 * num_segments].y = y2 - ry + vertex_cache[num_segments - ii].y;
       vertex_cache[1 + ii + 3 * num_segments].color = *br_col;
    }
    for (ii = 0; ii < num_segments; ii++) {
       vertex_cache[1 + ii].x = x2 - rx + vertex_cache[ii+1].x;
       vertex_cache[1 + ii].y = y1 + ry - vertex_cache[ii+1].y;
       vertex_cache[1 + ii].color = *tr_col;
    }
    vertex_cache[4*num_segments+1] = vertex_cache[1]; // close the fan

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vertex_cache, 0, 4*num_segments + 2);
}

void A5PrimHelper::filled_rect_gradient(float x1, float y1, float x2, float y2, const lgui::Color& col1, const lgui::Color& col2, lgui::GradientDirection dir)
{
    PrimVertex vtx[4];

    vtx[0].x = x1; vtx[0].y = y1;
    vtx[1].x = x1; vtx[1].y = y2;
    vtx[2].x = x2; vtx[2].y = y2;
    vtx[3].x = x2; vtx[3].y = y1;


    if(dir == lgui::GradientDirection::RightToLeft || dir == lgui::GradientDirection::BottomToTop) {
        vtx[0].color = col2;
        vtx[2].color = col1;
    }
    else {
        vtx[0].color = col1;
        vtx[2].color = col2;
    }

    if(dir == lgui::GradientDirection::LeftToRight || dir == lgui::GradientDirection::RightToLeft) {
        vtx[1].color = vtx[0].color;
        vtx[3].color = vtx[2].color;
    }
    else {
        vtx[1].color = vtx[2].color;
        vtx[3].color = vtx[0].color;
    }

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vtx, 0, 4);
}
}

// helpers for drawing custom rounded rectangles

static inline void init_rr_corner_thick_tl(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x1, float y1)
{
    const int last_arc_offs = arc_offs + 2 * num_segments -1;
    for(int ii = 0; ii < 2 * num_segments; ii += 2) {
        vc[offs + ii + 1].x = x1 + rx - vc[last_arc_offs - ii].x;
        vc[offs + ii + 1].y = y1 + ry - vc[last_arc_offs - ii].y;
        vc[offs + ii].x =     x1 + rx - vc[last_arc_offs  - ii - 1].x;
        vc[offs + ii].y =     y1 + ry - vc[last_arc_offs  - ii - 1].y;
    }
    offs+=2*num_segments;
}

static inline void init_rr_corner_thick_tr(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x2, float y1)
{
    for(int ii = 0; ii < 2 * num_segments; ii += 2) {
        vc[offs + ii].x = x2 - rx + vc[arc_offs + ii].x;
        vc[offs + ii].y = y1 + ry - vc[arc_offs + ii].y;
        vc[offs + ii + 1].x = x2 - rx + vc[arc_offs + ii + 1].x;
        vc[offs + ii + 1].y = y1 + ry - vc[arc_offs + ii + 1].y;
    }
    offs += 2*num_segments;
}

static inline void init_rr_corner_thick_bl(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x1, float y2)
{
    for(int ii = 0; ii < 2 * num_segments; ii += 2) {
        vc[offs + ii].x =     x1 + rx - vc[arc_offs+ii].x;
        vc[offs + ii].y =     y2 - ry + vc[arc_offs+ii].y;
        vc[offs + ii + 1].x = x1 + rx - vc[arc_offs+ii + 1].x;
        vc[offs + ii + 1].y = y2 - ry + vc[arc_offs+ii + 1].y;
    }
    offs+=2*num_segments;
}

static inline void init_rr_corner_thick_br(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x2, float y2)
{
    const int last_arc_offs = arc_offs + 2 * num_segments -1;
    for(int ii = 0; ii < 2 * num_segments; ii += 2) {
        vc[offs + ii + 1].x = x2 - rx + vc[last_arc_offs  - ii].x;
        vc[offs + ii + 1].y = y2 - ry + vc[last_arc_offs  - ii].y;
        vc[offs + ii].x = x2 - rx + vc[last_arc_offs  - ii - 1].x;
        vc[offs + ii].y = y2 - ry + vc[last_arc_offs  - ii - 1].y;
    }
    offs+=2*num_segments;
}

static inline void init_rr_corner_hairline_tl(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x1, float y1, const lgui::Color& color)
{
    const int last_arc_offs = arc_offs + num_segments -1;
    for(int ii = 0; ii < num_segments; ii++) {
        vc[offs+ii].x = x1 + rx - vc[last_arc_offs - ii].x;
        vc[offs+ii].y = y1 + ry - vc[last_arc_offs - ii].y;
        vc[offs+ii].color = color;
    }
    offs += num_segments;
}

static inline void init_rr_corner_hairline_tr(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x2, float y1, const lgui::Color& color)
{
    for(int ii = 0; ii < num_segments; ii++) {
        vc[offs+ii].x = x2 - rx + vc[arc_offs+ii].x;
        vc[offs+ii].y = y1 + ry - vc[arc_offs+ii].y;
        vc[offs+ii].color = color;
    }
    offs += num_segments;
}

static inline void init_rr_corner_hairline_bl(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x1, float y2, const lgui::Color& color)
{
    for(int ii = 0; ii < num_segments; ii++) {
        vc[offs+ii].x = x1 + rx - vc[arc_offs+ii].x;
        vc[offs+ii].y = y2 - ry + vc[arc_offs+ii].y;
        vc[offs+ii].color = color;
    }
    offs += num_segments;
}

static inline void init_rr_corner_hairline_br(lgui::PrimVertex* vc, int arc_offs, int num_segments, int& offs,
                                        float rx, float ry, float x2, float y2, const lgui::Color& color)
{
    const int last_arc_offs = arc_offs + num_segments -1;
    for(int ii = 0; ii < num_segments; ii++) {
        vc[offs + ii].x = x2 - rx + vc[last_arc_offs  - ii].x;
        vc[offs + ii].y = y2 - ry + vc[last_arc_offs  - ii].y;
        vc[offs+ii].color = color;
    }
    offs += num_segments;
}

namespace lgui {

void A5PrimHelper::rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color color, float thickness, int corners)
{
    ASSERT(rx >= 0);
    ASSERT(ry >= 0);

    if(corners == 0 || (rx == 0 || ry == 0)) {
        al_draw_rectangle(x1, y1, x2, y2, color, thickness);
        return;
    }
    else if(corners == 0xF) {
        al_draw_rounded_rectangle(x1, y1, x2, y2, rx, ry, color, thickness);
        return;
    }

    PrimVertex vertex_cache[PRIM_CACHE_SIZE];

    const float scale = 1.0;

    if(thickness > 0) {
        int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

        if (8 * num_segments + 2 >= PRIM_CACHE_SIZE) {
            num_segments = (PRIM_CACHE_SIZE - 3) / 8;
        }

        const int arc_offs = 6*num_segments+2;

        calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, thickness, num_segments);

        int offs = 0;

        float t = thickness / 2;

        int ii;
        // top-right
        if(corners & 1) {
            init_rr_corner_thick_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1);
        }
        else {
            vertex_cache[offs+1].x = x2 - t;
            vertex_cache[offs+1].y = y1 + t;
            vertex_cache[offs].x = x2 + t;
            vertex_cache[offs].y = y1 - t;
            offs+=2;
        }
        // top-left
        if(corners & 2) {
            init_rr_corner_thick_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1);
        }
        else {
            vertex_cache[offs].x = x1 - t;
            vertex_cache[offs].y = y1 - t;
            vertex_cache[offs+1].x = x1 + t;
            vertex_cache[offs+1].y = y1 + t;
            offs+=2;
        }
        // bottom-left

        if(corners & 4) {
            init_rr_corner_thick_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2);
        }
        else {
            vertex_cache[offs].x = x1 - t;
            vertex_cache[offs].y = y2 + t;
            vertex_cache[offs+1].x = x1 + t;
            vertex_cache[offs+1].y = y2 -t;
            offs+=2;
        }

        // bottom-right
        if(corners & 8) {
            init_rr_corner_thick_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2);
        }
        else {
            vertex_cache[offs].x = x2 + t;
            vertex_cache[offs].y = y2 + t;
            vertex_cache[offs+1].x = x2 - t;
            vertex_cache[offs+1].y = y2 - t;
            offs+=2;
        }

        vertex_cache[offs] = vertex_cache[0];
        vertex_cache[offs+1] = vertex_cache[1];
        offs+=2;

        for(ii = 0; ii < offs; ii++) {
            vertex_cache[ii].color = color;
        }

        draw_vertices(lgui::PrimType::PRIM_TRIANGLE_STRIP, vertex_cache, 0, offs);
    }
    else {
        int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

        if (num_segments * 4+1 >= PRIM_CACHE_SIZE) {
            num_segments = (PRIM_CACHE_SIZE - 1) / 4;
        }

        const int arc_offs = 3*num_segments+1;

        calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

        int offs=0;

        // top-right
        if(corners & 1) {
            init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, color);
        }
        else {
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs ++;
        }
        // top-left
        if(corners & 2) {
            init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, color);
        }
        else {
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs++;
        }
        // bottom-left
        if(corners & 4) {
            init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, color);
        }
        else {
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
        }
        // bottom-right
        if(corners & 8) {
            init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, color);
        }
        else {
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
        }
        draw_vertices(lgui::PrimType::PRIM_LINE_LOOP, vertex_cache, 0, offs);
    }
}

void A5PrimHelper::filled_rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color color, int corners)
{
    ASSERT(rx >= 0);
    ASSERT(ry >= 0);

    if(corners == 0 || (rx == 0 || ry == 0)) {
        al_draw_filled_rectangle(x1, y1, x2, y2, color);
        return;
    }
    else if(corners == 0xF) {
        al_draw_filled_rounded_rectangle(x1, y1, x2, y2, rx, ry, color);
        return;
    }

    PrimVertex vertex_cache[PRIM_CACHE_SIZE];

    const float scale = 1.0;

    int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

    if (num_segments * 4+1 >= PRIM_CACHE_SIZE) {
        num_segments = (PRIM_CACHE_SIZE - 1) / 4;
    }

    const int arc_offs = 3*num_segments+1;

    calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

    int offs=0;

    // top-right
    if(corners & 1) {
        init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs,
                                   rx, ry, x2, y1, color);
    }
    else {
        vertex_cache[offs].x = x2;
        vertex_cache[offs].y = y1;
        vertex_cache[offs].color = color;
        offs ++;
    }
    // top-left
    if(corners & 2) {
        init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs,
                                   rx, ry, x1, y1, color);
    }
    else {
        vertex_cache[offs].x = x1;
        vertex_cache[offs].y = y1;
        vertex_cache[offs].color = color;
        offs++;
    }
    // bottom-left

    if(corners & 4) {
        init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs,
                                   rx, ry, x1, y2, color);
    }
    else {
        vertex_cache[offs].x = x1;
        vertex_cache[offs].y = y2;
        vertex_cache[offs].color = color;
        offs++;
    }

    // bottom-right
    if(corners & 8) {
        init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs,
                                   rx, ry, x2, y2, color);
    }
    else {
        vertex_cache[offs].x = x2;
        vertex_cache[offs].y = y2;
        vertex_cache[offs].color = color;
        offs++;
    }

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vertex_cache, 0, offs);
}



void A5PrimHelper::rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color color,
                                      float thickness, lgui::OpenEdge oe)
{
    if(rx == 0 || ry == 0) {
        rect_bracket(x1, y1, x2, y2, color, thickness, oe);
        return;
    }

    ASSERT(rx >= 0);
    ASSERT(ry >= 0);

    PrimVertex vertex_cache[PRIM_CACHE_SIZE];

    const float scale = 1.0;

    if(thickness > 0) {
        int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

        if (5 * num_segments + 4 >= PRIM_CACHE_SIZE) {
            num_segments = (PRIM_CACHE_SIZE - 4) / 5;
        }

        const int arc_offs = 4*num_segments+3;

        calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, thickness, num_segments);

        int offs = 0;

        float t = thickness / 2;

        switch(oe) {
            case lgui::OpenEdge::OpenLeft:
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y2 + t;
                vertex_cache[offs+1].x = x1;
                vertex_cache[offs+1].y = y2 -t;
                offs+=2;
                init_rr_corner_thick_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2);
                init_rr_corner_thick_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1);
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y1 - t;
                vertex_cache[offs+1].x = x1;
                vertex_cache[offs+1].y = y1 + t;
                offs+=2;
                break;
            case lgui::OpenEdge::OpenTop:
                vertex_cache[offs].x = x1 - t;
                vertex_cache[offs].y = y1;
                vertex_cache[offs+1].x = x1 + t;
                vertex_cache[offs+1].y = y1;
                offs+=2;
                init_rr_corner_thick_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2);
                init_rr_corner_thick_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2);
                vertex_cache[offs+1].x = x2 - t;
                vertex_cache[offs+1].y = y1;
                vertex_cache[offs].x = x2 + t;
                vertex_cache[offs].y = y1;
                offs+=2;
                break;
            case lgui::OpenEdge::OpenRight:
                vertex_cache[offs+1].x = x2;
                vertex_cache[offs+1].y = y1 + t;
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y1 - t;
                offs+=2;
                init_rr_corner_thick_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1);
                init_rr_corner_thick_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2);
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y2 + t;
                vertex_cache[offs+1].x = x2;
                vertex_cache[offs+1].y = y2 - t;
                offs+=2;
                break;
            case lgui::OpenEdge::OpenBottom:
                vertex_cache[offs].x = x2 + t;
                vertex_cache[offs].y = y2;
                vertex_cache[offs+1].x = x2 - t;
                vertex_cache[offs+1].y = y2;
                offs+=2;
                init_rr_corner_thick_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1);
                init_rr_corner_thick_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1);
                vertex_cache[offs].x = x1 - t;
                vertex_cache[offs].y = y2;
                vertex_cache[offs+1].x = x1 + t;
                vertex_cache[offs+1].y = y2;
                offs+=2;
                break;
        }

        for(int ii = 0; ii < offs; ii++) {
            vertex_cache[ii].color = color;
        }

        draw_vertices(lgui::PrimType::PRIM_TRIANGLE_STRIP, vertex_cache, 0, offs);
    }
    else {
        int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

        if (num_segments * 2+2 >= PRIM_CACHE_SIZE) {
            num_segments = (PRIM_CACHE_SIZE - 2) / 2;
        }

        const int arc_offs = 2*num_segments+1;

        calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

        int offs=0;

        switch(oe) {
            case lgui::OpenEdge::OpenLeft:
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y2;
                vertex_cache[offs].color = color;
                offs++;
                init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, color);
                init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, color);
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y1;
                vertex_cache[offs].color = color;
                offs++;
                break;
            case lgui::OpenEdge::OpenTop:
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y1;
                vertex_cache[offs].color = color;
                offs++;
                init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, color);
                init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, color);
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y1;
                vertex_cache[offs].color = color;
                offs++;
                break;
            case lgui::OpenEdge::OpenRight:
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y1;
                vertex_cache[offs].color = color;
                offs++;
                init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, color);
                init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, color);
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y2;
                vertex_cache[offs].color = color;
                offs++;
                break;
            case lgui::OpenEdge::OpenBottom:
                vertex_cache[offs].x = x2;
                vertex_cache[offs].y = y2;
                vertex_cache[offs].color = color;
                offs++;
                init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, color);
                init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, color);
                vertex_cache[offs].x = x1;
                vertex_cache[offs].y = y2;
                vertex_cache[offs].color = color;
                offs++;
                break;
        }
        draw_vertices(lgui::PrimType::PRIM_LINE_STRIP, vertex_cache, 0, offs);
   }
}

void A5PrimHelper::rect_bracket(float x1, float y1, float x2, float y2, lgui::Color color, float thickness,
                              lgui::OpenEdge oe)
{

    if(thickness > 0) {
        PrimVertex vtx[8];

        float t = thickness / 2;

        switch(oe) {
            case lgui::OpenEdge::OpenLeft:
                vtx[0].x = x1;     vtx[0].y = y1 - t;
                vtx[1].x = x1;     vtx[1].y = y1 + t;
                vtx[2].x = x2 + t; vtx[2].y = y1 - t;
                vtx[3].x = x2 - t; vtx[3].y = y1 + t;
                vtx[4].x = x2 + t; vtx[4].y = y2 + t;
                vtx[5].x = x2 - t; vtx[5].y = y2 - t;
                vtx[6].x = x1;     vtx[6].y = y2 + t;
                vtx[7].x = x1;     vtx[7].y = y2 - t;
                break;
            case lgui::OpenEdge::OpenTop:
                vtx[0].x = x2 + t; vtx[0].y = y1 ;
                vtx[1].x = x2 - t; vtx[1].y = y1 ;
                vtx[2].x = x2 + t; vtx[2].y = y2 + t;
                vtx[3].x = x2 - t; vtx[3].y = y2 - t;
                vtx[4].x = x1 - t; vtx[4].y = y2 + t;
                vtx[5].x = x1 + t; vtx[5].y = y2 - t;
                vtx[6].x = x1 - t; vtx[6].y = y1;
                vtx[7].x = x1 + t; vtx[7].y = y1;
                break;
            case lgui::OpenEdge::OpenRight:
                vtx[0].x = x2;     vtx[0].y = y2 + t;
                vtx[1].x = x2;     vtx[1].y = y2 - t;
                vtx[2].x = x1 - t; vtx[2].y = y2 + t;
                vtx[3].x = x1 + t; vtx[3].y = y2 - t;
                vtx[4].x = x1 - t; vtx[4].y = y1 - t;
                vtx[5].x = x1 + t; vtx[5].y = y1 + t;
                vtx[6].x = x2;     vtx[6].y = y1 - t;
                vtx[7].x = x2;     vtx[7].y = y1 + t;
                break;
            case lgui::OpenEdge::OpenBottom:
                vtx[0].x = x1 - t; vtx[0].y = y2;
                vtx[1].x = x1 + t; vtx[1].y = y2;
                vtx[2].x = x1 - t; vtx[2].y = y1 - t;
                vtx[3].x = x1 + t; vtx[3].y = y1 + t;
                vtx[4].x = x2 + t; vtx[4].y = y1 - t;
                vtx[5].x = x2 - t; vtx[5].y = y1 + t;
                vtx[6].x = x2 + t; vtx[6].y = y2;
                vtx[7].x = x2 - t; vtx[7].y = y2;
                break;
        }

        for (auto& ii : vtx) {
            ii.color = color;
        }

        draw_vertices(lgui::PrimType::PRIM_TRIANGLE_STRIP, vtx, 0, 8);
    }
    else {
        PrimVertex vtx[4];

        switch(oe) {
            case lgui::OpenEdge::OpenLeft:
                vtx[0].x = x1;     vtx[0].y = y2;
                vtx[1].x = x2;     vtx[1].y = y2;
                vtx[2].x = x2;     vtx[2].y = y1;
                vtx[3].x = x1;     vtx[3].y = y1;
                break;
            case lgui::OpenEdge::OpenTop:
                vtx[0].x = x1;     vtx[0].y = y1;
                vtx[1].x = x1;     vtx[1].y = y2;
                vtx[2].x = x2;     vtx[2].y = y2;
                vtx[3].x = x2;     vtx[3].y = y1;
                break;
            case lgui::OpenEdge::OpenRight:
                vtx[0].x = x2;     vtx[0].y = y2;
                vtx[1].x = x1;     vtx[1].y = y2;
                vtx[2].x = x1;     vtx[2].y = y1;
                vtx[3].x = x2;     vtx[3].y = y1;
                break;
            case lgui::OpenEdge::OpenBottom:
                vtx[0].x = x2;     vtx[0].y = y2;
                vtx[1].x = x2;     vtx[1].y = y1;
                vtx[2].x = x1;     vtx[2].y = y1;
                vtx[3].x = x1;     vtx[3].y = y2;
                break;
        }

        for(int ii = 0; ii < 4; ii++) {
            vtx[ii].color = color;
        }
        draw_vertices(lgui::PrimType::PRIM_LINE_STRIP, vtx, 0, 4);
    }
}

void A5PrimHelper::filled_rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry,
                                             lgui::Color color, lgui::OpenEdge oe)
{
    if(rx == 0 || ry == 0) {
        filled_rect(x1, y1, x2, y2, color);
        return;
    }

    ASSERT(rx >= 0);
    ASSERT(ry >= 0);

    PrimVertex vertex_cache[PRIM_CACHE_SIZE];

    const float scale = 1.0;

    int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

    if (num_segments * 2+2 >= PRIM_CACHE_SIZE) {
        num_segments = (PRIM_CACHE_SIZE - 2) / 2;
    }

    const int arc_offs = 2*num_segments+1;

    calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

    int offs=0;

    switch(oe) {
        case lgui::OpenEdge::OpenLeft:
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
            init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, color);
            init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, color);
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs++;
            break;
        case lgui::OpenEdge::OpenTop:
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs++;
            init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, color);
            init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, color);
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs++;
            break;
        case lgui::OpenEdge::OpenRight:
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = color;
            offs++;
            init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, color);
            init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, color);
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
            break;
        case lgui::OpenEdge::OpenBottom:
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
            init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, color);
            init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, color);
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = color;
            offs++;
            break;
    }

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vertex_cache, 0, offs);
}


void A5PrimHelper::filled_rounded_rect_bracket_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                                      const lgui::Color& col1, const lgui::Color& col2, lgui::OpenEdge oe,
                                                      lgui::GradientDirection dir)
{
    if(rx == 0 || ry == 0) {
        filled_rect_gradient(x1, y1, x2, y2, col1, col2, dir);
        return;
    }

    ASSERT(rx >= 0);
    ASSERT(ry >= 0);

    PrimVertex vertex_cache[PRIM_CACHE_SIZE];

    const float scale = 1.0;

    int num_segments = PRIM_QUALITY * scale * sqrtf((rx + ry) / 2.0f) / 4;

    if (num_segments * 2+4 >= PRIM_CACHE_SIZE) {
        num_segments = (PRIM_CACHE_SIZE - 4) / 2;
    }

    const int arc_offs = 2*num_segments+2;

    calc_arc(&(vertex_cache[arc_offs].x), sizeof(PrimVertex), 0, 0, rx, ry, 0, ALLEGRO_PI / 2, 0, num_segments);

    int offs=0;

    const lgui::Color* tl_col;
    const lgui::Color* br_col;
    const lgui::Color* tr_col;
    const lgui::Color* bl_col;
    assign_gradient_colors(&col1, &col2, dir, &tl_col, &tr_col, &bl_col, &br_col);

    // center point
    vertex_cache[0].x = (x1 + x2) / 2.0;
    vertex_cache[0].y = (y1 + y2) / 2.0;
    vertex_cache[0].color = lgui::rgba((col1.r+col2.r) / 2.0,
                                       (col1.g+col2.g) / 2.0,
                                       (col1.b+col2.b) / 2.0,
                                       (col1.a+col2.a) / 2.0);
    offs++;

    switch(oe) {
        case lgui::OpenEdge::OpenLeft:
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = *bl_col;
            offs++;
            init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, *br_col);
            init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, *tr_col);
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = *tl_col;
            offs++;
            break;
        case lgui::OpenEdge::OpenTop:
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = *tl_col;
            offs++;
            init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, *bl_col);
            init_rr_corner_hairline_br(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y2, *br_col);
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = *tr_col;
            offs++;
            break;
        case lgui::OpenEdge::OpenRight:
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y1;
            vertex_cache[offs].color = *tr_col;
            offs++;
            init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, *tl_col);
            init_rr_corner_hairline_bl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y2, *bl_col);
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = *br_col;
            offs++;
            break;
        case lgui::OpenEdge::OpenBottom:
            vertex_cache[offs].x = x2;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = *br_col;
            offs++;
            init_rr_corner_hairline_tr(vertex_cache, arc_offs, num_segments, offs, rx, ry, x2, y1, *tr_col);
            init_rr_corner_hairline_tl(vertex_cache, arc_offs, num_segments, offs, rx, ry, x1, y1, *tl_col);
            vertex_cache[offs].x = x1;
            vertex_cache[offs].y = y2;
            vertex_cache[offs].color = *bl_col;
            offs++;
            break;
    }

    vertex_cache[offs++] = vertex_cache[1]; // close fan

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vertex_cache, 0, offs);
}

void A5PrimHelper::draw_visible_pixel(float px, float py, lgui::Color col)
{
    al_draw_line(px - 3, py,     px + 3, py,     col,  0);
    al_draw_line(px,     py - 3, px,     py + 3, col,  0);
}

void A5PrimHelper::draw_filled_pieslice(float cx, float cy, float r, float start_theta, float delta_theta, ALLEGRO_COLOR color)
{
    PrimVertex vertex_cache[PRIM_CACHE_SIZE];
    int num_segments, ii;

    num_segments = fabs(delta_theta / (2 * ALLEGRO_PI) * PRIM_QUALITY * sqrtf(r));

    if (num_segments < 2)
        return;

    if (num_segments >= PRIM_CACHE_SIZE) {
        num_segments = PRIM_CACHE_SIZE - 1;
    }


    calc_arc(&(vertex_cache[1].x), sizeof(PrimVertex), cx, cy,
            r, r, start_theta, delta_theta, 0, num_segments);
    vertex_cache[0].x = cx; vertex_cache[0].y = cy;

    for (ii = 0; ii < num_segments+1; ii++) {
        vertex_cache[ii].color = color;
    }

    draw_vertices(lgui::PrimType::PRIM_TRIANGLE_FAN, vertex_cache, 0, num_segments+1);
}

void A5PrimHelper::draw_vertices_indexed(lgui::PrimType type, const std::vector<PrimVertex>& verts,
                                         const std::vector<int>& indices, unsigned int n) const
{
    ASSERT(n <= indices.size());
    al_draw_indexed_prim(verts.data(), mprim_vertex_decl, nullptr, indices.data(), n, static_cast<int>(type));
}

void A5PrimHelper::draw_vertices(lgui::PrimType type, const std::vector<PrimVertex>& verts, unsigned int start,
                                 unsigned int end) const
{
    ASSERT(start <= verts.size() && end <= verts.size());
    al_draw_prim(verts.data(), mprim_vertex_decl, nullptr, start, end, static_cast<int>(type));
}

void A5PrimHelper::draw_vertices(lgui::PrimType type, const PrimVertex* first, unsigned int start,
                                 unsigned int end) const
{
    al_draw_prim(first, mprim_vertex_decl, nullptr, start, end, static_cast<int>(type));
}

void A5PrimHelper::calc_pie_slice(std::vector<PrimVertex>& dest, float cx, float cy, float rx, float ry,
                                float start_theta, float delta_theta, unsigned int num_segments)
{
    dest.resize(num_segments+1);
    calc_arc(&(dest[1].x), sizeof(PrimVertex), cx, cy,
                     rx, ry, start_theta * M_PI / 180.0, delta_theta * M_PI / 180.0, 0,
                     num_segments);
    // make pie slice
    dest[0].x = cx;
    dest[0].y = cy;
    // should be drawn with PRIM_TRIANGLE_FAN
}

void A5PrimHelper::calc_arc(float* dest, int stride, float cx, float cy, float rx, float ry, float start_theta, float delta_theta, float thickness, int num_points)
{
    al_calculate_arc(dest, stride, cx, cy, rx, ry, start_theta, delta_theta, thickness, num_points);
}

}
