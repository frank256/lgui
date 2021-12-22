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

#include "lgui_layout_utils.h"

namespace lgui {

Position do_alignment(const Rect& space, const Size& size, Align align) {
    int rx = space.w() - size.w(), ry = space.h() - size.h();
    int px = space.x(), py = space.y();

    if (align.horz() == Align::HCenter)
        px += rx / 2;
    else if (align.horz() == Align::Right)
        px += rx;

    if (align.vert() == Align::VCenter)
        py += ry / 2;
    else if (align.vert() == Align::Bottom)
        py += ry;

    return {px, py};
}

MeasureResult force_size_constraint(int size, SizeConstraint sc) {
    if (sc.mode() == SizeConstraintMode::Exactly)
        return {sc.value(), size > sc.value()};
    else if (sc.mode() == SizeConstraintMode::Maximum) {
        if (size <= sc.value())
            return {size, false};
        else
            return {sc.value(), true};
    }
    else {
        return {size, false};
    }
}

MeasureResult force_size_constraint(int size, SizeConstraint sc, bool too_small_acc) {
    if (sc.mode() == SizeConstraintMode::Exactly)
        return {sc.value(), (size > sc.value()) || too_small_acc};
    else if (sc.mode() == SizeConstraintMode::Maximum) {
        if (size <= sc.value())
            return {size, too_small_acc};
        else
            return {sc.value(), true};
    }
    else {
        return {size, too_small_acc};
    }
}

MeasureResult force_size_constraint(MeasureResult r, SizeConstraint sc) {
    return force_size_constraint(r.value(), sc, r.is_too_small());
}

MeasureResults force_size_constraints(Size s, SizeConstraint wc, SizeConstraint hc) {
    return {force_size_constraint(s.w(), wc),
            force_size_constraint(s.h(), hc)};
}

MeasureResults force_size_constraints(Size s, SizeConstraint wc, SizeConstraint hc,
                                      TooSmallAccumulator accum) {
    return {force_size_constraint(s.w(), wc, accum.width_too_small()),
            force_size_constraint(s.h(), hc, accum.height_too_small())};
}

MeasureResults force_size_constraints(MeasureResults mr, SizeConstraint wc,
                                      SizeConstraint hc) {
    return {force_size_constraint(mr.w(), wc, mr.is_width_too_small()),
            force_size_constraint(mr.h(), hc, mr.is_height_too_small())};
}


SizeConstraint SizeConstraint::adapted_for_child(int space_used) const {
    if (mode() == SizeConstraintMode::NoLimits)
        return SizeConstraint(0, SizeConstraintMode::NoLimits);

    int space = std::max(value() - space_used, 0);
    return SizeConstraint(space, SizeConstraintMode::Maximum);
}

}