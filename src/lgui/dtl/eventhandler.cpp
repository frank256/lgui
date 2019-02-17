/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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
#include <functional>

#include "eventhandler.h"
#include "lgui/dragrepresentation.h"

#include "platform/keycodes.h"
#include "lgui/vector_utils.h"

namespace lgui {

namespace dtl {

EventHandler::EventHandler(GUI& gui)
    : mgui(gui),
      mfocus_mngr(this),
      mdistr(mfocus_mngr),
      mmouse_tracker(mdistr, mlast_mouse_state),
      mtop_widget(nullptr), mmodal_widget(nullptr),
      mlast_mouse_pressed_on(nullptr), mdragged_widget(nullptr),
      mtab_moves_focus(true),
      mdistributing_timer_ticks(false)
{}


EventHandler::~EventHandler()
{
    // Do this?
    if(mtop_widget)
        mtop_widget->_recursive_configure({nullptr, nullptr});
    mtop_widget = nullptr;
}

Widget*EventHandler::focus_widget()
{
    return mfocus_mngr.focus_widget();
}


void EventHandler::set_top_widget(TopWidget* top)
{
    mmouse_tracker.clear_under_mouse_and_drag();
    mlast_mouse_pressed_on = nullptr;
    if(mfocus_mngr.modal_focus_widget())
        mfocus_mngr.release_modal_focus(*mfocus_mngr.modal_focus_widget());
    mfocus_mngr.focus_none();

    if(mtop_widget)
        mtop_widget->_recursive_configure({nullptr, nullptr});

    mtop_widget = nullptr;

    if(top) {
        ASSERT(!top->is_added_to_gui());
        ASSERT(!mfocus_mngr.know_widget(*top));
        mtop_widget = top;
        Widget::ConfigInfo ci { &mfocus_mngr, &mgui };
        mtop_widget->_recursive_configure(ci);

        top->set_closed(false);
        Widget* fc = top->focus_child();
        if(!fc)
            fc = top;
        if(fc->is_focusable()) {
            // Is focus child still valid?
            if(fc->is_child_of_recursive(top))
                mfocus_mngr.request_focus(*fc);
            else
                top->set_focus_child(nullptr); // Remove invalid info
        }
        reregister_under_mouse(false, true);
    }
}

void EventHandler::push_external_event(const lgui::ExternalEvent& event)
{
    // to generate events with a reasonable timestamp:
    mlast_mouse_state.set_timestamp(event.timestamp);

    switch(event.type) {
        case ExternalEvent::EVENT_MOUSE_MOVED:
            handle_mouse_moved(event);
            break;
        case ExternalEvent::EVENT_TIMER_TICK:
            handle_timer_tick(event);
            break;
        case ExternalEvent::EVENT_MOUSE_PRESSED:
            handle_mouse_pressed(event);
            break;
        case ExternalEvent::EVENT_MOUSE_RELEASED:
            handle_mouse_released(event);
            break;
        case ExternalEvent::EVENT_MOUSEWHEEL:
            handle_mouse_wheel(event);
            break;
        case ExternalEvent::EVENT_KEY_CHAR:
            handle_key_event(KeyEvent::Char, event);
            break;
        case ExternalEvent::EVENT_KEY_PRESSED:
            handle_key_event(KeyEvent::Pressed, event);
            break;
        case ExternalEvent::EVENT_KEY_RELEASED:
            handle_key_event(KeyEvent::Released, event);
            break;
        default: break;
    }
}

void EventHandler::handle_mouse_wheel(const ExternalEvent& event)
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
        under_mouse = get_widget_at(mouse_pos);

