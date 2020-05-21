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

#include "mousetrackhelper.h"
#include "lgui/mouseevent.h"
#include "lgui/vector_utils.h"
#include "trackhelper.h"

namespace lgui {
namespace dtl {

void MouseTrackHelper::print_info() {
    for (uint i = 0; i < mwidgets_under_mouse.size(); ++i) {
        Widget* w = mwidgets_under_mouse[i];
        int found_at = -1;
        for (uint j = 0; j < mwidgets_under_mouse.size(); ++j) {
            if (w->parent() == mwidgets_under_mouse[j])
                found_at = j;
        }
        printf("%d. Widget %p, found parent (%p) at %d.\n", i, w, w->parent(), found_at);
    }
    printf("\n");
}

bool MouseTrackHelper::is_under_mouse(const Widget& widget) const {
    return contains(mwidgets_under_mouse, &const_cast<Widget&>(widget)); // FIX this.
}

void MouseTrackHelper::remove_not_under_mouse(Position mouse_pos, double timestamp) {
    erase_remove_if(mwidgets_under_mouse, [mouse_pos, timestamp, this](Widget* w) -> bool {
        if (!is_abs_pos_still_inside(mouse_pos, *w)) {
            mdistr.send_mouse_event_abs_pos(w, mouse_pos, MouseEvent(MouseEvent::Left, timestamp, mouse_pos, 0));
            return true;
        }
        return false;
    });
}

void MouseTrackHelper::register_mouse_entered(const WidgetTreeTraversalStack& traversal_stack, double timestamp, int button) {
    for (int stack_index = traversal_stack.get_no_entries() - 1; stack_index >= 0; stack_index--) {
        const auto& entry = traversal_stack.get(stack_index);
        Widget* w = entry.w;
        if (!contains(mwidgets_under_mouse, w)) {
            register_widget_parents_first(mwidgets_under_mouse, w);
            mdistr.send_mouse_event(w, MouseEvent(MouseEvent::Entered, timestamp, entry.p.to_point(), button));
        }
    }
}

void MouseTrackHelper::clear_under_mouse(bool send_events) {
    if (send_events) {
        for (Widget* w : mwidgets_under_mouse) {
            mdistr.send_mouse_event_abs_pos(w, mlast_mouse_state.pos(),
                                            MouseEvent(MouseEvent::Left, mlast_mouse_state.timestamp(),
                                                       mlast_mouse_state.pos(), 0));
        }
    }
    mwidgets_under_mouse.clear();
}

void MouseTrackHelper::reregister_under_mouse(const WidgetTreeTraversalStack& traversal_stack, bool send_move) {
    if (!traversal_stack.is_empty()) {
        register_mouse_entered(traversal_stack, mlast_mouse_state.timestamp(), 0);
        if (send_move) {
            mdistr.distribute_mouse_event(traversal_stack,
                                          MouseEvent(MouseEvent::Moved, mlast_mouse_state.timestamp(), 0));
        }
    }
}

void MouseTrackHelper::remove_subtree_from_under_mouse(Widget* widget, bool send_events) {
    remove_widget_and_children_from_under_mouse(widget, send_events, false);
}

void MouseTrackHelper::remove_all_except_subtree_from_under_mouse(Widget* widget, bool send_events) {
    remove_widget_and_children_from_under_mouse(widget, send_events, true);
}

void MouseTrackHelper::remove_widget_and_children_from_under_mouse(Widget* widget, bool send_events,
                                                                   bool invert_predicate) {
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
                mdistr.send_mouse_event_abs_pos(w, mlast_mouse_state.pos(),
                                                MouseEvent(MouseEvent::Left, mlast_mouse_state.timestamp(),
                                                           mlast_mouse_state.pos(), 0));
            }
            return true;
        }
        return false;
    });

}

}

}