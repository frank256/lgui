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

#include "mousehandler.h"
#include "eventhandlerbase.h"

namespace lgui {

namespace dtl {

void MouseHandler::handle_mouse_pressed(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    int button = event.mouse.button;

    Widget* umw = mdragged_widget;

    if(umw == nullptr)
        umw = mevent_handler_base.get_widget_at(mouse_pos);

    if(umw) {
        // FIXME: how to deal with mouse being only pressed?
        if(!mmouse_tracker.is_under_mouse(*umw))
            mmouse_tracker.register_mouse_entered(umw, mouse_pos, button, event.timestamp);

        mdistr.distribute_mouse_event(umw, MouseEvent(MouseEvent::Pressed, event.timestamp, mouse_pos, button));
    }

    // If the widget didn't handle the press but someone else it bubbled up to, they
    // will receive click also via bubble-up, so just store the original widget here.
    mlast_mouse_pressed_on = umw;
    mdragged_widget = umw;

    mlast_mouse_state.set_pressed_button(button);
    mlast_mouse_state.set_dragged_button(button);
}

void MouseHandler::handle_mouse_released(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    int button = event.mouse.button;

    if(mdrag_drop_tracker.drag_representation()) {
        mdrag_drop_tracker.finish_drag_drop_operation(mouse_pos, button, event.timestamp);
        mmouse_tracker.clear_under_mouse();
        reregister_under_mouse(false, true);
        return; // This is exclusive.
    }

    // Drag means the widget stays the same.
    Widget* umw = mdragged_widget;
    if(!umw)
        umw = mevent_handler_base.get_widget_at(mouse_pos);

    // Reset this before clicked events are distributed.
    mdragged_widget = nullptr;

    if(umw) {
        mdistr.distribute_mouse_event(umw, MouseEvent(MouseEvent::Released, event.timestamp, mouse_pos, button));

        // Send click?
        if(mlast_mouse_pressed_on == umw && button == mlast_mouse_state.pressed_button()) {
            // FIXME: Only send click if still over widget?
            if(mmouse_tracker.is_under_mouse(*umw)) {
                mdistr.distribute_mouse_event(umw, MouseEvent(MouseEvent::Clicked, event.timestamp, mouse_pos,
                                                              button));
            }
            mlast_mouse_pressed_on = nullptr;
        }
    }
}

void MouseHandler::handle_mouse_moved(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    // Save so we can re-update under-mouse-queue even without new events.
    mlast_mouse_state.set_pos(mouse_pos);

    // Remove any widgets that are not under the mouse anymore, sending left events.
    // Will do both mouse-left and drag-left events; mouse-left events are even sent when
    // drag is active (we leave but don't enter then).
    mmouse_tracker.remove_not_under_mouse(mouse_pos, event.timestamp);

    DragRepresentation* drag_repr = mdrag_drop_tracker.drag_representation();
    if (drag_repr) {
        mdrag_drop_tracker.remove_not_under_drag(mouse_pos, event.timestamp);
        handle_mouse_moved_dragdrop(mouse_pos, event.timestamp);
        return; // This is exclusive.
    }

    if (mdragged_widget) {
        handle_mouse_moved_dragging(mouse_pos, event.timestamp);
    }
    else {
        handle_mouse_moved_normal(mouse_pos, event.timestamp);
    }
}

void MouseHandler::handle_mouse_moved_dragdrop(Position mouse_pos, double timestamp)
{
    DragRepresentation* drag_repr = mdrag_drop_tracker.drag_representation();
    ASSERT(drag_repr);

    // Drag left events were already sent in remove_not_under_mouse
    Widget* under_mouse = mevent_handler_base.get_widget_at(mouse_pos);
    if(under_mouse) {
        mdrag_drop_tracker.register_drag_entered(under_mouse, mouse_pos, 0, timestamp);
    }
    drag_repr->_set_pos(mouse_pos - drag_repr->hotspot());
}

void MouseHandler::handle_mouse_moved_dragging(Position mouse_pos, double timestamp)
{
    if(!mmouse_tracker.is_under_mouse(*mdragged_widget)) {
        // Widget being dragged, but mouse left.
        Widget* under_mouse = mevent_handler_base.get_widget_at(mouse_pos);
        if(under_mouse == mdragged_widget) {
            // Re-entered.
            mmouse_tracker.register_mouse_entered(under_mouse, mouse_pos, 0, timestamp);
        }
        // else: No, don't send enter events to other widgets while a widget is being dragged.
    }
    // Widget will receive dragged event even if mouse has left.
    DragRepresentation *drag_repr =
        mdistr.distribute_mouse_event(mdragged_widget, MouseEvent(MouseEvent::Dragged, timestamp, mouse_pos,
                                                                  mlast_mouse_state.dragged_button()));
    // If a drag representation is returned from a drag event, a drag-drop operation has been started,
    // we need to switch-modes.
    if(drag_repr) {
        mdragged_widget = nullptr;
        mdrag_drop_tracker.prepare_drag_drop_operation(drag_repr, mouse_pos);
    }
}

void MouseHandler::handle_mouse_moved_normal(Position mouse_pos, double timestamp)
{
    // Send mouse enter events to all widgets newly under mouse and track them.
    // We will decide for ONE widget under mouse and send entered to its parent if we haven't tracked
    // them already.
    Widget* under_mouse = mevent_handler_base.get_widget_at(mouse_pos);
    if(under_mouse) {
        mmouse_tracker.register_mouse_entered(under_mouse, mouse_pos, 0, timestamp);
        // They also receive a first mouse move, so their move handlers also receives the first
        // position.
        mdistr.distribute_mouse_event(under_mouse, MouseEvent(MouseEvent::Moved, timestamp, mouse_pos, 0));
    }
}

void MouseHandler::reregister_under_mouse(bool do_dd, bool send_move)
{
    Widget* under_mouse = mevent_handler_base.get_widget_at(mlast_mouse_state.pos());

    if (under_mouse) {
        if (!mdrag_drop_tracker.drag_representation())
            mmouse_tracker.reregister_under_mouse(under_mouse, send_move);
        else if (do_dd) {
            mdrag_drop_tracker.reregister_drag(under_mouse, send_move);
        }
    }
}

void MouseHandler::update_under_mouse()
{
    const Position& last_mouse_pos = mlast_mouse_state.pos();
    mmouse_tracker.remove_not_under_mouse(last_mouse_pos, mlast_mouse_state.timestamp());
    if (mdrag_drop_tracker.drag_representation()) {
        mdrag_drop_tracker.remove_not_under_drag(last_mouse_pos, mlast_mouse_state.timestamp());
    }
    reregister_under_mouse(true, true);
}

void MouseHandler::handle_mouse_wheel(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);

