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

#ifndef LGUI_EVENTHANDLER_H
#define LGUI_EVENTHANDLER_H

#include "platform/events.h"
#include <vector>

#include "lgui/widget.h"
#include "platform/events.h"
#include "mousetrackhelper.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "lgui/dragdropevent.h"
#include "focusmanager.h"
#include "lgui/timertickevent.h"
#include "eventdistributor.h"
#include "dragdroptrackhelper.h"

namespace lgui {

namespace dtl {

using TopWidget = Widget;

/** Class used internally to deliver events to widgets.
 */
class EventHandler {
    public:
        explicit EventHandler(GUI& gui);
        ~EventHandler();
        void push_external_event(const ExternalEvent& event);

        const DragRepresentation* drag_representation() const { return mdrag_drop_tracker.drag_representation(); }
        Widget* focus_widget();
        TopWidget* top_widget() { return mtop_widget; } // used by focus manager
        Widget* modal_widget() { return mmodal_widget; } // used by focus manager

        bool _request_modal_widget(Widget& w);
        bool _release_modal_widget(Widget& w);

        void _subscribe_to_timer_ticks(Widget& w);
        void _unsubscribe_from_timer_ticks(Widget& w);

        void update_under_mouse();
        void set_top_widget(TopWidget* top);

        bool does_tab_move_focus() const { return mtab_moves_focus; }
        void set_tab_moves_focus(bool tmf) { mtab_moves_focus = tmf; }

        void _handle_modal_focus_changed();
        void _handle_widget_invisible_or_inactive(Widget& widget);
        void _handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed);
    private:
        void handle_key_event(KeyEvent::Type type, const ExternalEvent& event);
        void handle_mouse_pressed(const ExternalEvent& event);
        void handle_mouse_released(const ExternalEvent& event);
        void handle_mouse_moved(const ExternalEvent& event);
        void handle_mouse_wheel(const ExternalEvent& event);
        void handle_timer_tick(const ExternalEvent& event);

        void handle_mouse_moved_dragdrop(Position mouse_pos, double timestamp);
        void handle_mouse_moved_dragging(Position mouse_pos, double timestamp);
        void handle_mouse_moved_normal(Position mouse_pos, double timestamp);

        void reregister_under_mouse(bool do_dd, bool send_move);

        Widget* get_widget_at(Position pos);

        GUI& mgui;
        FocusManager mfocus_mngr;
        EventDistributor mdistr;
        MouseState mlast_mouse_state;
        MouseTrackHelper mmouse_tracker;
        DragDropTrackHelper mdrag_drop_tracker;

        std::vector <Widget*> mwidgets_subscribed_to_timer_ticks, mwidgets_timer_ticks_subscriptions_queue;

        Widget* mtop_widget, *mmodal_widget;
        Widget* mlast_mouse_pressed_on, *mdragged_widget;
        bool mtab_moves_focus, mdistributing_timer_ticks;
};

}

}

#endif // LGUI_EVENTDISTRIBUTOR_H
