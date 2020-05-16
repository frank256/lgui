/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-20 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "eventdistributor.h"

#include "lgui/dragrepresentation.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "focusmanager.h"
#include "trackhelper.h"

namespace lgui {
namespace dtl {

DragRepresentation* EventDistributor::distribute_mouse_event(Widget* target, MouseEvent::Type type,
                                                             double timestamp, Position abs_pos,
                                                             int button, bool to_target_only) const
{
    if (target) {

        PointF rel_mouse_pos_f = map_from_outside(*target, PointF(abs_pos));
        Point rel_mouse_pos = rel_mouse_pos_f.to_point();

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
                rel_mouse_pos_f = w->map_to_parent(rel_mouse_pos_f);
                rel_mouse_pos = rel_mouse_pos_f.to_point();
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
        PointF rel_mouse_pos_f = map_from_outside(*target, PointF(abs_pos));
        Point rel_mouse_pos = rel_mouse_pos_f.to_point();
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
                rel_mouse_pos_f = w->map_to_parent(rel_mouse_pos_f);
                rel_mouse_pos = rel_mouse_pos_f.to_point();
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