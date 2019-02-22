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
    : EventHandlerBase(gui),
      mdistr(mfocus_mngr),
      mmouse_handler(*this, mdistr),
      mtab_moves_focus(true)
{}


void EventHandler::push_external_event(const lgui::ExternalEvent& event)
{
    // to generate events with a reasonable timestamp:
    mmouse_handler.set_last_timestamp(event.timestamp);

    switch(event.type) {
        case ExternalEvent::EVENT_MOUSE_MOVED:
            mmouse_handler.handle_mouse_moved(event);
            break;
        case ExternalEvent::EVENT_TIMER_TICK:
            mtimer_handler.handle_timer_tick(event);
            break;
        case ExternalEvent::EVENT_MOUSE_PRESSED:
            mmouse_handler.handle_mouse_pressed(event);
            break;
        case ExternalEvent::EVENT_MOUSE_RELEASED:
            mmouse_handler.handle_mouse_released(event);
            break;
        case ExternalEvent::EVENT_MOUSEWHEEL:
            mmouse_handler.handle_mouse_wheel(event);
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
    mmouse_handler._handle_widget_invisible_or_inactive(widget);

    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);
}

void EventHandler::_handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed)
{
    mmouse_handler._handle_widget_deregistered(widget, going_to_be_destroyed);

    if (widget.receives_timer_ticks())
        _unsubscribe_from_timer_ticks(widget);

    if(modal_widget() == &widget)
        reset_modal_widget();
}

void EventHandler::_handle_modal_focus_changed()
{
    mmouse_handler._handle_modal_focus_changed();
}
void EventHandler::before_top_widget_changes()
{
    mmouse_handler.before_top_widget_changes();
}

void EventHandler::after_top_widget_changed()
{
    mmouse_handler.reregister_under_mouse(false, true);
}

const DragRepresentation* EventHandler::drag_representation() const { return mmouse_handler.drag_representation(); }

}

}
