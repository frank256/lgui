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

#ifndef LGUI_EVENTS_H
#define LGUI_EVENTS_H

#include <cstdint>

namespace lgui {

/** Platform agnostic way of describing an external event. Closely resembles Allegro 5 events at the moment.
*/
struct ExternalEvent {

    enum EventType {
        EVENT_KEY_PRESSED,
        EVENT_KEY_RELEASED,
        EVENT_KEY_CHAR,
        EVENT_MOUSE_MOVED,
        EVENT_MOUSE_PRESSED,
        EVENT_MOUSE_RELEASED,
        EVENT_MOUSEWHEEL,
        EVENT_MOUSE_FOURTH_AXIS,
        EVENT_REQUEST_EXIT,
        EVENT_TIMER_TICK,
        EVENT_HALT_DRAWING,
        EVENT_RESUME_DRAWING,
        EVENT_DISPLAY_RESIZE
    };

    EventType type;
    double timestamp;
    union {
        struct {
            int code;
            int modifiers;
            int unichar;
            bool repeated;
        } key;
        struct {
            int x, y, button;
            int w, z;
            int dx, dy, dw, dz;
        } mouse;
        struct {
            int64_t count;
        } timer;
    };
};

}

#include "a5/a5events.h"

#endif // LGUI_EVENTS_H
