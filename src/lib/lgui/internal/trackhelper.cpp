#include "trackhelper.h"
#include "lgui/widget.h"

namespace lgui {
namespace dtl {

static Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos);

bool is_abs_pos_still_inside(Position pos, const Widget& widget) {
    Rect r = widget.get_absolute_rect();
    if (!r.contains(pos)) {
        return false;
    }
    if (widget.is_irregular_shape()) {
        Position widget_pos = map_back_to_widget(r, pos);
        return widget.is_inside_irregular_shape(widget_pos);
    }
    return true;
}

Position map_back_to_widget(const Rect& abs_rect, lgui::Position pos) {
    // We take a little shortcut... This works as long as there is no scaling involved.
    return {pos.x() - abs_rect.x(), pos.y() - abs_rect.y()};
}

}
}
