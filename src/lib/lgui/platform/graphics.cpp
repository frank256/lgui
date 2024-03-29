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

#include "graphics.h"
#include "error.h"

namespace lgui {

Graphics::Graphics()
        : mclip(false) {
    mtransform.set_identity();
}

void Graphics::push_draw_area(const lgui::Rect& r, bool clip) {
    push_draw_area(r.x(), r.y(), r.w(), r.h(), clip);
}

void Graphics::push_draw_area(const Rect& r, const Transform& transform, bool clip) {
    push_draw_area(r.x(), r.y(), r.w(), r.h(), transform, clip);
}

void Graphics::push_draw_area(int offsx, int offsy, int w, int h, bool clip) {
    mdraw_areas.push(DrawAreaStackEntry{mtransform, lgui::Rect(moffsx, moffsy, mw, mh), mclip});
    if (clip) {
        update_clip_rect(offsx, offsy, w, h);
    }
    moffsx += offsx;
    moffsy += offsy;
    mw = w;
    mh = h;
    mclip = clip;

    mtransform.translate_pre(PointF(offsx, offsy));
    use_transform(mtransform);
}

void Graphics::push_draw_area(int offsx, int offsy, int w, int h, const Transform& transform, bool clip) {
    mdraw_areas.push(DrawAreaStackEntry{mtransform, lgui::Rect(moffsx, moffsy, mw, mh), mclip});
    if (clip) {
        update_clip_rect(offsx, offsy, w, h);
    }
    moffsx += offsx;
    moffsy += offsy;
    mw = w;
    mh = h;
    mclip = clip;

    Transform t(transform);
    t.translate_post(PointF(offsx, offsy));
    mtransform.compose_pre(t);
    use_transform(mtransform);
}

void Graphics::pop_draw_area() {
    ASSERT(!mdraw_areas.empty());
    const auto& last = mdraw_areas.top();
    if (mclip) {
        ASSERT(!mclip_rects.empty());
        const auto& lcr = mclip_rects.top();
        set_clip_rect(lcr.x(), lcr.y(), lcr.w(), lcr.h());
        mclip_rects.pop();
    }
    mtransform = last.transform;
    use_transform(mtransform);
    moffsx = last.rect.x();
    moffsy = last.rect.y();
    mw = last.rect.w();
    mh = last.rect.h();
    mclip = last.is_clipped;
    mdraw_areas.pop();
}

void Graphics::update_clip_rect(int offsx, int offsy, int& w, int& h) {
    int cx, cy, cw, ch;
    get_clip_rect(cx, cy, cw, ch);
    mclip_rects.push(Rect(cx, cy, cw, ch));
    int ncx = moffsx + offsx;
    int ncy = moffsy + offsy;

    // clip against old rect:
    if (ncx < cx) {
        w -= cx - ncx;
        ncx = cx;
    }
    if (ncy < cy) {
        h -= cy - ncy;
        ncy = cy;
    }
    if (ncx + w > cx + cw) {
        w = cx + cw - ncx;
    }
    if (ncy + h > cy + ch) {
        h = cy + ch - ncy;
    }
    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    set_clip_rect(ncx, ncy, w, h);
}

void Graphics::draw_ninepatch(const lgui::NinePatch& np, const lgui::Position& pos,
                              const lgui::Size& content_size) const {
    np.draw_tinted(lgui::rgb(1.0, 1.0, 1.0), pos.x(), pos.y(), content_size);
}

void Graphics::draw_ninepatch(const lgui::NinePatch& np, int dx, int dy, const lgui::Size& content_size) const {
    np.draw_tinted(lgui::rgb(1.0, 1.0, 1.0), dx, dy, content_size);
}

void Graphics::draw_ninepatch(const lgui::NinePatch& np, int dx, int dy, int content_w, int content_h) const {
    np.draw_tinted(lgui::rgb(1.0, 1.0, 1.0), dx, dy, lgui::Size(content_w, content_h));
}

void Graphics::draw_ninepatch_tinted(const lgui::NinePatch& np, const lgui::Color& col, int dx, int dy,
                                     int content_w, int content_h) const {
    np.draw_tinted(col, dx, dy, lgui::Size(content_w, content_h));
}

void Graphics::draw_ninepatch_tinted(const lgui::NinePatch& np, const lgui::Color& col, int dx, int dy,
                                     const lgui::Size& content_size) const {
    np.draw_tinted(col, dx, dy, content_size);
}

void Graphics::draw_ninepatch_tinted(const lgui::NinePatch& np, const lgui::Color& col, const lgui::Position& pos,
                                     const lgui::Size& content_size) const {
    np.draw_tinted(col, pos.x(), pos.y(), content_size);
}

void Graphics::draw_ninepatch_outer_size(const lgui::NinePatch& np, const lgui::Position& pos,
                                         const lgui::Size& total_size) const {
    lgui::Size cs = np.content_for_total_size(total_size);
    np.draw_tinted(lgui::rgb(1.0, 1.0, 1.0), pos.x(), pos.y(), cs);
}

void Graphics::draw_ninepatch_outer_size(const lgui::NinePatch& np, int dx, int dy,
                                         const lgui::Size& total_size) const {
    lgui::Size cs = np.content_for_total_size(total_size);
    np.draw_tinted(lgui::rgb(1.0, 1.0, 1.0), dx, dy, cs);
}

void Graphics::draw_tinted_ninepatch_outer_size(const lgui::NinePatch& np, const lgui::Color& col,
                                                const lgui::Position& pos, const lgui::Size& total_size) const {
    lgui::Size cs = np.content_for_total_size(total_size);
    np.draw_tinted(col, pos.x(), pos.y(), cs);
}

void Graphics::draw_tinted_ninepatch_outer_size(const lgui::NinePatch& np, const lgui::Color& col, int dx, int dy,
                                                const lgui::Size& total_size) const {
    lgui::Size cs = np.content_for_total_size(total_size);
    np.draw_tinted(col, dx, dy, cs);
}


void Graphics::rect(float x1, float y1, float x2, float y2, lgui::Color col, float thickness) {
    mprim_helper.rect(x1, y1, x2, y2, col, thickness);
}

void Graphics::rect(const lgui::Rect& r, lgui::Color col, float thickness) {
    // we add 0.5 for a width of one
    mprim_helper.rect(r.x1() + 0.5, r.y1() + 0.5, r.x2() + 0.5, r.y2() + 0.5,
                      col, thickness);
}


void Graphics::filled_rect(float x1, float y1, float x2, float y2, lgui::Color col) {
    mprim_helper.filled_rect(x1, y1, x2, y2, col);
}

void Graphics::filled_rect(const lgui::Rect& r, lgui::Color col) {
    mprim_helper.filled_rect(r.x1(), r.y1(), r.x2() + 1, r.y2() + 1, col);
}

void Graphics::filled_triangle(lgui::Point v1, lgui::Point v2, lgui::Point v3, lgui::Color col) {
    mprim_helper.filled_triangle(v1.x(), v1.y(),
                                 v2.x(), v2.y(),
                                 v3.x(), v3.y(), col);
}

void Graphics::filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, lgui::Color col) {
    mprim_helper.filled_triangle(x1, y1, x2, y2, x3, y3, col);
}

