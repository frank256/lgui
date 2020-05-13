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

#ifndef LGUI_IEVENTLISTENER_H
#define LGUI_IEVENTLISTENER_H

#include "ikeylistener.h"

namespace lgui {

    class MouseEvent;
    class DragDropEvent;
    class FocusEvent;
    class TimerTickEvent;

    /** Every widget implements this. Key listener is a base class to allow for hotkeys independent from
     *  widgets.
     * @see MouseEvent, KeyEvent, FocusEvent, DragDropEvent */
    class IEventListener : public IKeyListener
    {
        public:
            IEventListener() = default;
            ~IEventListener() override = default;

            /** Called when the timer has ticked. */
            virtual void timer_ticked(const TimerTickEvent& event);

        protected:
            /** Called whenever the mouse is pressed on the widget. */
            virtual void mouse_pressed(MouseEvent& event);
            /** Called whenever the mouse is released on the widget.
                Also called when the mouse has been moved (=dragged)
                with a mouse button last pressed on the widget even after the
                mouse has left the widget. Also called on the source widget
                after a drag-and-drop operation has finished. */
            virtual void mouse_released(MouseEvent& event);
            /** Called whenever the mouse is moved over the widget without buttons
                being pressed. */
            virtual void mouse_moved(MouseEvent& event);
            /** Called whenever the mouse is moved over the widget with a
             *  button pressed that was pressed on the same widget
             *  The widget that the button was pressed on will receive
             *  these events even after the mouse has left the widget. */
            virtual void mouse_dragged(MouseEvent& event);
            /** Called whenever the mouse enters (begins to hover over)
             *  the widget. */
            virtual void mouse_entered(MouseEvent& event);
            /** Called whenever the mouse leaves (ends to hover over)
             *  the widget. */
            virtual void mouse_left(MouseEvent& event);
            /** Called when the mouse has been released on the same
             *  widget is has been pressed on. Will be sent after
             *  the mouseReleased() event. */
            virtual void mouse_clicked(MouseEvent& event);
            /** Called when the mouse wheel has been moved up. */
            virtual void mouse_wheel_up(MouseEvent& event);
            /** Called when the mouse wheel has been moved down. */
            virtual void mouse_wheel_down(MouseEvent& event);

            /** Called when the widget has gained input focus. */
            virtual void focus_gained(FocusEvent& event);
            /** Called when the widget has lost input focus. */
            virtual void focus_lost(FocusEvent& event);

            /** Called when the widget has gained modal input focus. */
            virtual void modal_focus_gained(FocusEvent& event);
            /** Called when the widget has lost modal input focus. */
            virtual void modal_focus_lost(FocusEvent& event);

            /** Called when the representation of a drag-and-drop operation
             *  enters the widget. Call DragDropEvent::set_accept_drag() to state
             *  that you want to accept the drag. */
            virtual void drag_entered(DragDropEvent& event);
            /** Called when the representation of a drag-and-drop operation
             *  moves over the widget. These are only sent to the widget
             *  that has accepted the drag on drag_entered() (the "target") */
            virtual void drag_moved(DragDropEvent& event);
            /** Called when the representation of a drag-and-drop operation
             *  leaves the widget.  */
            virtual void drag_left(DragDropEvent& event);
            /** Called when the representation of a drag-and-drop operation
             *  is dropped over the widget. These are only sent to the widget
             *  that has accepted the drag on drag_entered(). Once you've voted
             *  to accept a drag on drag_entered(), you cannot refuse it being dropped. */
            virtual void dropped(DragDropEvent& event);
            /** Sent to the source widget after the drag operation it spawned has either
                been dropped or cancelled. */
            virtual void drag_ended(DragDropEvent& event);
    };

}

#endif // LGUI_IEVENTLISTENER_H
