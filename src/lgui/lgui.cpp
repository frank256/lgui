/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#include "lgui.h"
#include <algorithm>

void lgui::Rect::clip_to(const lgui::Rect& frame)
{
    if(x() < frame.x()) {
        int dx = frame.x()-x();
        set_pos_x(frame.x());
        set_width(w()-dx);
    }
    if(y() < frame.y()) {
        int dy = frame.y()-y();
        set_pos_y(frame.y());
        set_height(h()-dy);
    }
    if(x2() > frame.x2())
        set_width(frame.x2()-x()+1);

    if(y2() > frame.y2())
        set_height(frame.y2()-y()+1);
}

void lgui::Rect::clip_point(lgui::Point& p) const
{
    if(p.x() < x1())
        p.set_x(x1());
    else if(p.x() > x2())
        p.set_x(x2());
    if(p.y() < y1())
        p.set_y(y1());
    else if(p.y() > y2())
        p.set_y(y2());
}

lgui::MeasureResult lgui::force_size_constraint(int size, lgui::SizeConstraint sc)
{
    if (sc.mode() == SizeConstraintMode::Exactly)
        return MeasureResult(sc.value(), size > sc.value());
    else if (sc.mode() == SizeConstraintMode::Maximum) {
        if (size <= sc.value())
            return MeasureResult(size, false);
        else
            return MeasureResult(sc.value(), true);
    }
    else {
        return MeasureResult(size, false);
    }
}

lgui::MeasureResult lgui::force_size_constraint(int size, lgui::SizeConstraint sc, bool too_small_acc)
{
    if (sc.mode() == SizeConstraintMode::Exactly)
        return MeasureResult(sc.value(), (size > sc.value()) || too_small_acc);
    else if (sc.mode() == SizeConstraintMode::Maximum) {
        if (size <= sc.value())
            return MeasureResult(size, too_small_acc);
        else
            return MeasureResult(sc.value(), true);
    }
    else {
        return MeasureResult(size, too_small_acc);
    }
}

lgui::MeasureResult lgui::force_size_constraint(lgui::MeasureResult r, lgui::SizeConstraint sc)
{
    return force_size_constraint(r.value(), sc, r.is_too_small());
}

lgui::MeasureResults lgui::force_size_constraints(lgui::Size s, lgui::SizeConstraint wc, lgui::SizeConstraint hc) {
    return MeasureResults(force_size_constraint(s.w(), wc),
                          force_size_constraint(s.h(), hc));
}

lgui::MeasureResults lgui::force_size_constraints(lgui::Size s, lgui::SizeConstraint wc, lgui::SizeConstraint hc,
                                                TooSmallAccumulator accum)
{
    return MeasureResults(force_size_constraint(s.w(), wc, accum.width_too_small()),
                          force_size_constraint(s.h(), hc, accum.height_too_small()));
}

lgui::MeasureResults lgui::force_size_constraints(lgui::MeasureResults mr, lgui::SizeConstraint wc, lgui::SizeConstraint hc)
{
    return MeasureResults(force_size_constraint(mr.w(), wc, mr.is_width_too_small()),
                          force_size_constraint(mr.h(), hc, mr.is_height_too_small()));
}


lgui::SizeConstraint lgui::SizeConstraint::adapted_for_child(int space_used) const
{
    if(mode() == SizeConstraintMode::NoLimits)
        return lgui::SizeConstraint(0, SizeConstraintMode::NoLimits);

    int space = std::max(value() - space_used, 0);
    return lgui::SizeConstraint(space, SizeConstraintMode::Maximum);
}

lgui::Position lgui::do_alignment(const lgui::Rect& space, const lgui::Size& size, Align align)
{
    int rx = space.w() - size.w(), ry = space.h() - size.h();
    int px = space.x(), py = space.y();

    if(align.horz() == Align::HCenter)
        px += rx / 2;
    else if(align.horz() == Align::Right)
        px += rx;

    if(align.vert() == Align::VCenter)
        py += ry / 2;
    else if(align.vert() == Align::Bottom)
        py += ry;

    return Position(px, py);
}

