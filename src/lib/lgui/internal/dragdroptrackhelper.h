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

#ifndef LGUI_DRAGDROPTRACKHELPER_H
#define LGUI_DRAGDROPTRACKHELPER_H

#include "lgui/widget.h"
#include "lgui/dragrepresentation.h"
#include "eventdistributor.h"
#include "mousestate.h"

#include <vector>

namespace lgui {
namespace dtl {

class DragDropTrackHelper {
    public:
        explicit DragDropTrackHelper(EventDistributor& distr, const MouseState& last_mouse_state)
            : mdistr(distr), mlast_mouse_state(last_mouse_state), mdrag_repr(nullptr) {}

        DragRepresentation* drag_representation() { return mdrag_repr; }
        const DragRepresentation* drag_representation() const { return mdrag_repr; }

        void remove_not_under_drag(Position mouse_pos, double timestamp);
        void register_drag_entered(Widget* widget, Position mouse_pos, int button, double timestamp);


        void prepare_drag_drop_operation(DragRepresentation* drag_repr, Position mouse_pos);
        void finish_drag_drop_operation(Position mouse_pos, int button, double timestamp);
        void clear_under_drag();
        void abort_drag(bool send_events, bool send_dd_end_to_gone_src);

        void remove_subtree_from_under_drag(Widget* widget, bool send_events,
                                                                 bool send_dd_end_to_gone_src);
        void remove_all_except_subtree_from_under_drag(Widget* widget, bool send_events,
                                                                            bool send_dd_end_to_gone_src);

        void reregister_drag(Widget* under_mouse, bool send_move);

    private:
        void remove_widget_and_children_from_under_drag(Widget* widget, bool send_events,
                                                                             bool send_dd_end_to_gone_src,
                                                                             bool invert_predicate);

        EventDistributor& mdistr;
        const MouseState& mlast_mouse_state;
        std::vector<Widget*> mwidgets_under_drag;
        DragRepresentation* mdrag_repr;
};

}
}


#endif //LGUI_DRAGDROPTRACKHELPER_H
