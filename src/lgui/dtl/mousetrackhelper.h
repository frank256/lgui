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
            : mdistr(distr), mlast_mouse_state(mouse_state_tracker) {}

        bool is_under_mouse(const Widget& widget) const;

        void remove_not_under_mouse(Position mouse_pos, double timestamp);
        void register_mouse_entered(Widget* widget, Position mouse_pos, int button, double timestamp);

        void reregister_under_mouse(Widget* under_mouse, bool send_move);
        void clear_under_mouse(bool send_events=true);

        void remove_subtree_from_under_mouse(Widget* widget, bool send_events);
        void remove_all_except_subtree_from_under_mouse(Widget* widget, bool send_events);

    private:
        void remove_widget_and_children_from_under_mouse(Widget* widget, bool send_events, bool invert_predicate = false);

        EventDistributor& mdistr;
        const MouseState& mlast_mouse_state;
        std::vector<Widget*> mwidgets_under_mouse;

};

}
}

#endif // MOUSETRACKHELPER_H