    MouseEvent::Type type;
    if(event.mouse.dz < 0)
        type = MouseEvent::WheelDown;
    else if(event.mouse.dz > 0)
        type = MouseEvent::WheelUp;
    else
        return;

    Widget* under_mouse = mdragged_widget;
    if(under_mouse == nullptr)
        under_mouse = mevent_handler_base.get_widget_at(mouse_pos);

    if(under_mouse)
        mdistr.distribute_mouse_event(under_mouse, MouseEvent(type, event.timestamp, mouse_pos, 0));
}


void MouseHandler::set_last_timestamp(double timestamp)
{
    mlast_mouse_state.set_timestamp(timestamp);
}

void MouseHandler::_handle_widget_invisible_or_inactive(Widget& widget)
{
    mmouse_tracker.remove_subtree_from_under_mouse(&widget, true);
    mdrag_drop_tracker.remove_subtree_from_under_drag(&widget, true, true);

    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;

    reregister_under_mouse(true, true);
}

void MouseHandler::_handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed)
{
    mmouse_tracker.remove_subtree_from_under_mouse(&widget, !going_to_be_destroyed);
    mdrag_drop_tracker.remove_subtree_from_under_drag(&widget, !going_to_be_destroyed, false);

    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;
}

void MouseHandler::_handle_modal_focus_changed()
{
    Widget* modal_w = mevent_handler_base.modal_focus_widget();
    if(modal_w != nullptr) {
        mmouse_tracker.remove_all_except_subtree_from_under_mouse(modal_w, true);
        mdrag_drop_tracker.remove_all_except_subtree_from_under_drag(modal_w, true, true);
        if(mlast_mouse_pressed_on && !mlast_mouse_pressed_on->is_child_of_recursive(modal_w))
            mlast_mouse_pressed_on = nullptr;
        reregister_under_mouse(true, true);
    }
    else {
        // Modal focus released: do a complete update of mouse queue.
        mmouse_tracker.clear_under_mouse();
        mdrag_drop_tracker.clear_under_drag();
        reregister_under_mouse(true, false);
    }

}

void MouseHandler::before_top_widget_changes()
{
    mmouse_tracker.clear_under_mouse();
    mdrag_drop_tracker.clear_under_drag();
    mlast_mouse_pressed_on = nullptr;
}

}

}