void Graphics::fill_draw_area(lgui::Color col) {
    mprim_helper.filled_rect(0, 0, mw, mh, col);
}

void Graphics::fill_draw_area_black(float alpha) {
    mprim_helper.filled_rect(0, 0, mw, mh, lgui::rgba(0, 0, 0, alpha));
}


void Graphics::rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col,
                            float thickness) {
    mprim_helper.rounded_rect(x1, y1, x2, y2,
                              rx, ry, col, thickness);
}

void Graphics::rounded_rect(const lgui::Rect& r, float rx, float ry, lgui::Color col, float thickness) {
    // for a 1px line
    mprim_helper.rounded_rect(r.x1() + 0.5, r.y1() + 0.5, r.x2() + 0.5,
                              r.y2() + 0.5,
                              rx, ry, col, thickness);
}

void Graphics::filled_rounded_rect(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color col) {
    mprim_helper.filled_rounded_rect(x1, y1, x2, y2, rx, ry, col);
}

void Graphics::filled_rounded_rect(const lgui::Rect& r, float rx, float ry, lgui::Color col) {
    mprim_helper.filled_rounded_rect(r.x1(), r.y1(), r.x() + r.w(), r.y() + r.h(),
                                     rx, ry, col);
}

void Graphics::circle(float cx, float cy, float r, lgui::Color col, float thickness) {
    mprim_helper.circle(cx, cy, r, col, thickness);
}

