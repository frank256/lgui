#ifndef MOUSETRACKHELPER_H
#define MOUSETRACKHELPER_H

#include "lgui/widget.h"
#include <vector>
#include "lgui/dragrepresentation.h"
#include "eventdistributor.h"
#include "mousestate.h"

namespace lgui {
namespace dtl {

class MouseTrackHelper {
    public:
        explicit MouseTrackHelper(EventDistributor& distr, const MouseState& mouse_state_tracker)
            : mdistr(distr), mlast_mouse_state(mouse_state_tracker), mdrag_repr(nullptr) {}

        bool is_under_mouse(const Widget& widget) const;

        DragRepresentation* drag_representation() { return mdrag_repr; }

        const DragRepresentation* drag_representation() const { return mdrag_repr; }

        void remove_not_under_mouse(Position mouse_pos, double timestamp);
        void register_mouse_entered(Widget* widget, Position mouse_pos, int button,
                                    double timestamp);
        void register_drag_entered(Widget* widget, Position mouse_pos, int button, DragRepresentation* drag_repr,
                                   double timestamp);

        void reregister_under_mouse(Widget* under_mouse, bool do_dd, bool send_move);

        void prepare_drag_drop_operation(DragRepresentation* drag_repr, Position mouse_pos);
        void finish_drag_drop_operation(Position mouse_pos, int button, double timestamp);
        void abort_drag(bool send_events, bool send_dd_end_to_gone_src);

        void clear_under_mouse_and_drag();
        void remove_subtree_from_under_mouse_and_drag(Widget* widget, bool send_events, bool send_dd_end_to_gone_src);
        void remove_all_except_subtree_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                 bool send_dd_end_to_gone_src);

    private:
        void remove_widget_and_children_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                  bool send_dd_end_to_gone_src,
                                                                  bool invert_predicate = false);

        EventDistributor& mdistr;
        const MouseState& mlast_mouse_state;
        std::vector<Widget*> mwidgets_under_mouse, mwidgets_under_drag;
        DragRepresentation* mdrag_repr;

};

}
}

#endif // MOUSETRACKHELPER_H
