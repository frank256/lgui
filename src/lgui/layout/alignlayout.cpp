#include "alignlayout.h"

namespace lgui {

AlignLayout::AlignLayout()
{

}

void AlignLayout::add_item(const LayoutItemProxy& elem, const Align& align)
{
    mitems.emplace_back(dtl::AlignLayoutItem(elem, align));
    added_elem(*elem.elem());
}

MeasureResults AlignLayout::measure(SizeConstraint wc, SizeConstraint hc)
{
    mlast_wc = wc;
    mlast_hc = hc;

    TooSmallAccumulator too_small;

    Size max_content_size;

    for(auto& li : mitems) {
        if(li.skip())
            continue;

        SizeConstraint horz = get_horz_constraint(wc, li.align());
        SizeConstraint vert = get_vert_constraint(hc, li.align());

        MeasureResults r = li.measure(horz, vert);
        Size s = too_small.consider(r);

        li.set_allotted_size(s);

        max_content_size.set_w(std::max(max_content_size.w(), s.w()));
        max_content_size.set_h(std::max(max_content_size.h(), s.h()));
    }

    return MeasureResults(wc.mode() != SizeConstraintMode::NoLimits ? wc.value() : max_content_size.w(),
                          hc.mode() != SizeConstraintMode::NoLimits ? hc.value() : max_content_size.h(),
                          too_small.width_too_small(), too_small.height_too_small());

}

void AlignLayout::do_layout(const Rect& r)
{
    if(!mtarget)
        return;

    Size ts = r.size();
    SizeConstraint wc = SizeConstraint(ts.w(), SizeConstraintMode::Exactly);
    SizeConstraint hc = SizeConstraint(ts.h(), SizeConstraintMode::Exactly);

    if(mtarget->needs_relayout() ||
       (wc != mlast_wc || hc != mlast_hc)) {
        measure(wc, hc);
    }

    for(auto& li : mitems) {
        if(li.skip())
            continue;
        Size lis = li.allotted_size();
        li.layout(Rect(do_alignment(Rect(0, 0, r.size()), lis, li.align()), lis));
    }
}


SizeConstraint AlignLayout::get_horz_constraint(SizeConstraint wc, Align align) {
    if (wc.mode() == NoLimits)
        return wc;
    else if (align.horz() == Align::HMatchParent)
        return SizeConstraint(wc.value(), SizeConstraintMode::Exactly);
    else
        return SizeConstraint(wc.value(), SizeConstraintMode::Maximum);
}

SizeConstraint AlignLayout::get_vert_constraint(SizeConstraint hc, Align align)
{
    if (hc.mode() == NoLimits)
        return hc;
    else if (align.vert() == Align::VMatchParent)
        return SizeConstraint(hc.value(), SizeConstraintMode::Exactly);
    else
        return SizeConstraint(hc.value(), SizeConstraintMode::Maximum);
}

}
