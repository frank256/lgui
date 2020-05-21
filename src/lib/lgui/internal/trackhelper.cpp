#include <vector>
#include <stack>
#include <queue>
#include "trackhelper.h"
#include "lgui/widget.h"

namespace lgui {
namespace dtl {

//static Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos);

// TODO: reduce-tree-traversals
bool is_abs_pos_still_inside(Position pos, const Widget& widget) {
    PointF w_pos = map_from_outside(widget, PointF(pos));
    w_pos = widget.map_to_parent(w_pos);
//    printf("\nis_abs_still_inside %p (%0f, %0f): %d\n", &widget, w_pos.x(), w_pos.y(), widget.is_inside(w_pos));
    return widget.is_inside(w_pos);
}

//Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos) {
//    // We take a little shortcut... This works as long as there is no scaling involved.
//    return {pos.x() - abs_rect.x(), pos.y() - abs_rect.y()};
//}

// TODO: reduce-tree-traversals
PointF map_from_outside(const Widget& widget, PointF p) {
    std::stack<const Widget*> stack;
    // Trace to root:
    const Widget* root = &widget;
    while (root) {
        stack.push(root);
        root = root->parent();
    }
    while (!stack.empty()) {
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

void trace_back_traversal(Widget* w, Point pos, WidgetTreeTraversalStack& traversal_stack) {
    // Go up from widget to root and push path onto traversal stack - backwards.
    traversal_stack.new_backwards_traversal();
    Widget* root = w;
    while (root) {
        traversal_stack.push_backwards(root, PointF());
        root = root->parent();
    }
    traversal_stack.backwards_traversal_finished();
    // Now go  down the recorded path from root to widget again and save all local positions of pos.
    PointF p = PointF(pos);
    for (int i = 0; i < traversal_stack.get_no_entries(); ++i) {
        WidgetTreeTraversalStack::Entry& entry = traversal_stack.get(i);
        p = entry.w->map_from_parent(p);
        entry.p = p;
    }
}

}
}
