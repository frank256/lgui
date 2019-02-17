#include <algorithm>
#include "mousetrackhelper.h"
#include "dragdropevent.h"
#include "mouseevent.h"
#include "vector_utils.h"

namespace lgui {
namespace dtl {

void MouseTrackHelper::register_mouse_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                              double timestamp)
{
    for (Widget* umw = widget; umw != nullptr; umw = umw->parent()) {
        if (!contains(mwidgets_under_mouse, umw)) {
            mwidgets_under_mouse.push_back(umw);
            mdistr.distribute_mouse_event(umw, MouseEvent::Entered, timestamp,
                                          mouse_x, mouse_y, button, true);
        }
    }
}

void MouseTrackHelper::register_drag_entered(Widget* widget, int mouse_x, int mouse_y, int button,
                                             DragRepresentation* drag_repr, double timestamp)
{
    for (Widget* umw = widget; umw != nullptr; umw = umw->parent()) {
        if (!contains(mwidgets_under_drag, umw)) {
            mwidgets_under_drag.push_back(umw);
            if (mdistr.distribute_dragdrop_event(umw, DragDropEvent::Entered, timestamp,
                                                 mouse_x, mouse_y, button, drag_repr, true)) {
                // We found a widget that wants the drag.
                // FIXME: break?
                drag_repr->_set_target_widget(umw);
            }
        }
    }

    if (drag_repr->target_widget()) {
        mdistr.distribute_dragdrop_event(drag_repr->target_widget(), DragDropEvent::Moved, timestamp,
                                         mouse_x, mouse_y, mlast_mouse_dragged_button, drag_repr, true);
    }
}

void MouseTrackHelper::reregister_under_mouse(Widget* under_mouse, bool do_dd, bool send_move)
{
    int mouse_x = mlast_mouse_pos.x(), mouse_y = mlast_mouse_pos.y();
    if (under_mouse) {
        // Never send mouse entered (or moved) while drag is active.
        if (!mdrag_repr) {
            register_mouse_entered(under_mouse, mouse_x, mouse_y, 0, mlast_timestamp);
            if (send_move) {
                mdistr.distribute_mouse_event(under_mouse, MouseEvent::Moved, mlast_timestamp,
                                              mouse_x, mouse_y, 0);
            }
        }
        else if (do_dd) {
            // If a drag is active, only send events if explicitely requested.
            // FIXME: do_dd flag redundant?
            register_drag_entered(under_mouse, mouse_x, mouse_y, mlast_mouse_dragged_button,
                                  mdrag_repr, mlast_timestamp);
            if (send_move && mdrag_repr->target_widget()) {
                mdistr.distribute_dragdrop_event(mdrag_repr->target_widget(), DragDropEvent::Moved, mlast_timestamp,
                                                 mouse_x, mouse_y, mlast_mouse_dragged_button, mdrag_repr, true);
            }
            // Since mouse pos hasn't actually changed, drag representation doesn't need to be moved.
        }
    }
}

void MouseTrackHelper::remove_not_under_mouse(int mouse_x, int mouse_y, double timestamp)
{
    // We send mouse left events even when doing drag and drop.
    mwidgets_under_mouse.erase(std::remove_if(mwidgets_under_mouse.begin(), mwidgets_under_mouse.end(),
                                              [mouse_x, mouse_y, timestamp, this](Widget* w) -> bool {
                                                  Rect r = w->get_absolute_rect();
                                                  if (!r.contains(mouse_x, mouse_y)) {
                                                      mdistr.distribute_mouse_event(w, MouseEvent::Left, timestamp,
                                                                                    mouse_x, mouse_y, 0, true);
                                                      return true;
                                                  }
                                                  return false;
                                              }), mwidgets_under_mouse.end());
    if (mdrag_repr) {
        // Remove any widgets that are not under the drag repr. anymore, sending drag left events.
        mwidgets_under_drag.erase(std::remove_if(mwidgets_under_drag.begin(), mwidgets_under_drag.end(),
                                                 [mouse_x, mouse_y, timestamp, this](Widget* w) -> bool {
                                                     Rect r = w->get_absolute_rect();
                                                     if (!r.contains(mouse_x, mouse_y)) {
                                                         mdistr.distribute_dragdrop_event(w, DragDropEvent::Left,
                                                                                          timestamp,
                                                                                          mouse_x, mouse_y, 0,
                                                                                          mdrag_repr, true);
                                                         if (w == mdrag_repr->target_widget())
                                                             mdrag_repr->_set_target_widget(nullptr); // target lost
                                                         return true;
                                                     }
                                                     return false;
                                                 }), mwidgets_under_drag.end());
    }
}

void MouseTrackHelper::prepare_drag_drop_operation(DragRepresentation* drag_repr, int mouse_x, int mouse_y)
{
    ASSERT(drag_repr);
    drag_repr->_set_pos(Position(mouse_x, mouse_y) - drag_repr->hotspot());
    mdrag_repr = drag_repr;
}

// Normal way to end a drap-drog-operation contained in this method.
// Note it can also be cancelled if the source widget is somehow lost in the middle of a drag-drop op.
void MouseTrackHelper::finish_drag_drop_operation(int mouse_x, int mouse_y, int button, double timestamp)
{
    ASSERT(mdrag_repr);
    Widget* src = mdrag_repr->source_widget();
    Widget* target = mdrag_repr->target_widget();
    if (target) {
        mdistr.distribute_dragdrop_event(target, DragDropEvent::Dropped, timestamp,
                                         mouse_x, mouse_y, button, mdrag_repr, true);
    }
    if (src) {
        mdistr.distribute_dragdrop_event(src, DragDropEvent::DragEnded, timestamp,
                                         mouse_x, mouse_y, button, mdrag_repr, true);
        // Also distribute a mouse-released event even if the mouse was released elsewhere to
        // keep the pressed-released symmetry for the source widget.
        mdistr.distribute_mouse_event(src, MouseEvent::Released, timestamp,
                                      mouse_x, mouse_y, button, true);
    }
    // Distribute remaining left events: drag leaves everyone in list that is not target.
    // FIXME: Unsure whether to do this, since it's dropped already.
    for (Widget* w : mwidgets_under_drag) {
        if (w != target)
            mdistr.distribute_dragdrop_event(w, DragDropEvent::Left, timestamp,
                                             mouse_x, mouse_y, 0, mdrag_repr, true);
    }
    mwidgets_under_drag.clear();

    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;

    // Also distribute mouse-left events to every widget still in list. (Do this?)
    for (Widget* w : mwidgets_under_mouse) {
        mdistr.distribute_mouse_event(w, MouseEvent::Left, timestamp,
                                      mouse_x, mouse_y, 0, true);
    }
    mwidgets_under_mouse.clear();
}

// Terminate the drag prematurely (e.g. when source has become invalid).
void MouseTrackHelper::abort_drag(bool send_events, bool send_dd_end_to_gone_src)
{
    if (!mdrag_repr)
        return;
    Widget* src = mdrag_repr->source_widget();
    if (src) {
        if (send_dd_end_to_gone_src) {
            mdistr.distribute_dragdrop_event(src, DragDropEvent::DragEnded, mlast_timestamp, mlast_mouse_pos.x(),
                                             mlast_mouse_pos.y(), mlast_mouse_dragged_button, mdrag_repr, true);
            mdistr.distribute_mouse_event(src, MouseEvent::Released, mlast_timestamp, mlast_mouse_pos.x(),
                                          mlast_mouse_pos.y(), mlast_mouse_dragged_button, true);
        }
        mdrag_repr->_clear_source_widget();
    }
    // A d&d operation ending means the under mouse / under drag buffers are flushed.
    if (send_events) {
        for (Widget* w : mwidgets_under_drag) {
            mdistr.distribute_dragdrop_event(w, DragDropEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                             mlast_mouse_pos.y(), 0, mdrag_repr, true);
        }

        for (Widget* w : mwidgets_under_mouse) {
            mdistr.distribute_mouse_event(w, MouseEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                          mlast_mouse_pos.y(), 0, true);
        }
    }
    mwidgets_under_drag.clear();
    mwidgets_under_mouse.clear();
    // Get rid of drag representation.
    delete mdrag_repr;
    mdrag_repr = nullptr;
}

void MouseTrackHelper::clear_under_mouse_and_drag()
{
    for (Widget* w : mwidgets_under_mouse) {
        mdistr.distribute_mouse_event(w, MouseEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                      mlast_mouse_pos.y(), 0, true);
    }
    mwidgets_under_mouse.clear();

    for (Widget* w : mwidgets_under_drag) {
        mdistr.distribute_dragdrop_event(w, DragDropEvent::Left, mlast_timestamp, mlast_mouse_pos.x(),
                                         mlast_mouse_pos.y(), 0, mdrag_repr, true);
    }
    mwidgets_under_drag.clear();

    if (mdrag_repr) {
        mdrag_repr->_set_target_widget(nullptr);
        Widget* src = mdrag_repr->source_widget();
        if (src)
            abort_drag(true, true);
    }
}

void MouseTrackHelper::remove_widget_and_children_from_under_mouse_and_drag(Widget* widget, bool send_events,
                                                                            bool send_dd_end_to_gone_src,
                                                                            bool invert_predicate)
{
    std::function<bool(Widget* w)> predicate;
    if (!invert_predicate) {
        predicate = [widget](Widget* w) { return w->is_child_of_recursive(widget); };
    }
    else {
        predicate = [widget](Widget* w) { return !w->is_child_of_recursive(widget); };
    }

    mwidgets_under_mouse.erase(std::remove_if(mwidgets_under_mouse.begin(), mwidgets_under_mouse.end(),
                                              [this, send_events, &predicate](Widget* w) -> bool {
                                                  if (predicate(w)) {
                                                      if (send_events) {
                                                          mdistr.distribute_mouse_event(w, MouseEvent::Left,
                                                                                        mlast_timestamp,
                                                                                        mlast_mouse_pos.x(),
                                                                                        mlast_mouse_pos.y(), 0, true);
                                                      }
                                                      return true;
                                                  }
                                                  return false;
                                              }), mwidgets_under_mouse.end());

    // FIXME: We don't do anything on mdragged_widget as the mouse button is probably still pressed and
    //        we'd mess up events if we did. Should hardly occur anyway (?).

    mwidgets_under_drag.erase(std::remove_if(mwidgets_under_drag.begin(), mwidgets_under_drag.end(),
                                             [this, send_events, &predicate](Widget* w) -> bool {
                                                 if (predicate(w)) {
                                                     if (send_events) {
                                                         mdistr.distribute_dragdrop_event(w, DragDropEvent::Left,
                                                                                          mlast_timestamp,
                                                                                          mlast_mouse_pos.x(),
                                                                                          mlast_mouse_pos.y(), 0,
                                                                                          mdrag_repr, true);
                                                     }
                                                     return true;
                                                 }
                                                 return false;
                                             }), mwidgets_under_drag.end());

    if (mdrag_repr) {
        // Target has got the "left"-event already if events are to be sent.
        if (mdrag_repr->target_widget() && predicate(mdrag_repr->target_widget()))
            mdrag_repr->_set_target_widget(nullptr);

        Widget* src = mdrag_repr->source_widget();
        if (src && predicate(src)) {
            // Source has become invalid.
            abort_drag(send_events, send_dd_end_to_gone_src);
        }
    }
}

bool MouseTrackHelper::is_under_mouse(const Widget& widget) const
{
    return contains(mwidgets_under_mouse, &const_cast<Widget&>(widget)); // FIX this.
}

}

}