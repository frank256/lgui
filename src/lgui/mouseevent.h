#ifndef LGUI_MOUSEEVENT_H
#define LGUI_MOUSEEVENT_H

#include "inputevent.h"
#include "lgui.h"
#include "platform/error.h"
#include <string>

namespace lgui {

    class DragRepresentation;

    /** A mouse event. When a the mouse cursor enters or leaves a widget's rectangle, the widget will receive
     *  `Entered` and `Left` events. When the widget is moved or resized, this status is not updated until
     *  the mouse is actually moved again. You can call Widget::invalidate_under_mouse() to request an update
     *  anyway.
     *
     *  If the (same) mouse buttons remain pressed while moving, the widgets gets a `Dragged` instead of a
     *  `Moved` event. The widget may decide to react upon a drag event by calling spawn_drag() with a
     *  DragRepresentation object that represents the object being dragged in a drag and drop operation.
     *
     *  @see Widget::invalidate_under_mouse(), spawn_drag(), DragRepresentation
     */
    class MouseEvent : public InputEvent
    {
        public:
            enum Type { Moved, Pressed, Released,
                        Entered, Left, Clicked, Dragged, WheelUp, WheelDown };

            MouseEvent(double timestamp, int modifiers, Type type, int x, int y, int button)
                : InputEvent(timestamp, modifiers), mtype(type), mpos(x, y), mbutton(button),
                  mdrag_repr(nullptr)
            {}

            MouseEvent(double timestamp, int modifiers, Type type, Position p, int button)
                : InputEvent(timestamp, modifiers), mtype(type), mpos(p), mbutton(button),
                  mdrag_repr(nullptr)
            {}

            Type type() const { return mtype; }
            int x() const { return mpos.x(); }
            int y() const { return mpos.y(); }
            Position pos() const { return mpos; }
            int button() const { return mbutton; }

            /** Call this with an object created with new. The GUI *will take
             *  ownership* of the object (!), so *don't* destroy it.
             *  You shouldn't do much else after you've called this.
             *  When the drag ended (successful or not, i.e. with or without
             *  ending in a drop), the source widget will receive a dragEnded
             *  event.
             *
             *  @todo Use a unique_ptr here? - Don't want to depend on \<memory\>
             *        though
             */
            void spawn_drag(DragRepresentation *drag_repr)
            {
                ASSERT(mtype == Clicked || mtype == Dragged);
                mdrag_repr = drag_repr;
            }

            bool has_drag_been_spawned() const
            { return mdrag_repr != nullptr; }

            DragRepresentation *drag_representation() { return mdrag_repr; }

            std::string to_string() const;

            /** Internal. */
            void _set_pos(Position p) { mpos = p; }

        private:
            Type mtype;
            Position mpos;
            int mbutton;
            DragRepresentation *mdrag_repr;
    };

}

#endif // LGUI_MOUSEEVENT_H