    if(under_mouse)
        mdistr.distribute_mouse_event(under_mouse, type, event.timestamp, mouse_pos, 0);
}

void EventHandler::handle_mouse_pressed(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    int button = event.mouse.button;

    Widget* umw = mdragged_widget;

    if(umw == nullptr)
        umw = get_widget_at(mouse_pos);

    if(umw) {
        // FIXME: how to deal with mouse being only pressed?
        if(!mmouse_tracker.is_under_mouse(*umw))
            mmouse_tracker.register_mouse_entered(umw, mouse_pos, button, event.timestamp);

        mdistr.distribute_mouse_event(umw, MouseEvent::Pressed, event.timestamp, mouse_pos, button);
    }

    // If the widget didn't handle the press but someone else it bubbled up to, they
    // will receive click also via bubble-up, so just store the original widget here.
    mlast_mouse_pressed_on = umw;
    mdragged_widget = umw;

    mlast_mouse_state.set_pressed_button(button);
    mlast_mouse_state.set_dragged_button(button);
}

void EventHandler::handle_mouse_released(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    int button = event.mouse.button;

    if(mmouse_tracker.drag_representation()) {
        mmouse_tracker.finish_drag_drop_operation(mouse_pos, button, event.timestamp);
        reregister_under_mouse(false, true);
        return; // This is exclusive.
    }

    // Drag means the widget stays the same.
    Widget* umw = mdragged_widget;
    if(!umw)
        umw = get_widget_at(mouse_pos);


    // Reset this before clicked events are distributed.
    mdragged_widget = nullptr;

    if(umw) {
        mdistr.distribute_mouse_event(umw, MouseEvent::Released, event.timestamp, mouse_pos, button);

        // Send click?
        if(mlast_mouse_pressed_on == umw && button == mlast_mouse_state.pressed_button()) {
            // FIXME: Only send click if still over widget?
            if(mmouse_tracker.is_under_mouse(*umw)) {
                mdistr.distribute_mouse_event(umw, MouseEvent::Clicked, event.timestamp, mouse_pos, button);
            }
            mlast_mouse_pressed_on = nullptr;
        }
    }
}


void EventHandler::handle_mouse_moved(const ExternalEvent& event)
{
    Position mouse_pos(event.mouse.x, event.mouse.y);
    // Save so we can re-update under-mouse-queue even without new events.
    mlast_mouse_state.set_pos(mouse_pos);

    // Remove any widgets that are not under the mouse anymore, sending left events.
    // Will do both mouse-left and drag-left events; mouse-left events are even sent when
    // drag is active (we leave but don't enter then).
    mmouse_tracker.remove_not_under_mouse(mouse_pos, event.timestamp);

    // Currently drag-drop-ing?
    DragRepresentation* drag_repr = mmouse_tracker.drag_representation();
    if(drag_repr) {
        handle_mouse_moved_dragdrop(mouse_pos, event.timestamp);
        return; // This is exclusive.
    }

    if(!mdragged_widget) {
        handle_mouse_moved_normal(mouse_pos, event.timestamp);
    }
    else {
        handle_mouse_moved_dragging(mouse_pos, event.timestamp);
    }
}

void EventHandler::handle_mouse_moved_dragdrop(Position mouse_pos, double timestamp)
{
    DragRepresentation* drag_repr = mmouse_tracker.drag_representation();
    ASSERT(drag_repr);

    // Drag left events were already sent in remove_not_under_mouse
    Widget* under_mouse = get_widget_at(mouse_pos);
    if(under_mouse) {
        mmouse_tracker.register_drag_entered(under_mouse, mouse_pos, 0, drag_repr, timestamp);
    }
    drag_repr->_set_pos(mouse_pos - drag_repr->hotspot());
}

void EventHandler::handle_mouse_moved_dragging(Position mouse_pos, double timestamp)
{
    if(!mmouse_tracker.is_under_mouse(*mdragged_widget)) {
        // Widget being dragged, but mouse left.
        Widget* under_mouse = get_widget_at(mouse_pos);
        if(under_mouse == mdragged_widget) {
            // Re-entered.
            mmouse_tracker.register_mouse_entered(under_mouse, mouse_pos, 0, timestamp);
        }
        // else: No, don't send enter events to other widgets while a widget is being dragged.
    }
    // Widget will receive dragged event even if mouse has left.
    DragRepresentation *drag_repr =
        mdistr.distribute_mouse_event(mdragged_widget, MouseEvent::Dragged, timestamp,
                                      mouse_pos, mlast_mouse_state.dragged_button());
    // If a drag representation is returned from a drag event, a drag-drop operation has been started,
    // we need to switch-modes.
    if(drag_repr) {
        mdragged_widget = nullptr;
        mmouse_tracker.prepare_drag_drop_operation(drag_repr, mouse_pos);
    }
}

void EventHandler::handle_mouse_moved_normal(Position mouse_pos, double timestamp)
{
    // Send mouse enter events to all widgets newly under mouse and track them.
    // We will decide for ONE widget under mouse and send entered to its parent if we haven't tracked
    // them already.
    Widget* under_mouse = get_widget_at(mouse_pos);
    if(under_mouse) {
        mmouse_tracker.register_mouse_entered(under_mouse, mouse_pos, 0, timestamp);
        // They also receive a first mouse move, so their move handlers also receives the first
        // position.
        mdistr.distribute_mouse_event(under_mouse, MouseEvent::Moved, timestamp, mouse_pos, 0);
    }
}

// This function will return ONE widget that is considered to be under the mouse.
Widget* EventHandler::get_widget_at(Position pos)
{
    if(mmodal_widget) {
        int rel_x = pos.x() - mmodal_widget->pos_x(),
        rel_y = pos.y() - mmodal_widget->pos_y();

        Widget* target = Widget::get_leaf_widget_at_recursive(mmodal_widget, rel_x, rel_y);
        if(!target)
            target = mmodal_widget;
        return target;
    }

    if(mtop_widget) {
        int rel_x = pos.x() - mtop_widget->pos_x(),
        rel_y = pos.y() - mtop_widget->pos_y();
        Widget* target = Widget::get_leaf_widget_at_recursive(mtop_widget, rel_x, rel_y);
        if(target) {
            // Prevent non-modal-widget under mouse - modal focus widget will catch it.
            if(mfocus_mngr.modal_focus_widget()) {
                if(!mfocus_mngr.is_modal_focus_child(target))
                    return mfocus_mngr.modal_focus_widget();
            }
            return target;
        }
    }
    return nullptr;
}

bool EventHandler::_request_modal_widget(Widget& w)
{
    if(mmodal_widget || mfocus_mngr.modal_focus_widget())
        return false;
    ASSERT(w.parent() == nullptr);
    ASSERT(!w.is_added_to_gui());
    ASSERT(!mfocus_mngr.know_widget(w));
    mmodal_widget = &w;
    mmodal_widget->_recursive_configure({&mfocus_mngr, &mgui});
    mfocus_mngr.request_modal_focus(*mmodal_widget);
    return true;
}

bool EventHandler::_release_modal_widget(Widget& w)
{
    if(mmodal_widget != &w)
        return false;
    mfocus_mngr.release_modal_focus(*mmodal_widget);
    mmodal_widget->_recursive_configure({nullptr, nullptr});
    mmodal_widget = nullptr;
    return true;
}

void EventHandler::_subscribe_to_timer_ticks(Widget& w)
{
    if(!contains(mwidgets_subscribed_to_timer_ticks, &w)) {
        if (mdistributing_timer_ticks) {
            if(!contains(mwidgets_timer_ticks_subscriptions_queue, &w)) {
                mwidgets_timer_ticks_subscriptions_queue.push_back(&w);
            }
        }
        else {
            mwidgets_subscribed_to_timer_ticks.push_back(&w);
            mwidgets_subscribed_to_timer_ticks.erase(std::remove(mwidgets_subscribed_to_timer_ticks.begin(),
                        mwidgets_subscribed_to_timer_ticks.end(), nullptr), mwidgets_subscribed_to_timer_ticks.end());
        }
    }
}

void EventHandler::_unsubscribe_from_timer_ticks(Widget& w) {
    if (!mdistributing_timer_ticks) {
        mwidgets_subscribed_to_timer_ticks.erase(std::remove(mwidgets_subscribed_to_timer_ticks.begin(),
                    mwidgets_subscribed_to_timer_ticks.end(), &w), mwidgets_subscribed_to_timer_ticks.end());
        mwidgets_subscribed_to_timer_ticks.erase(std::remove(mwidgets_subscribed_to_timer_ticks.begin(),
                    mwidgets_subscribed_to_timer_ticks.end(), nullptr), mwidgets_subscribed_to_timer_ticks.end());
    }
    else {
        for (auto& subscribed_widget : mwidgets_subscribed_to_timer_ticks) {
            if (subscribed_widget == &w) {
                subscribed_widget = nullptr;
                break;
            }
        }
        mwidgets_timer_ticks_subscriptions_queue.erase(std::remove(mwidgets_timer_ticks_subscriptions_queue.begin(),
                    mwidgets_timer_ticks_subscriptions_queue.end(), &w), mwidgets_timer_ticks_subscriptions_queue.end());
    }
}

void EventHandler::handle_timer_tick(const ExternalEvent& event)
{
    mdistributing_timer_ticks = true;
    TimerTickEvent tte(event.timestamp, event.timer.count);

    for (Widget* w : mwidgets_subscribed_to_timer_ticks) {
        if (w && event.timer.count % w->timer_tick_skip_mod() == 0)
            w->timer_ticked(tte);
    }
    mdistributing_timer_ticks = false;
    if (!mwidgets_timer_ticks_subscriptions_queue.empty()) {
        for (Widget* w : mwidgets_timer_ticks_subscriptions_queue) {
            if (w && w->is_added_to_gui())
                mwidgets_subscribed_to_timer_ticks.push_back(w);
        }
        mwidgets_timer_ticks_subscriptions_queue.clear();
    }
}

void EventHandler::handle_key_event(KeyEvent::Type type,
                                      const ExternalEvent& event)
{
    KeyEvent ke(event.timestamp, type, KeyCode(event.key.code),
             event.key.modifiers, (type == KeyEvent::Char) ? event.key.unichar : 0,
             event.key.repeated);
    int modifiers_status = event.key.modifiers;
    mdistr.set_modifiers_status(modifiers_status);

    bool handled = mdistr.distribute_key_event(ke);

    if((!handled && mtab_moves_focus) &&
       (type == KeyEvent::Char && event.key.code == Keycodes::KEY_TAB)) {
        // since we want it to repeat, act on key char
        bool prev = (modifiers_status & KeyModifiers::KEYMOD_SHIFT);
        mfocus_mngr.tab_move_focus(prev);
    }
}

void EventHandler::_handle_widget_invisible_or_inactive(Widget& widget) {
    mmouse_tracker.remove_subtree_from_under_mouse_and_drag(&widget, true, true);
    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);
    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;
    reregister_under_mouse(true, true);
}

