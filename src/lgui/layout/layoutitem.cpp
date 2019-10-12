#include "layoutitem.h"

lgui::MeasureResults lgui::LayoutItem::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) {
    if (mle) {
        mlast_measure_size = mle->measure(get_horz_constraint(wc), get_vert_constraint(hc));
    }
    else {
        mlast_measure_size = Size();
    }
    int w = mlast_measure_size.w() + mmargin.horz();
    int h = mlast_measure_size.h() + mmargin.vert();;
    if (wc.mode() != SizeConstraintMode::NoLimits && (malign.align() & Align::HTakeAll)) {
        w = wc.value();
    }
    if (hc.mode() != SizeConstraintMode::NoLimits && (malign.align() & Align::VTakeAll)) {
        h = hc.value();
    }
    return force_size_constraints(Size(w, h), wc, hc);
}

void lgui::LayoutItem::layout(const lgui::Rect& r) {
    if (mle) {
        Size size = mlast_measure_size;
        lgui::Rect without_margin(r.pos() + mmargin.left_top_offs(), mmargin.sub(r.size()));
        size.set(std::min(without_margin.w(), size.w()), std::min(without_margin.h(), size.h()));
        lgui::Position align_offset = do_alignment(without_margin, size, malign);
        mle->layout(Rect(align_offset, size));
    }
}

lgui::SizeConstraint lgui::LayoutItem::get_horz_constraint(lgui::SizeConstraint wc) const {
    if (wc.mode() == NoLimits)
        return wc;

    SizeConstraintMode mode = (malign.is(Align::HMatchParent) &&
                               (malign.is(Align::HTakeAll) || wc.mode() == SizeConstraintMode::Exactly))
                              ? SizeConstraintMode::Exactly
                              : SizeConstraintMode::Maximum;

    return SizeConstraint(wc.value() - mmargin.horz(), mode);
}

lgui::SizeConstraint lgui::LayoutItem::get_vert_constraint(lgui::SizeConstraint hc) const {
    if (hc.mode() == NoLimits)
        return hc;

    SizeConstraintMode mode = (malign.is(Align::VMatchParent) &&
                               (malign.is(Align::VTakeAll) || hc.mode() == SizeConstraintMode::Exactly))
                              ? SizeConstraintMode::Exactly
                              : SizeConstraintMode::Maximum;

    return SizeConstraint(hc.value() - mmargin.vert(), mode);
}
