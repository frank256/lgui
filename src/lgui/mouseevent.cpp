#include "mouseevent.h"


#include <cstdio>

namespace lgui {

    std::string MouseEvent::to_string() const
    {
        char buf[512];
        // these are to be according to enum in class def
        static const char *typestr[] = { "Moved", "Pressed", "Released", "Entered", "Left", "Clicked", "Dragged",
                                        "WheelUp", "WheelDown" };

        snprintf(buf, 512, "-- %.2f: MouseEvent: %s, pos: %d, %d, button: %d, drag_repr: %p",
                 timestamp(), typestr[mtype], mpos.x(), mpos.y(), mbutton, mdrag_repr);
        return std::string(buf);
    }

}
