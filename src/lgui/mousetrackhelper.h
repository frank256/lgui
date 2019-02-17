#ifndef MOUSETRACKHELPER_H
#define MOUSETRACKHELPER_H

#include "widget.h"
#include <vector>
#include "dragrepresentation.h"
#include "eventdistributor.h"

namespace lgui {
namespace dtl {

class MouseTrackHelper {
    public:
        explicit MouseTrackHelper(EventDistributor& distr)
        : mdistr(distr), mdrag_repr(nullptr), mlast_mouse_pos(0, 0), mlast_mouse_dragged_button(0),
          mlast_timestamp(0.0) {}

        void update_last_timestamp(double timestamp) { mlast_timestamp = timestamp; }
        double last_timestamp() const { return mlast_timestamp; }

        bool is_under_mouse(const Widget& widget) const;

        const Position& last_mouse_pos() const { return mlast_mouse_pos; }

        void set_last_mouse_pos(int mouse_x, int mouse_y) { mlast_mouse_pos.set(mouse_x, mouse_y); }

        int last_mouse_dragged_button() const { return mlast_mouse_dragged_button; }

        void set_last_mouse_dragged_button(int last_mouse_dragged_button)
        {
            mlast_mouse_dragged_button = last_mouse_dragged_button;
        }

        DragRepresentation* drag_representation() { return mdrag_repr; }

        const DragRepresentation* drag_representation() const { return mdrag_repr; }

        void remove_not_under_mouse(int mouse_x, int mouse_y, double timestamp);
        void register_mouse_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                    double timestamp);
        void register_drag_entered(Widget* widget, int mouse_x, int mouse_y, int button, DragRepresentation* drag_repr,
                                   double timestamp);

        void reregister_under_mouse(Widget* under_mouse, bool do_dd, bool send_move);

        void prepare_drag_drop_operation(DragRepresentation* drag_repr, int mouse_x, int mouse_y);
        void finish_drag_drop_operation(int mouse_x, int mouse_y, int button, double timestamp);
        void abort_drag(bool send_events, bool send_dd_end_to_gone_src);

        void clear_under_mouse_and_drag();
        void remove_subtree_from_under_mouse_and_drag(Widget* widget, bool send_events, bool send_dd_end_to_gone_src);
        void remove_all_except_subtree_from_under_mouse_and_drag(Widget* widget, bool send_events, bool send_dd_end_to_gone_src);


    private:
        void remove_widget_and_children_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                  bool send_dd_end_to_gone_src,
                                                                  bool invert_predicate = false);


        EventDistributor& mdistr;
        std::vector<Widget*> mwidgets_under_mouse, mwidgets_under_drag;
        DragRepresentation* mdrag_repr;

        lgui::Position mlast_mouse_pos;
        int mlast_mouse_dragged_button;
        double mlast_timestamp;
};

}
}

#endif // MOUSETRACKHELPER_H
