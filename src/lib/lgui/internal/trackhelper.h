#ifndef TRACKHELPER_H
#define TRACKHELPER_H

#include "lgui/lgui_types.h"

namespace lgui {
class Widget;

namespace dtl {

bool is_abs_pos_still_inside(Position position, const Widget& widget);

PointF map_from_outside(const Widget& widget, PointF p);

void register_widget_parents_first(std::vector<Widget*>& widgets, Widget* w);

}
}

#endif // TRACKHELPER_H
