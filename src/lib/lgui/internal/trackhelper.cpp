#include <vector>
#include <stack>
#include <queue>
#include "trackhelper.h"
#include "lgui/widget.h"

namespace lgui {
namespace dtl {

PointF map_from_absolute(Widget* widget, Point p) {
    WidgetTreeTraversalStack stack;
    trace_back_traversal(widget, p, stack);
    return stack.topmost_widget_pos();
}

void register_widget_parents_first(std::vector<Widget*>& widgets, Widget* w) {
    std::vector<Widget*>::const_iterator dest_pos = widgets.end();
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        if (w->parent() == *it) {
            dest_pos = it.base();
            break;
        }
        if ((*it)->parent() == w) {
            dest_pos = it.base();
            --dest_pos;
            break;
        }
    }
    widgets.insert(dest_pos, w);
}

void trace_back_traversal(Widget* widget, Point pos, WidgetTreeTraversalStack& traversal_stack) {
    // Go up from widget to root and push path onto traversal stack - backwards.
    traversal_stack.new_backwards_traversal();
    Widget* w = widget;
    while (w) {
        traversal_stack.push_backwards(w, PointF());
        w = w->parent();
    }
    traversal_stack.backwards_traversal_finished();
    // Now go down the recorded path from root to widget again and save all local positions of pos.
    PointF p = PointF(pos);
    for (int i = 0; i < traversal_stack.get_no_entries(); ++i) {
        WidgetTreeTraversalStack::Entry& entry = traversal_stack.get(i);
        p = entry.w->map_from_parent(p);
        entry.p = p;
    }
}

}
}
