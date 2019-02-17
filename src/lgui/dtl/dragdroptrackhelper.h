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
