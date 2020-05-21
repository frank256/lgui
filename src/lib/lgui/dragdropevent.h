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

#ifndef LGUI_DRAGDROPEVENT_H
#define LGUI_DRAGDROPEVENT_H

#include "inputevent.h"
#include "lgui_types.h"
#include "lgui/platform/error.h"
#include <string>

namespace lgui {

    class DragRepresentation;

    /** Events sent during drag and drop operations. A widget can decide whether it will accept a drag during
     *  processing the `Entered` event. If it sets set_accept_drag(true), it may receive a `Dropped` event
     *  afterwards. You cannot refuse a dropped event after you've called set_accept_drag(true).
     *
     *  `Entered` and `Left` are currently dispatched according to mouse cursor position only. Large
     *  `DragReprentation`s may overlap the widget long before a drag `Entered` event is actually sent.
     *
     *  The widget from which the drag originated will receive a `DragEnded` event after the destination
     *  widget has received a `Dropped` event. It will also receive a `mouseReleased` event.
     *
     *  @note: For events of type Left and DragEnded, the position is always set to (0, 0).
     *         They usually occur outside the widget anyway.
     */
    class DragDropEvent : public InputEvent
    {
        public:
            enum Type { Entered, Moved, Left, Dropped, DragEnded };

            DragDropEvent(Type type, double timestamp, Point p, int button, DragRepresentation* drag_repr)
            : InputEvent(timestamp, 0), mtype(type), mpos(p),
              mbutton(button), mdrag_repr(drag_repr), maccept_drag(false)
            {}

            DragDropEvent(Type type, double timestamp, int button, DragRepresentation* drag_repr)
            : InputEvent(timestamp, 0), mtype(type), mpos{0, 0},
              mbutton(button), mdrag_repr(drag_repr), maccept_drag(false)
            {}

            DragDropEvent(Type type, double timestamp, DragRepresentation* drag_repr)
            : InputEvent(timestamp, 0), mtype(type), mpos{0, 0},
              mbutton(0), mdrag_repr(drag_repr), maccept_drag(false)
            {}

            Type type() const { return mtype; }
            int x() const { return mpos.x(); }
            int y() const { return mpos.y(); }
            int button() const { return mbutton; }
            Position pos() const { return mpos; }
            void set_pos(Position p) { mpos = p; }
            DragRepresentation& drag_representation()
                { ASSERT(mdrag_repr); return *mdrag_repr; }

            // FIXME keep this?
            void set_accept_drag(bool accept) { maccept_drag = accept; }
            bool is_drag_accepted() const { return maccept_drag; }

            std::string to_string() const; // debug purposes

        private:
            Type mtype;
            Position mpos;
            int mbutton;
            DragRepresentation *mdrag_repr;
            bool maccept_drag;
    };

}

#endif // LGUI_DRAGDROPEVENT_H
