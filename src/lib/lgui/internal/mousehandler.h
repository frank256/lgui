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

#ifndef LGUI_MOUSEHANDLER_H
#define LGUI_MOUSEHANDLER_H

#include "lgui/platform/events.h"
#include "mousestate.h"
#include "mousetrackhelper.h"
#include "dragdroptrackhelper.h"


namespace lgui {

namespace dtl {

class EventHandlerBase;

class MouseHandler {
    public:
        MouseHandler(EventHandlerBase& event_handler_base, EventDistributor& event_distributor)
                : mevent_handler_base(event_handler_base),
                  mdistr(event_distributor),
                  mmouse_tracker(event_distributor, mlast_mouse_state),
                  mdrag_drop_tracker(event_distributor, mlast_mouse_state),
                  mlast_mouse_pressed_on(nullptr), mdragged_widget(nullptr) {}

        const DragRepresentation* drag_representation() const { return mdrag_drop_tracker.drag_representation(); };

        void handle_mouse_pressed(const ExternalEvent& event);
        void handle_mouse_released(const ExternalEvent& event);
        void handle_mouse_moved(const ExternalEvent& event);
        void handle_mouse_wheel(const ExternalEvent& event);
        void update_under_mouse();
        void set_last_timestamp(double timestamp);
        void reregister_under_mouse(const WidgetTreeTraversalStack& traversal_stack, bool do_dd, bool send_move);
        void reregister_under_mouse(bool do_dd, bool send_move);

        void _handle_widget_invisible_or_inactive(Widget& widget);
        void _handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed);
        void _handle_modal_focus_changed();
        void before_top_widget_changes();

    private:
        void handle_mouse_moved_dragdrop(const WidgetTreeTraversalStack& traversal_stack, Position mouse_pos,
                                         double timestamp);
        void handle_mouse_moved_dragging(const WidgetTreeTraversalStack& traversal_stack, Position mouse_pos,
                                         double timestamp);
        void handle_mouse_moved_normal(const WidgetTreeTraversalStack& traversal_stack, double timestamp);
        DragRepresentation* send_dragged_event_to_dragged_widget(const WidgetTreeTraversalStack& traversal_stack,
                                                                 const Position& mouse_pos, double timestamp) const;

        EventHandlerBase& mevent_handler_base;
        EventDistributor& mdistr;
        MouseState mlast_mouse_state;
        MouseTrackHelper mmouse_tracker;
        DragDropTrackHelper mdrag_drop_tracker;

        Widget* mlast_mouse_pressed_on, * mdragged_widget;
};

}

}


#endif //LGUI_MOUSEHANDLER_H
