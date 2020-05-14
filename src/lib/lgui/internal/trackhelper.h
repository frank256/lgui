#ifndef TRACKHELPER_H
#define TRACKHELPER_H

#include "lgui/lgui_types.h"

namespace lgui {
class Widget;

namespace dtl {

bool is_abs_pos_still_inside(Position position, const Widget& widget);

}
}

#endif // TRACKHELPER_H
