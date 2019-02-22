/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#include <algorithm>
#include "mousetrackhelper.h"
#include "lgui/dragdropevent.h"
#include "lgui/mouseevent.h"
#include "lgui/vector_utils.h"

namespace lgui {
namespace dtl {

bool MouseTrackHelper::is_under_mouse(const Widget& widget) const
{
    return contains(mwidgets_under_mouse, &const_cast<Widget&>(widget)); // FIX this.
}

void MouseTrackHelper::remove_not_under_mouse(Position mouse_pos, double timestamp)
{
    erase_remove_if(mwidgets_under_mouse, [mouse_pos, timestamp, this] (Widget* w) -> bool {
        Rect r = w->get_absolute_rect();
        if (!r.contains(mouse_pos)) {
            mdistr.distribute_mouse_event(w, MouseEvent::Left, timestamp,
                                          mouse_pos, 0, true);
            return true;
        }
        return false;
    });
}

void MouseTrackHelper::register_mouse_entered(Widget* widget, Position mouse_pos, int button,
                                              double timestamp)
{
    for (Widget* umw = widget; umw != nullptr; umw = umw->parent()) {
        if (!contains(mwidgets_under_mouse, umw)) {
            mwidgets_under_mouse.push_back(umw);
            mdistr.distribute_mouse_event(umw, MouseEvent::Entered, timestamp,
                                          mouse_pos, button, true);
        }
    }
}

void MouseTrackHelper::clear_under_mouse(bool send_events)
{
    if (send_events) {
        for (Widget* w : mwidgets_under_mouse) {
            mdistr.distribute_mouse_event(w, MouseEvent::Left, mlast_mouse_state.timestamp(), mlast_mouse_state.pos(), 0, true);
        }
    }
    mwidgets_under_mouse.clear();
}

void MouseTrackHelper::reregister_under_mouse(Widget* under_mouse, bool send_move)
{
    Position mouse_pos = mlast_mouse_state.pos();
    register_mouse_entered(under_mouse, mouse_pos, 0, mlast_mouse_state.timestamp());
    if (send_move) {
        mdistr.distribute_mouse_event(under_mouse, MouseEvent::Moved, mlast_mouse_state.timestamp(),
                                      mouse_pos, 0);
    }
}

void MouseTrackHelper::remove_subtree_from_under_mouse(Widget* widget, bool send_events)
{
    remove_widget_and_children_from_under_mouse(widget, send_events, false);
}

void MouseTrackHelper::remove_all_except_subtree_from_under_mouse(Widget* widget, bool send_events)
{
    remove_widget_and_children_from_under_mouse(widget, send_events, true);
}

void MouseTrackHelper::remove_widget_and_children_from_under_mouse(Widget* widget, bool send_events,
                                                                   bool invert_predicate)
{
    std::function<bool(Widget* w)> predicate;
    if (!invert_predicate) {
        predicate = [widget](Widget* w) { return w->is_child_of_recursive(widget); };
    }
    else {
        predicate = [widget](Widget* w) { return !w->is_child_of_recursive(widget); };
    }

    erase_remove_if(mwidgets_under_mouse, [this, send_events, &predicate](Widget* w) -> bool {
        if (predicate(w)) {
            if (send_events) {
                mdistr.distribute_mouse_event(w, MouseEvent::Left,
                                              mlast_mouse_state.timestamp(),
                                              mlast_mouse_state.pos(), 0, true);
            }
            return true;
        }
        return false;
    });

}


}

}