#ifndef TRACKHELPER_H
#define TRACKHELPER_H

#include "lgui/lgui_types.h"
#include "widgettraversalstack.h"

namespace lgui {
class Widget;

namespace dtl {

PointF map_from_absolute(Widget* widget, Point p);

void register_widget_parents_first(std::vector<Widget*>& widgets, Widget* w);

void trace_back_traversal(Widget* widget, Point pos, WidgetTreeTraversalStack& traversal_stack);

}
}

#endif // TRACKHELPER_H
