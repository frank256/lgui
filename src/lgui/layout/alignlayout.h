#ifndef ALIGNLAYOUT_H
#define ALIGNLAYOUT_H

#include <vector>
#include "layoutitemcontainerbase.h"

namespace lgui {
namespace dtl {
    class AlignLayoutItem : public LayoutItem {
        public:
            AlignLayoutItem(const LayoutItemProxy& proxy, const Align& align)
                : LayoutItem(proxy), malign(align)
            {}

            const Align& align() const { return malign; }

        private:
            Align malign;
    };
}

/** Layout to just align widgets inside the parent. This is more of a dummy layout, may be used to easily
 *  center one child, for example. It will always take all the space it can for the target widget. */
class AlignLayout : public lgui::LayoutItemContainerBase<dtl::AlignLayoutItem, std::vector<dtl::AlignLayoutItem> >
{
    public:
        AlignLayout();

        void add_item(const LayoutItemProxy& elem, const Align& align);

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        void do_layout(const Rect& r) override;

    private:
        static SizeConstraint get_horz_constraint(SizeConstraint wc, Align align);
        static SizeConstraint get_vert_constraint(SizeConstraint wc, Align align);
        SizeConstraint mlast_wc, mlast_hc;
};

}

#endif // ALIGNLAYOUT_H
