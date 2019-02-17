#include "eventdistributor.h"

#include "lgui/dragrepresentation.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "focusmanager.h"

namespace lgui {
namespace dtl {

DragRepresentation* EventDistributor::distribute_mouse_event(Widget* target, MouseEvent::Type type,
                                                             double timestamp, Position abs_pos,
                                                             int button, bool to_target_only) const
{
    if (target) {
        Position target_abs_pos = target->get_absolute_position();
        Position rel_mouse_pos = abs_pos - target_abs_pos;
        MouseEvent me(timestamp, mmodifiers_status, type, rel_mouse_pos.x(), rel_mouse_pos.y(), button);
        Widget* w = target;
        // Bubble up.
        while (w != nullptr) {
            if ((w->is_active() && w->is_visible()) || to_target_only) {
                if (w->send_mouse_event(me) || to_target_only) {
                    // Return drag object if widget has started a drag.
                    return me.drag_representation();
                }
            }
            if (w->parent()) {
                rel_mouse_pos = w->map_to_parent(rel_mouse_pos);
                me._set_pos(rel_mouse_pos);
            }
            w = w->parent();
        }
    }
    return nullptr;
}

bool EventDistributor::distribute_dragdrop_event(Widget* target, DragDropEvent::Type type, double timestamp,
                                                 Position abs_pos, int button,
                                                 DragRepresentation* drag_repr, bool to_target_only) const
{
    if (target) {
        Position target_abs_pos = target->get_absolute_position();
        Position rel_mouse_pos = abs_pos - target_abs_pos;
        DragDropEvent de(timestamp, mmodifiers_status, type, rel_mouse_pos.x(), rel_mouse_pos.y(), button,
                         drag_repr);
        Widget* w = target;
        // Bubble up.
        while (w != nullptr) {
            if ((w->is_active() && w->is_visible()) || to_target_only) {
                if (w->send_dragdrop_event(de) || to_target_only) {
                    return de.is_drag_accepted();
                }
            }
            if (w->parent()) {
                rel_mouse_pos = w->map_to_parent(rel_mouse_pos);
                de.set_pos(rel_mouse_pos);
            }
            w = w->parent();
        }
    }
    return false;
}


bool EventDistributor::distribute_key_event(KeyEvent& event)
{
    // First try focus widget, then its parents, finally all widgets.
    Widget* fw = mfocus_mngr.focus_widget();
    while (fw != nullptr) {
        if (fw->is_visible() && fw->is_active()) {
            if (fw->send_key_event(event))
                break;
        }
        fw = fw->parent();
    }
    return event.has_been_consumed();
    // Broadcast to key listeners.
    /*for(Widget* w : mglobal_key_listeners) {
        if(send_key_event_to_widget(w, event))
            break;
    }*/
}

}
}