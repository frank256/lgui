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

#ifndef LGUI_EVENTHANDLER_H
#define LGUI_EVENTHANDLER_H

#include <vector>

#include "lgui/widget.h"
#include "lgui/platform/events.h"
#include "mousetrackhelper.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "lgui/dragdropevent.h"
#include "focusmanager.h"
#include "lgui/timertickevent.h"
#include "eventdistributor.h"
#include "dragdroptrackhelper.h"
#include "eventhandlerbase.h"
#include "mousehandler.h"
#include "timerhandler.h"

namespace lgui {

namespace dtl {

using TopWidget = Widget;

/**
 * Class used internally to process external events.
 */
class EventHandler : public EventHandlerBase {
    public:
        explicit EventHandler(GUI& gui);

        void push_external_event(const ExternalEvent& event);

        const DragRepresentation* drag_representation() const;

        void update_under_mouse() { mmouse_handler.update_under_mouse(); }

        bool does_tab_move_focus() const { return mtab_moves_focus; }
        void set_tab_moves_focus(bool tmf) { mtab_moves_focus = tmf; }

        void _handle_widget_invisible_or_inactive(Widget& widget);
        void _handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed);
        void _handle_modal_focus_changed() override;
        void _subscribe_to_timer_ticks(Widget& w) { mtimer_handler.subscribe_to_timer_ticks(w); }
        void _unsubscribe_from_timer_ticks(Widget& w) { mtimer_handler.unsubscribe_from_timer_ticks(w); }
    private:
        void handle_key_event(KeyEvent::Type type, const ExternalEvent& event);
        void before_top_widget_changes() override;
        void after_top_widget_changed() override;

        EventDistributor mdistr;
        MouseHandler mmouse_handler;
        TimerHandler mtimer_handler;

        bool mtab_moves_focus;
};

}

}

#endif // LGUI_EVENTDISTRIBUTOR_H
