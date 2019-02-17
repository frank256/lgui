#ifndef LGUI_EVENTDISTRIBUTOR_H
#define LGUI_EVENTDISTRIBUTOR_H

#include "mouseevent.h"
#include "widget.h"
#include "dragdropevent.h"

namespace lgui {

class EventDistributor {

    public:
        explicit EventDistributor(FocusManager& mfocus_mngr)
                : mfocus_mngr(mfocus_mngr), mmodifiers_status(0)
                {}

        DragRepresentation* distribute_mouse_event(Widget * target, MouseEvent::Type type, double timestamp, int abs_x, int abs_y,
                                                   int button, bool to_target_only=false) const;

        bool distribute_dragdrop_event(Widget * target, DragDropEvent::Type type, double timestamp, int abs_x, int abs_y,
                                      int button, DragRepresentation * drag_repr, bool to_target_only) const;

        bool distribute_key_event(KeyEvent& event);

        void set_modifiers_status(int modifiers_status) { mmodifiers_status = modifiers_status; }

    private:
        FocusManager& mfocus_mngr;
        int mmodifiers_status;
};

}

#endif //LGUI_EVENTDISTRIBUTOR_H
