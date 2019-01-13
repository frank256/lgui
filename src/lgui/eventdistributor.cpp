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

#include "dragrepresentation.h"

#include "eventdistributor.h"
#include "platform/keycodes.h"

namespace lgui {

namespace dtl {

EventDistributor::EventDistributor(GUI& gui)
    : mgui(gui),
      mfocus_mngr(this),
      mtop_widget(nullptr), mmodal_widget(nullptr),
      mlast_mouse_pressed_on(nullptr), mdragged_widget(nullptr),
      mdrag_repr(nullptr),
      mlast_mouse_pressed_button(0),
      mlast_mouse_dragged_button(0),
      mlast_timestamp(0.0),
      mmodifiers_status(0),
      mtab_moves_focus(true),
      mdistributing_timer_ticks(false)
{}


EventDistributor::~EventDistributor()
{
    // Do this?
    if(mtop_widget)
        mtop_widget->_recursive_configure({nullptr, nullptr});
    mtop_widget = nullptr;
}


Widget*EventDistributor::focus_widget()
{
    return mfocus_mngr.focus_widget();
}


void EventDistributor::set_top_widget(TopWidget* top)
{
    clear_under_mouse_and_drag();
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

void EventDistributor::push_external_event(const lgui::ExternalEvent& event)
{
    // to generate events with a reasonable timestamp:
    mlast_timestamp = event.timestamp;

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

bool EventDistributor::_request_modal_widget(Widget& w)
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

bool EventDistributor::_release_modal_widget(Widget& w)
{
    if(mmodal_widget != &w)
        return false;
    mfocus_mngr.release_modal_focus(*mmodal_widget);
    mmodal_widget->_recursive_configure({nullptr, nullptr});
    mmodal_widget = nullptr;
    return true;
}

void EventDistributor::_subscribe_to_timer_ticks(Widget& w)
{
    if(!is_widget_in_list(&w, mwidgets_subscribed_to_timer_ticks)) {
        if (mdistributing_timer_ticks) {
            if(!is_widget_in_list(&w, mwidgets_timer_ticks_subscriptions_queue)) {
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

void EventDistributor::_unsubscribe_from_timer_ticks(Widget& w) {
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

void EventDistributor::handle_timer_tick(const ExternalEvent& event)
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

void EventDistributor::handle_key_event(KeyEvent::Type type,
                                      const ExternalEvent& event)
{
    KeyEvent ke(event.timestamp, type, KeyCode(event.key.code),
             event.key.modifiers, (type == KeyEvent::Char) ? event.key.unichar : 0,
             event.key.repeated);
    mmodifiers_status = event.key.modifiers;

    bool handled = distribute_key_event(ke);

    if((!handled && mtab_moves_focus) &&
       (type == KeyEvent::Char && event.key.code == Keycodes::KEY_TAB)) {
        // since we want it to repeat, act on key char
        bool prev = (mmodifiers_status & KeyModifiers::KEYMOD_SHIFT);
        mfocus_mngr.tab_move_focus(prev);
    }
}

void EventDistributor::handle_mouse_wheel(const ExternalEvent& event)
{
    int mouse_x = event.mouse.x;
    int mouse_y = event.mouse.y;
    MouseEvent::Type type;
    if(event.mouse.dz < 0)
        type = MouseEvent::WheelDown;
    else if(event.mouse.dz > 0)
        type = MouseEvent::WheelUp;
    else
        return;

    Widget* under_mouse = mdragged_widget;
    if(under_mouse == nullptr)
        under_mouse = get_widget_at(mouse_x, mouse_y);

    if(under_mouse)
        distribute_mouse_event(under_mouse, type, event.timestamp, mouse_x, mouse_y, 0);
}

void EventDistributor::handle_mouse_pressed(const ExternalEvent& event)
{
    int mouse_x = event.mouse.x;
    int mouse_y = event.mouse.y;
    int button = event.mouse.button;

    Widget* umw = mdragged_widget;

    if(umw == nullptr)
        umw = get_widget_at(mouse_x, mouse_y);

    if(umw) {
        // FIXME: how to deal with mouse being only pressed?
        if(!is_widget_in_list(umw, mwidgets_under_mouse))
            register_mouse_entered(umw, mouse_x, mouse_y, button, event.timestamp);

        distribute_mouse_event(umw, MouseEvent::Pressed, event.timestamp, mouse_x, mouse_y, button);
    }

    // If the widget didn't handle the press but someone else it bubbled up to, they
    // will receive click also via bubble-up, so just store the original widget here.
    mlast_mouse_pressed_on = umw;
    mlast_mouse_pressed_button = button;

    mlast_mouse_dragged_button = button;
    mdragged_widget = umw;
}

void EventDistributor::handle_mouse_released(const ExternalEvent& event)
{
    int mouse_x = event.mouse.x;
    int mouse_y = event.mouse.y;
    int button = event.mouse.button;

    if(mdrag_repr) {
        finish_drag_drop_operation(mouse_x, mouse_y, button, event.timestamp);
        return; // This is exclusive.
    }

    // Drag means the widget stays the same.
    Widget* umw = mdragged_widget;
    if(!umw)
        umw = get_widget_at(mouse_x, mouse_y);


    // Reset this before clicked events are distributed.
    mdragged_widget = nullptr;

    if(umw) {
        distribute_mouse_event(umw, MouseEvent::Released, event.timestamp, mouse_x, mouse_y, button);

        // Send click?
        if(mlast_mouse_pressed_on == umw && button == mlast_mouse_pressed_button) {
            // FIXME: Only send click if still over widget?
            if(is_widget_in_list(umw, mwidgets_under_mouse)) {
                distribute_mouse_event(umw, MouseEvent::Clicked, event.timestamp, mouse_x, mouse_y, button);
            }
            mlast_mouse_pressed_on = nullptr;
        }
    }
}


void EventDistributor::handle_mouse_moved(const ExternalEvent& event)
{
    int mouse_x = event.mouse.x;
    int mouse_y = event.mouse.y;
    // Save so we can re-update under-mouse-queue even without new events.
    mlast_mouse_pos.set(mouse_x, mouse_y);

    // Remove any widgets that are not under the mouse anymore, sending left events.
    // Will do both mouse-left and drag-left events; mouse-left events are even sent when
    // drag is active (we leave but don't enter then).
    remove_not_under_mouse(mouse_x, mouse_y, event.timestamp);

    // Currently drag-drop-ing?
    if(mdrag_repr) {
        // Drag left events were already sent in remove_not_under_mouse
        Widget* under_mouse = get_widget_at(mouse_x, mouse_y);
        if(under_mouse) {
            register_drag_entered(under_mouse, mouse_x, mouse_y, 0, mdrag_repr, event.timestamp);
            // Will be set if drag has been accepted -> send drag move events.
            if(mdrag_repr->target_widget()) {
                distribute_dragdrop_event(mdrag_repr->target_widget(), DragDropEvent::Moved, event.timestamp,
                                          mouse_x, mouse_y, mlast_mouse_dragged_button, mdrag_repr, true);
            }
        }
        Position newpos(mouse_x, mouse_y);
        mdrag_repr->_set_pos(newpos - mdrag_repr->hotspot());
        return; // This is exclusive.
    }


    // Send mouse enter events to all widgets newly under mouse and track them.
    // We will decide for ONE widget under mouse and send entered to its parent if we haven't tracked
    // them already.
    if(!mdragged_widget) {
        Widget* under_mouse = get_widget_at(mouse_x, mouse_y);
        if(under_mouse) {
            register_mouse_entered(under_mouse, mouse_x, mouse_y, 0, event.timestamp);
            // They also receive a first mouse move, so their move handlers also receives the first
            // position.
            distribute_mouse_event(under_mouse, MouseEvent::Moved, event.timestamp,
                                   mouse_x, mouse_y, 0);
        }
    }
    else {
        if(!is_widget_in_list(mdragged_widget, mwidgets_under_mouse)) {
            // Widget being dragged, but mouse left.
            Widget* under_mouse = get_widget_at(mouse_x, mouse_y);
            if(under_mouse == mdragged_widget) {
                // Re-entered.
                register_mouse_entered(under_mouse, mouse_x, mouse_y, 0, event.timestamp);
            }
            // else: No, don't send enter events to other widgets while a widget is being dragged.
        }
        // Widget will receive dragged event even if mouse has left.
        DragRepresentation *drag_repr =
            distribute_mouse_event(mdragged_widget, MouseEvent::Dragged, event.timestamp,
                                   mouse_x, mouse_y, mlast_mouse_dragged_button);
        // If a drag representation is returned from a drag event, a drag-drop operation has been started,
        // we need to switch-modes.
        if(drag_repr)
            prepare_drag_drop_operation(drag_repr, mouse_x, mouse_y);
    }
}

void EventDistributor::remove_not_under_mouse(int mouse_x, int mouse_y, double timestamp) {
    // We send mouse left events even when doing drag and drop.
    mwidgets_under_mouse.erase(std::remove_if(mwidgets_under_mouse.begin(), mwidgets_under_mouse.end(),
                   [mouse_x, mouse_y, timestamp, this] (Widget* w) -> bool {
                       Rect r = w->get_absolute_rect();
                       if(!r.contains(mouse_x, mouse_y)) {
                               distribute_mouse_event(w, MouseEvent::Left, timestamp,
                                                      mouse_x, mouse_y, 0, true);
                           return true;
                       }
                       return false;
                   }), mwidgets_under_mouse.end());
    if(mdrag_repr) {
        // Remove any widgets that are not under the drag repr. anymore, sending drag left events.
        mwidgets_under_drag.erase(std::remove_if(mwidgets_under_drag.begin(), mwidgets_under_drag.end(),
                       [mouse_x, mouse_y, timestamp, this] (Widget* w) -> bool {
                           Rect r = w->get_absolute_rect();
                           if(!r.contains(mouse_x, mouse_y)) {
                               distribute_dragdrop_event(w, DragDropEvent::Left, timestamp,
                                                         mouse_x, mouse_y, 0, mdrag_repr, true);
                               if(w == mdrag_repr->target_widget())
                                   mdrag_repr->_set_target_widget(nullptr); // target lost
                               return true;
                           }
                           return false;
                       }), mwidgets_under_drag.end());
    }
}

void EventDistributor::register_mouse_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                              double timestamp)
{
    for(Widget* umw = widget; umw != nullptr; umw = umw->parent()) {
        if(!is_widget_in_list(umw, mwidgets_under_mouse)) {
            mwidgets_under_mouse.push_back(umw);
            distribute_mouse_event(umw, MouseEvent::Entered, timestamp,
                                   mouse_x, mouse_y, button, true);
        }
    }
}

void EventDistributor::register_drag_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                             DragRepresentation *drag_repr, double timestamp)
{
    for(Widget* umw = widget; umw != nullptr; umw = umw->parent()) {
        if(!is_widget_in_list(umw, mwidgets_under_drag)) {
            mwidgets_under_drag.push_back(umw);
            if(distribute_dragdrop_event(umw, DragDropEvent::Entered, timestamp,
                                         mouse_x, mouse_y, button, drag_repr, true)) {
                // We found a widget that wants the drag.
                // FIXME: break?
                mdrag_repr->_set_target_widget(umw);
            }
        }
    }
}

void EventDistributor::reregister_under_mouse(bool do_dd, bool send_move)
{
    int mouse_x = mlast_mouse_pos.x(), mouse_y = mlast_mouse_pos.y();
    Widget* under_mouse = get_widget_at(mouse_x, mouse_y);
    if(under_mouse) {
        // Never send mouse entered (or moved) while drag is active.
        if(!mdrag_repr) {
            register_mouse_entered(under_mouse, mouse_x, mouse_y, 0, mlast_timestamp);
            if (send_move) {
                distribute_mouse_event(under_mouse, MouseEvent::Moved, mlast_timestamp,
                                       mouse_x, mouse_y, 0);
            }
        }
        else if(do_dd) {
            // If a drag is active, only send events if explicitely requested.
            // FIXME: do_dd flag redundant?
            register_drag_entered(under_mouse, mouse_x, mouse_y, mlast_mouse_dragged_button,
                                  mdrag_repr, mlast_timestamp);
            if(send_move && mdrag_repr->target_widget()) {
                distribute_dragdrop_event(mdrag_repr->target_widget(), DragDropEvent::Moved, mlast_timestamp,
                                          mouse_x, mouse_y, mlast_mouse_dragged_button, mdrag_repr, true);
            }
            // Since mouse pos hasn't actually changed, drag representation doesn't need to be moved.
        }
    }
}

// Only called after we've been asked to by GUI::set_under_mouse_invalid().
void EventDistributor::update_under_mouse()
{
    remove_not_under_mouse(mlast_mouse_pos.x(), mlast_mouse_pos.y(), mlast_timestamp);
    reregister_under_mouse(true, true);
}


void EventDistributor::prepare_drag_drop_operation(DragRepresentation* drag_repr, int mouse_x, int mouse_y)
{
    ASSERT(drag_repr);
    drag_repr->_set_pos(Position(mouse_x, mouse_y) - drag_repr->hotspot());
    mdrag_repr = drag_repr;
    mdragged_widget = nullptr;
}

// Normal way to end a drap-drog-operation contained in this method.
// Note it can also be cancelled if the source widget is somehow lost in the middle of a drag-drop op.
void EventDistributor::finish_drag_drop_operation(int mouse_x, int mouse_y, int button, double timestamp)
{
    ASSERT(mdrag_repr);
    Widget* src = mdrag_repr->source_widget();
    Widget* target = mdrag_repr->target_widget();
    if(target) {
        distribute_dragdrop_event(target, DragDropEvent::Dropped, timestamp,
                                  mouse_x, mouse_y, button, mdrag_repr, true);
    }
    if(src) {
        distribute_dragdrop_event(src, DragDropEvent::DragEnded, timestamp,
                                  mouse_x, mouse_y, button, mdrag_repr, true);
        // Also distribute a mouse-released event even if the mouse was released elsewhere to
        // keep the pressed-released symmetry for the source widget.
        distribute_mouse_event(src, MouseEvent::Released, timestamp,
                               mouse_x, mouse_y, button, true);
    }
    // Distribute remaining left events: drag leaves everyone in list that is not target.
    // FIXME: Unsure whether to do this, since it's dropped already.
    for(Widget* w : mwidgets_under_drag) {
        if(w != target)
            distribute_dragdrop_event(w, DragDropEvent::Left, timestamp,
                                      mouse_x, mouse_y, 0, mdrag_repr, true);
    }
    mwidgets_under_drag.clear();

    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;

    // Also distribute mouse-left events to every widget still in list. (Do this?)
    for(Widget* w : mwidgets_under_mouse) {
        distribute_mouse_event(w, MouseEvent::Left, timestamp,
                               mouse_x, mouse_y, 0, true);
    }
    mwidgets_under_mouse.clear();
    // Now finally update under-mouse list according to current position.
    reregister_under_mouse(false, true);
}

// Terminate the drag prematurely (e.g. when source has become invalid).
void EventDistributor::abort_drag(bool send_events, bool send_dd_end_to_gone_src) {
    if (!mdrag_repr)
        return;
    Widget* src = mdrag_repr->source_widget();
    if (src) {
        if(send_dd_end_to_gone_src) {
            distribute_dragdrop_event(src, DragDropEvent::DragEnded, mlast_timestamp, mlast_mouse_pos.x(),
                                      mlast_mouse_pos.y(), mlast_mouse_dragged_button, mdrag_repr, true);
            distribute_mouse_event(src, MouseEvent::Released, mlast_timestamp, mlast_mouse_pos.x(),
                                   mlast_mouse_pos.y(), mlast_mouse_dragged_button, true);
        }
        mdrag_repr->_clear_source_widget();
    }
    // A d&d operation ending means the under mouse / under drag buffers are flushed.
    if(send_events) {
        for(Widget* w : mwidgets_under_drag) {
                distribute_dragdrop_event(w, DragDropEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                          mlast_mouse_pos.y(), 0, mdrag_repr, true);
        }

        for(Widget* w : mwidgets_under_mouse) {
            distribute_mouse_event(w, MouseEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                   mlast_mouse_pos.y(), 0, true);
        }
    }
    mwidgets_under_drag.clear();
    mwidgets_under_mouse.clear();
    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;
}


// This function will return ONE widget that is considered to be under the mouse.
Widget* EventDistributor::get_widget_at(int x, int y)
{
    if(mmodal_widget) {
        int rel_x = x - mmodal_widget->pos_x(),
                rel_y = y - mmodal_widget->pos_y();
        Widget* target = Widget::get_leaf_widget_at_recursive(mmodal_widget, rel_x, rel_y);
        if(!target)
            target = mmodal_widget;
        return target;
    }

    if(mtop_widget) {
        int rel_x = x - mtop_widget->pos_x(),
            rel_y = y - mtop_widget->pos_y();
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

bool EventDistributor::is_widget_in_list(const Widget* w, const std::vector<Widget*>& list)
{
    return std::find(list.begin(), list.end(), w) != list.end();
}

void EventDistributor::clear_under_mouse_and_drag()
{
    for (Widget* w : mwidgets_under_mouse) {
        distribute_mouse_event(w, MouseEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                               mlast_mouse_pos.y(), 0, true);
    }
    mwidgets_under_mouse.clear();

    for (Widget* w : mwidgets_under_drag) {
        distribute_dragdrop_event(w, DragDropEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                  mlast_mouse_pos.y(), 0, mdrag_repr, true);
    }
    mwidgets_under_drag.clear();

    if(mdrag_repr) {
        mdrag_repr->_set_target_widget(nullptr);
        Widget* src = mdrag_repr->source_widget();
        if(src)
            abort_drag(true, true);
    }
}

void EventDistributor::remove_widget_and_children_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                            bool send_dd_end_to_gone_src,
                                                                            bool invert_predicate)
{
    std::function<bool (Widget*w)> predicate;
    if (!invert_predicate) {
        predicate = [widget](Widget*w) { return w->is_child_of_recursive(widget); };
    }
    else {
        predicate = [widget](Widget*w) { return !w->is_child_of_recursive(widget); };
    }

    mwidgets_under_mouse.erase(std::remove_if(mwidgets_under_mouse.begin(), mwidgets_under_mouse.end(),
                   [this, send_events, &predicate] (Widget* w) -> bool {
                       if (predicate(w)) {
                           if(send_events) {
                               distribute_mouse_event(w, MouseEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                                      mlast_mouse_pos.y(), 0, true);
                           }
                           return true;
                       }
                       return false;
                   }), mwidgets_under_mouse.end());

    // FIXME: We don't do anything on mdragged_widget as the mouse button is probably still pressed and
    //        we'd mess up events if we did. Should hardly occur anyway (?).

    mwidgets_under_drag.erase(std::remove_if(mwidgets_under_drag.begin(), mwidgets_under_drag.end(),
                   [this, send_events, &predicate] (Widget* w) -> bool {
                        if (predicate(w)) {
                            if(send_events) {
                                distribute_dragdrop_event(w, DragDropEvent::Left, mlast_timestamp,
                                                          mlast_mouse_pos.x(), mlast_mouse_pos.y(), 0,
                                                          mdrag_repr, true);
                            }
                            return true;
                        }
                        return false;
                   }), mwidgets_under_drag.end());

    if(mdrag_repr) {
        // Target has got the "left"-event already if events are to be sent.
        if(mdrag_repr->target_widget() && predicate(mdrag_repr->target_widget()))
            mdrag_repr->_set_target_widget(nullptr);

        Widget* src = mdrag_repr->source_widget();
        if(src && predicate(src)) {
            // Source has become invalid.
            abort_drag(send_events, send_dd_end_to_gone_src);
        }
    }
}

void EventDistributor::_handle_widget_invisible_or_inactive(Widget& widget) {
    remove_widget_and_children_from_under_mouse_and_drag(&widget, true, true);
    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);
    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;
    reregister_under_mouse(true, true);
}

void EventDistributor::_handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed)
{

    remove_widget_and_children_from_under_mouse_and_drag(&widget, !going_to_be_destroyed, false);

    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);

    if(mlast_mouse_pressed_on == &widget)
        mlast_mouse_pressed_on = nullptr;

    if(mmodal_widget == &widget)
        mmodal_widget = nullptr;

}

void EventDistributor::_handle_modal_focus_changed()
{
    Widget* modal_w = mfocus_mngr.modal_focus_widget();
    if(modal_w != nullptr) {
        remove_widget_and_children_from_under_mouse_and_drag(modal_w, true, true, true);
        reregister_under_mouse(true, true);
        if(mlast_mouse_pressed_on && !mlast_mouse_pressed_on->is_child_of_recursive(modal_w))
            mlast_mouse_pressed_on = nullptr;
    }
    else {
        // Modal focus released: do a complete update of mouse queue.
        clear_under_mouse_and_drag();
        reregister_under_mouse(true, false);
    }
}

DragRepresentation* EventDistributor::distribute_mouse_event(Widget* target, MouseEvent::Type type,
                                                             double timestamp, int abs_x, int abs_y,
                                                             int button, bool to_target_only)
{
    if(target) {
        Position target_abs_pos = target->get_absolute_position();
        Position rel_mouse_pos = Position(abs_x, abs_y) - target_abs_pos;
        MouseEvent me(timestamp, mmodifiers_status, type, rel_mouse_pos.x(), rel_mouse_pos.y(), button);
        Widget* w = target;
        // Bubble up.
        while(w != nullptr) {
            if((w->is_active() && w->is_visible()) || to_target_only) {
                if(w->send_mouse_event(me) || to_target_only) {
                    // Return drag object if widget has started a drag.
                    return me.drag_representation();
                }
            }
            if(w->parent()) {
                rel_mouse_pos = w->map_to_parent(rel_mouse_pos);
                me._set_pos(rel_mouse_pos);
            }
            w = w->parent();
        }
    }
    return nullptr;
}

bool EventDistributor::distribute_dragdrop_event(Widget* target, DragDropEvent::Type type, double timestamp,
                                                 int abs_x, int abs_y, int button,
                                                 DragRepresentation *drag_repr, bool to_target_only)
{
    if(target) {
        Position target_abs_pos = target->get_absolute_position();
        Position rel_mouse_pos = Position(abs_x, abs_y) - target_abs_pos;
        DragDropEvent de(timestamp, mmodifiers_status, type, rel_mouse_pos.x(), rel_mouse_pos.y(), button,
                         drag_repr);
        Widget* w = target;
        // Bubble up.
        while(w != nullptr) {
            if((w->is_active() && w->is_visible()) || to_target_only) {
                if(w->send_dragdrop_event(de) || to_target_only) {
                    return de.is_drag_accepted();
                }
            }
            if(w->parent()) {
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
    while(fw != nullptr) {
        if(fw->is_visible() && fw->is_active()) {
            if(fw->send_key_event(event))
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
