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

#ifndef LGUI_EVENTDISTRIBUTOR_H
#define LGUI_EVENTDISTRIBUTOR_H

#include "../mouseevent.h"
#include "../widget.h"
#include "../dragdropevent.h"
#include "widgettraversalstack.h"

namespace lgui {

namespace dtl {

class EventDistributor {

    public:
        explicit EventDistributor(FocusManager& mfocus_mngr)
            : mfocus_mngr(mfocus_mngr), mmodifiers_status(0) {}

        DragRepresentation* distribute_mouse_event(const WidgetTreeTraversalStack& traversal_stack,
                                                                     MouseEvent&& event) const;
        DragRepresentation* send_mouse_event(Widget* target, MouseEvent&& event) const;
        DragRepresentation* send_mouse_event_abs_pos(Widget* target, Point abs_pos, MouseEvent&& event) const;

        bool send_dragdrop_event(Widget* target, DragDropEvent&& event) const;
        bool send_dragdrop_event_abs_pos(Widget* target, Point abs_pos, DragDropEvent&& event) const;

        void send_drag_move_event_to_target_widget(const WidgetTreeTraversalStack& traversal_stack, Position mouse_pos,
                                                   double timestamp, int button, DragRepresentation* drag_repr) const;

        bool distribute_key_event(KeyEvent&& event);

        void set_modifiers_status(int modifiers_status) { mmodifiers_status = modifiers_status; }

    private:
        FocusManager& mfocus_mngr;
        int mmodifiers_status;
};

}
}

#endif //LGUI_EVENTDISTRIBUTOR_H