void Graphics::filled_circle(float cx, float cy, float r, lgui::Color col) {
    mprim_helper.filled_circle(cx, cy, r, col);
}

void Graphics::line(float x1, float y1, float x2, float y2, lgui::Color col, float thickness) {
    mprim_helper.line(x1, y1, x2, y2, col, thickness);
}

void Graphics::line_p05(float x1, float y1, float x2, float y2, lgui::Color col, float thickness) {
    mprim_helper.line(x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5, col, thickness);
}

void Graphics::draw_visible_pixel(float px, float py, lgui::Color col) {
    mprim_helper.draw_visible_pixel(px, py, col);
}

void Graphics::draw_filled_pieslice(float cx, float cy, float r, float start_theta, float delta_theta,
                                    lgui::Color color) {
    mprim_helper.draw_filled_pieslice(cx, cy, r, start_theta, delta_theta, color);
}

void Graphics::draw_vertices(lgui::PrimType type, const PrimVertex* first, unsigned int start,
                             unsigned int end) const {
    mprim_helper.draw_vertices(type, first, start, end);
}

void Graphics::draw_vertices(lgui::PrimType type, const std::vector<PrimVertex>& verts, unsigned int start,
                             unsigned int end) const {
    mprim_helper.draw_vertices(type, verts, start, end);
}

void Graphics::draw_vertices_indexed(lgui::PrimType type, const std::vector<PrimVertex>& verts,
                                     const std::vector<int>& indices, unsigned int n) const {
    mprim_helper.draw_vertices_indexed(type, verts, indices, n);
}

void Graphics::filled_rounded_rect_gradient(const lgui::Rect& r, float rx, float ry, const lgui::Color& col1,
                                            const lgui::Color& col2, lgui::GradientDirection dir) {
    mprim_helper.filled_rounded_rect_gradient(r.x1(), r.y1(),
                                              r.x() + r.w(), r.y() + r.h(), rx, ry, col1, col2, dir);
}

void Graphics::filled_rounded_rect_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                            const lgui::Color& col1, const lgui::Color& col2,
                                            lgui::GradientDirection dir) {
    mprim_helper.filled_rounded_rect_gradient(x1, y1,
                                              x2, y2, rx, ry, col1, col2, dir);
}

void Graphics::filled_rect_gradient(const lgui::Rect& r, const lgui::Color& col1, const lgui::Color& col2,
                                    lgui::GradientDirection dir) {
    mprim_helper.filled_rect_gradient(r.x(), r.y(),
                                      r.x() + r.w(), r.y() + r.h(),
                                      col1, col2, dir);
}

void Graphics::filled_rect_gradient(float x1, float y1, float x2, float y2,
                                    const lgui::Color& col1, const lgui::Color& col2,
                                    lgui::GradientDirection dir) {
    mprim_helper.filled_rect_gradient(x1, y1,
                                      x2, y2,
                                      col1, col2, dir);
}

void Graphics::rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry,
                                         lgui::Color color, float thickness, int corners) {
    mprim_helper.rounded_rect_spec_corners(x1, y1,
                                           x2, y2,
                                           rx, ry, color, thickness, corners);
}

