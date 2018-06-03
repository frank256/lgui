/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

#ifndef LGUI_EVENTDISTRIBUTOR_H
#define LGUI_EVENTDISTRIBUTOR_H

#include "platform/events.h"
#include <vector>

#include "widget.h"
#include "platform/events.h"
#include "keyevent.h"
#include "mouseevent.h"
#include "dragdropevent.h"
#include "focusmanager.h"
#include "timertickevent.h"

namespace lgui {

namespace dtl {

using TopWidget = Widget;

/** Class used internally to deliver events to widgets.
 */
class EventDistributor
{
    public:
        EventDistributor(GUI& gui);
        ~EventDistributor();
        void push_external_event(const ExternalEvent& event);

        const DragRepresentation* drag_representation() const { return mdrag_repr; }
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

        void register_mouse_entered(Widget* widget, int mouse_x, int mouse_y, int button, double timestamp);
        void register_drag_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                   DragRepresentation *drag_repr, double timestamp);
        void remove_not_under_mouse(int mouse_x, int mouse_y, double timestamp);
        void reregister_under_mouse(bool do_dd=false);
        void prepare_drag_drop_operation(DragRepresentation *drag_repr, int mouse_x, int mouse_y);
        void finish_drag_drop_operation(int mouse_x, int mouse_y, int button, double timestamp);
        void abort_drag(bool send_events, bool send_dd_end_to_gone_src);

        Widget* get_widget_at(int x, int y);

        void remove_widget_and_children_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                  bool send_dd_end_to_gone_src,
                                                                  bool invert_predicate=false);
        void clear_under_mouse_and_drag();

        bool distribute_key_event(KeyEvent& event);
        DragRepresentation* distribute_mouse_event(Widget* target, MouseEvent::Type type, double timestamp,
                                                   int abs_x, int abs_y, int button, bool to_target_only=false);
        bool distribute_dragdrop_event(Widget* target, DragDropEvent::Type type, double timestamp,
                                       int abs_x, int abs_y, int button, DragRepresentation *drag_repr,
                                       bool to_target_only=false);

        static bool is_widget_in_list(const Widget* w, const std::vector <Widget* >& list);


        GUI& mgui;
        FocusManager mfocus_mngr;

        std::vector <Widget*> mwidgets_under_mouse, mwidgets_under_drag;
        std::vector <Widget*> mwidgets_subscribed_to_timer_ticks, mwidgets_timer_ticks_subscriptions_queue;

        Widget* mtop_widget, *mmodal_widget;
        Widget* mlast_mouse_pressed_on, *mdragged_widget;
        DragRepresentation *mdrag_repr;
        Position mlast_mouse_pos;
        int mlast_mouse_pressed_button, mlast_mouse_dragged_button;
        double mlast_timestamp;
        int mmodifiers_status;
        bool mtab_moves_focus, mdistributing_timer_ticks;
};

}

}

#endif // LGUI_EVENTDISTRIBUTOR_H
