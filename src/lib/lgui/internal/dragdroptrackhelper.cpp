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

#include "dragdroptrackhelper.h"

#include "../vector_utils.h"
#include "trackhelper.h"

namespace lgui {

namespace dtl {

void DragDropTrackHelper::remove_not_under_drag(const WidgetTreeTraversalStack& traversal_stack, double timestamp) {
    if (mdrag_repr) {
        // Remove any widgets that are not under the drag repr. anymore, sending drag left events.
        erase_remove_if(mwidgets_under_drag, [this, &traversal_stack, timestamp](Widget* w) -> bool {
            if (!traversal_stack.contains(*w)) {
                mdistr.send_dragdrop_event(w, DragDropEvent(DragDropEvent::Left, timestamp, mdrag_repr));
                if (w == mdrag_repr->target_widget())
                    mdrag_repr->_set_target_widget(nullptr); // target lost
                return true;
            }
            return false;
        });
    }
}

void DragDropTrackHelper::register_drag_entered(const WidgetTreeTraversalStack& stack, double timestamp, int button) {
    ASSERT(mdrag_repr);
    for (int stack_index = stack.get_no_entries() - 1; stack_index >= 0; --stack_index) {
        const auto& entry = stack.get(stack_index);
        Widget* w = entry.w;
        if (!contains(mwidgets_under_drag, w)) {
            register_widget_parents_first(mwidgets_under_drag, w);
            if (mdistr.send_dragdrop_event(w, DragDropEvent(DragDropEvent::Entered, timestamp,
                                                            entry.p.to_point(), button, mdrag_repr))) {
                // We found a widget that wants the drag.
                // FIXME: break?
                mdrag_repr->_set_target_widget(w);
            }
        }
    }
}

void DragDropTrackHelper::prepare_drag_drop_operation(DragRepresentation* drag_repr, Position mouse_pos) {
    ASSERT(drag_repr);
    drag_repr->_set_pos(mouse_pos - drag_repr->hotspot());
    mdrag_repr = drag_repr;
}

// Normal way to end a drap-drog-operation contained in this method.
// Note it can also be cancelled if the source widget is somehow lost in the middle of a drag-drop op.
void DragDropTrackHelper::finish_drag_drop_operation(Position mouse_pos, int button, double timestamp) {
    ASSERT(mdrag_repr);
    Widget* src = mdrag_repr->source_widget();
    Widget* target = mdrag_repr->target_widget();
    if (target) {
        mdistr.send_dragdrop_event_abs_pos(target, mouse_pos, DragDropEvent(DragDropEvent::Dropped, timestamp,
                                                                            button, mdrag_repr));
    }
    if (src) {
        mdistr.send_dragdrop_event(src, DragDropEvent(DragDropEvent::DragEnded, timestamp, button, mdrag_repr));
        // Also send a mouse-released event even if the mouse was released elsewhere to
        // keep the pressed-released symmetry for the source widget.
        // Coordinates are, of course, quite off here.
        mdistr.send_mouse_event_abs_pos(src, mouse_pos, MouseEvent(MouseEvent::Released, timestamp, button));
    }
    // Send remaining left events: drag leaves everyone in list that is not target.
    // FIXME: Unsure whether to do this, since it's dropped already.
    for (Widget* w : mwidgets_under_drag) {
        if (w != target)
            mdistr.send_dragdrop_event(w, DragDropEvent(DragDropEvent::Left, timestamp, mdrag_repr));
    }
    mwidgets_under_drag.clear();

    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;
}

// Terminate the drag prematurely (e.g. when source has become invalid).
void DragDropTrackHelper::abort_drag(bool send_events, bool send_dd_end_to_gone_src) {
    if (!mdrag_repr)
        return;
    Widget* src = mdrag_repr->source_widget();
    if (src) {
        if (send_dd_end_to_gone_src) {
            mdistr.send_dragdrop_event(src, DragDropEvent(DragDropEvent::DragEnded, mlast_mouse_state.timestamp(),
                                                          mlast_mouse_state.dragged_button(), mdrag_repr));
            mdistr.send_mouse_event_abs_pos(src, mlast_mouse_state.pos(),
                                            MouseEvent(MouseEvent::Released, mlast_mouse_state.timestamp(),
                                                       mlast_mouse_state.dragged_button()));
        }
        mdrag_repr->_clear_source_widget();
    }
    // A d&d operation ending means under drag (& under mouse) buffers are cleared.
    if (send_events) {
        for (Widget* w : mwidgets_under_drag) {
            mdistr.send_dragdrop_event(w,
                                       DragDropEvent(DragDropEvent::Left, mlast_mouse_state.timestamp(), mdrag_repr));
        }

    }
    mwidgets_under_drag.clear();

    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;
}

void DragDropTrackHelper::clear_under_drag() {
    for (Widget* w : mwidgets_under_drag) {
        mdistr.send_dragdrop_event(w, DragDropEvent(DragDropEvent::Left, mlast_mouse_state.timestamp(), mdrag_repr));
    }
    mwidgets_under_drag.clear();

    if (mdrag_repr) {
        mdrag_repr->_set_target_widget(nullptr);
        Widget* src = mdrag_repr->source_widget();
        if (src) {
            abort_drag(true, true);
        }
    }
}

void DragDropTrackHelper::reregister_drag(const WidgetTreeTraversalStack& traversal_stack, bool send_move) {
    Position mouse_pos = mlast_mouse_state.pos();

    if (!traversal_stack.is_empty()) {
        register_drag_entered(traversal_stack, mlast_mouse_state.timestamp(), mlast_mouse_state.dragged_button());
    }

    if (send_move && mdrag_repr->target_widget()) {
        mdistr.send_drag_move_event_to_target_widget(traversal_stack, mouse_pos, mlast_mouse_state.timestamp(),
                                                     mlast_mouse_state.dragged_button(), mdrag_repr);
    }
}

void DragDropTrackHelper::remove_subtree_from_under_drag(Widget* widget, bool send_events,
                                                         bool send_dd_end_to_gone_src) {
    remove_widget_and_children_from_under_drag(widget, send_events, send_dd_end_to_gone_src, false);
}

void DragDropTrackHelper::remove_all_except_subtree_from_under_drag(Widget* widget, bool send_events,
                                                                    bool send_dd_end_to_gone_src) {
    remove_widget_and_children_from_under_drag(widget, send_events, send_dd_end_to_gone_src, true);
}

void DragDropTrackHelper::remove_widget_and_children_from_under_drag(Widget* widget, bool send_events,
                                                                     bool send_dd_end_to_gone_src,
                                                                     bool invert_predicate) {
    std::function<bool(Widget* w)> predicate;
    if (!invert_predicate) {
        predicate = [widget](Widget* w) { return w->is_child_of_recursive(widget); };
    }
    else {
        predicate = [widget](Widget* w) { return !w->is_child_of_recursive(widget); };
    }

    erase_remove_if(mwidgets_under_drag, [this, send_events, &predicate](Widget* w) -> bool {
        if (predicate(w)) {
            if (send_events) {
                mdistr.send_dragdrop_event(w, DragDropEvent(DragDropEvent::Left, mlast_mouse_state.timestamp(),
                                                            mdrag_repr));
            }
            return true;
        }
        return false;
    });

    if (mdrag_repr) {
        // Target has got the "left"-event already if events are to be sent.
        if (mdrag_repr->target_widget() && predicate(mdrag_repr->target_widget()))
            mdrag_repr->_set_target_widget(nullptr);

        Widget* src = mdrag_repr->source_widget();
        if (src && predicate(src)) {
            // Source has become invalid.
            abort_drag(send_events, send_dd_end_to_gone_src);
        }
    }
}

}

}