void EventHandler::_handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed)
{
    mmouse_tracker.remove_subtree_from_under_mouse_and_drag(&widget, !going_to_be_destroyed, false);

    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);

    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;

    if(mmodal_widget == &widget)
        mmodal_widget = nullptr;

}

void EventHandler::_handle_modal_focus_changed()
{
    Widget* modal_w = mfocus_mngr.modal_focus_widget();
    if(modal_w != nullptr) {
        mmouse_tracker.remove_all_except_subtree_from_under_mouse_and_drag(modal_w, true, true);
        reregister_under_mouse(true, true);
        if(mlast_mouse_pressed_on && !mlast_mouse_pressed_on->is_child_of_recursive(modal_w))
            mlast_mouse_pressed_on = nullptr;
    }
    else {
        // Modal focus released: do a complete update of mouse queue.
        mmouse_tracker.clear_under_mouse_and_drag();
        reregister_under_mouse(true, false);
    }
}

void EventHandler::reregister_under_mouse(bool do_dd, bool send_move)
{
    Widget* under_mouse = get_widget_at(mlast_mouse_state.pos());

    mmouse_tracker.reregister_under_mouse(under_mouse, do_dd, send_move);
}

void EventHandler::update_under_mouse()
{
    const Position& last_mouse_pos = mlast_mouse_state.pos();
    mmouse_tracker.remove_not_under_mouse(last_mouse_pos, mlast_mouse_state.timestamp());
    reregister_under_mouse(true, true);
}

}

}
