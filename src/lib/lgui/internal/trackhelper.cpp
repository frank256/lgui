#include <vector>
#include <stack>
#include <queue>
#include "trackhelper.h"
#include "lgui/widget.h"

namespace lgui {
namespace dtl {

static Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos);

bool is_abs_pos_still_inside(Position pos, const Widget& widget) {
    PointF w_pos = map_from_outside(widget, PointF(pos));
    return widget.is_inside(w_pos);
}

Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos) {
    // We take a little shortcut... This works as long as there is no scaling involved.
    return {pos.x() - abs_rect.x(), pos.y() - abs_rect.y()};
}

PointF map_from_outside(const Widget& widget, PointF p) {
    std::stack<const Widget*> stack;
    // Trace to root:
    const Widget* root = &widget;
    while(root) {
        stack.push(root);
        root = root->parent();
    }
    while(!stack.empty()) {
        const Widget* node = stack.top();
        stack.pop();
        p = node->map_from_parent(p);
    }
    return p;
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


}
}