void Graphics::rounded_rect_spec_corners(const lgui::Rect& r, float rx, float ry, lgui::Color color,
                                         float thickness, int corners) {
    // we add 0.5 for a width of one
    mprim_helper.rounded_rect_spec_corners(r.x() + 0.5, r.y() + 0.5,
                                           r.x2() + 0.5, r.y2() + 0.5,
                                           rx, ry, color, thickness, corners);
}

void Graphics::filled_rounded_rect_spec_corners(float x1, float y1, float x2, float y2, float rx, float ry,
                                                lgui::Color color, int corners) {
    mprim_helper.filled_rounded_rect_spec_corners(x1, y1,
                                                  x2, y2,
                                                  rx, ry, color, corners);
}

void Graphics::filled_rounded_rect_spec_corners(const lgui::Rect& r, float rx, float ry, lgui::Color color,
                                                int corners) {
    mprim_helper.filled_rounded_rect_spec_corners(r.x(), r.y(),
                                                  r.x() + r.w(), r.y() + r.h(),
                                                  rx, ry, color, corners);
}


void Graphics::rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry, lgui::Color color,
                                    float thickness, lgui::OpenEdge oe) {
    mprim_helper.rounded_rect_bracket(x1, y1,
                                      x2, y2,
                                      rx, ry, color, thickness, oe);

}

void Graphics::rounded_rect_bracket(const lgui::Rect& r, float rx, float ry, lgui::Color color, float thickness,
                                    lgui::OpenEdge oe) {
    // we add 0.5 for a width of one
    mprim_helper.rounded_rect_bracket(r.x1() + 0.5, r.y1() + 0.5,
                                      r.x2() + 0.5, r.y2() + 0.5,
                                      rx, ry, color, thickness, oe);
}


void Graphics::rect_bracket(float x1, float y1, float x2, float y2, lgui::Color color, float thickness,
                            lgui::OpenEdge oe) {
    mprim_helper.rect_bracket(x1, y1,
                              x2, y2,
                              color, thickness, oe);
}

void Graphics::rect_bracket(const lgui::Rect& r, lgui::Color color, float thickness, lgui::OpenEdge oe) {
    // we add 0.5 for a width of one
    mprim_helper.rect_bracket(r.x1() + 0.5, r.y1() + 0.5,
                              r.x2() + 0.5, r.y2() + 0.5,
                              color, thickness, oe);
}

void Graphics::filled_rounded_rect_bracket(float x1, float y1, float x2, float y2, float rx, float ry,
                                           lgui::Color color, lgui::OpenEdge oe) {
    mprim_helper.filled_rounded_rect_bracket(x1, y1,
                                             x2, y2,
                                             rx, ry, color, oe);
}

void Graphics::filled_rounded_rect_bracket(const lgui::Rect& r, float rx, float ry, lgui::Color color,
                                           lgui::OpenEdge oe) {
    mprim_helper.filled_rounded_rect_bracket(r.x(), r.y(),
                                             r.x() + r.w(), r.y() + r.h(),
                                             rx, ry, color, oe);
}


void Graphics::filled_rounded_rect_bracket_gradient(float x1, float y1, float x2, float y2, float rx, float ry,
                                                    const lgui::Color& col1, const lgui::Color& col2,
                                                    lgui::OpenEdge oe, lgui::GradientDirection dir) {
    mprim_helper.filled_rounded_rect_bracket_gradient(x1, y1,
                                                      x2, y2,
                                                      rx, ry, col1, col2, oe, dir);
}

void Graphics::filled_rounded_rect_bracket_gradient(const lgui::Rect& r, float rx, float ry,
                                                    const lgui::Color& col1, const lgui::Color& col2,
                                                    lgui::OpenEdge oe, lgui::GradientDirection dir) {
    mprim_helper.filled_rounded_rect_bracket_gradient(r.x(), r.y(),
                                                      r.x() + r.w(), r.y() + r.h(),
                                                      rx, ry, col1, col2, oe, dir);
}

}
