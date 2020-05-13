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

#ifndef LGUI_GRAPHICS_H
#define LGUI_GRAPHICS_H

#include "a5/a5graphics.h"
namespace lgui {
using GraphicsImplementation = A5Graphics;
}


#include "primhelper.h"
#include "ninepatch.h"
#include <stack>

namespace lgui {

/** The graphics context object used to draw things. Provides methods to draw things and manages a stack of
 *  clipping rectangles. */
class Graphics : public GraphicsImplementation {
    public:
        Graphics();
        ~Graphics() = default;

        void push_draw_area(int offsx, int offsy, int w, int h, bool clip=false);
        void push_draw_area(const Rect& r, bool clip=false);
        void pop_draw_area();

        void draw_ninepatch(const NinePatch& np, const Position& pos,
                             const Size& content_size) const;
        void draw_ninepatch(const NinePatch& np, int dx, int dy, const Size& content_size) const;
        void draw_ninepatch(const NinePatch& np, int dx, int dy, int content_w, int content_h) const;
        void draw_ninepatch_tinted(const NinePatch& np, const Color& col,
                                    int dx, int dy, int content_w, int content_h) const;
        void draw_ninepatch_tinted(const NinePatch& np, const Color& col,
                                    int dx, int dy, const Size& content_size) const;
        void draw_ninepatch_tinted(const NinePatch& np, const Color& col,
                                    const Position& pos, const Size& content_size) const;

        void draw_ninepatch_outer_size(const NinePatch& np, const Position& pos,
                             const Size& total_size) const;
        void draw_ninepatch_outer_size(const NinePatch& np, int dx, int dy, const Size& total_size) const;
        void draw_tinted_ninepatch_outer_size(const NinePatch& np, const Color& col, const Position& pos,
                             const Size& total_size) const;
        void draw_tinted_ninepatch_outer_size(const NinePatch& np, const Color& col,
                                               int dx, int dy, const Size& total_size) const;


        void draw_vertices(PrimType type, const PrimVertex* first, unsigned int start, unsigned int end) const;
        void draw_vertices(PrimType type, const std::vector <PrimVertex>& verts, unsigned int start=0,
                           unsigned int end=0) const;
        void draw_vertices_indexed(PrimType type, const std::vector <PrimVertex>& verts,
                                   const std::vector <int>& indices, unsigned int n) const;

        void rect(float x1, float y1, float x2, float y2, Color col, float thickness);
        void rect(const Rect& r, Color col, float thickness);
        void filled_rect(float x1, float y1, float x2, float y2, Color col);
        void filled_rect(const Rect& r, Color col);
        void filled_triangle(Point v1, Point v2, Point v3, Color col);
        void filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, Color col);
        void fill_draw_area(Color col);
        void fill_draw_area_black(float alpha);

        void rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, Color col, float thickness);
        void rounded_rect(const Rect& r, float rx, float ry, Color col, float thickness);
        void filled_rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry,  Color col);
        void filled_rounded_rect(const Rect& r, float rx, float ry,  Color col);
        void filled_rounded_rect_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                          const Color& col1, const Color& col2, GradientDirection dir);
        void filled_rounded_rect_gradient(const Rect& r, float rx, float ry,
                                          const Color& col1, const Color& col2, GradientDirection dir);
        void filled_rect_gradient(float x1, float y1, float x2, float y2, const Color& col1, const Color& col2,
                                  GradientDirection dir);
        void filled_rect_gradient(const Rect& r, const Color& col1, const Color& col2,
                                  GradientDirection dir);

        void rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry, Color col,
                                       float thickness, int corners);
        void rounded_rect_spec_corners(const Rect& r, float rx, float ry, Color col, float thickness,
                                       int corners);
        void filled_rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry,
                                              Color color, int corners);
        void filled_rounded_rect_spec_corners(const Rect& r, float rx, float ry, Color color, int corners);

        void rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry, Color color,
                                  float thickness, OpenEdge oe);
        void rounded_rect_bracket(const Rect& r, float rx, float ry, Color color, float thickness,
                                  OpenEdge oe);
        void rect_bracket(float x1, float y1, float x2, float y2, Color color, float thickness,
                          OpenEdge oe);
        void rect_bracket(const Rect& r, Color color, float thickness, OpenEdge oe);

        void filled_rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry,
                                         Color color, OpenEdge oe);
        void filled_rounded_rect_bracket(const Rect& r, float rx, float ry, Color color, OpenEdge oe);
        void filled_rounded_rect_bracket_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                                  const Color& col1, const Color& col2, OpenEdge oe,
                                                  GradientDirection dir);
        void filled_rounded_rect_bracket_gradient(const Rect& r, float rx, float ry,
                                                  const Color& col1, const Color& col2, OpenEdge oe,
                                                  GradientDirection dir);


        void circle(float cx, float cy, float r, Color col, float thickness);
        void filled_circle(float cx, float cy, float r, Color col);
        void line(float x1, float y1, float x2, float y2, Color col, float thickness);
        void line_p05(float x1, float y1, float x2, float y2, Color col, float thickness);
        void draw_visible_pixel(float px, float py, Color col);
        void draw_filled_pieslice(float cx, float cy, float r, float start_theta,
                                  float delta_theta, Color color);

    private:
        PrimHelper mprim_helper;
        std::stack <std::pair <Rect, bool> > mdraw_areas;
        std::stack <Rect> mclip_rects;
        bool mclip;

};
}

#endif // LGUI_GRAPHICS_H

