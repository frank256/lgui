/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#include "../events.h"


#ifdef __ANDROID__
#define USE_MOUSE_EMULATION
#endif

namespace lgui {

bool convert_a5_event(const ALLEGRO_EVENT &al_ev, ExternalEvent &event)
{
    event.timestamp = al_ev.any.timestamp;
    switch(al_ev.type) {
#ifdef USE_MOUSE_EMULATION
        case ALLEGRO_EVENT_TOUCH_MOVE:
           if(!al_ev.touch.primary)
              return false;
           event.mouse.x = al_ev.touch.x;
           event.mouse.y = al_ev.touch.y;
           event.mouse.dx = al_ev.touch.dx;
           event.mouse.dy = al_ev.touch.dy;
           event.type = ExternalEvent::EVENT_MOUSE_MOVED;
           break;
        case ALLEGRO_EVENT_TOUCH_BEGIN:
           if(!al_ev.touch.primary)
              return false;
           event.mouse.x = al_ev.touch.x;
           event.mouse.y = al_ev.touch.y;
           event.type = ExternalEvent::EVENT_MOUSE_PRESSED;
           break;
        case ALLEGRO_EVENT_TOUCH_END:
           if(!al_ev.touch.primary)
              return false;
           event.mouse.x = al_ev.touch.x;
           event.mouse.y = al_ev.touch.y;
           event.type = ExternalEvent::EVENT_MOUSE_RELEASED;
           break;
#endif
        case ALLEGRO_EVENT_MOUSE_AXES:
           /* Mousewheel has priority */
           if(al_ev.mouse.dz != 0) {
               event.type = ExternalEvent::EVENT_MOUSEWHEEL;
               event.mouse.dz = al_ev.mouse.dz;
               event.mouse.z = al_ev.mouse.z;
           }
           else if(al_ev.mouse.dw != 0) {
              event.type = ExternalEvent::EVENT_MOUSE_FOURTH_AXIS;
              event.mouse.dw = al_ev.mouse.dw;
              event.mouse.w = al_ev.mouse.w;
           }
           else {
               event.type = ExternalEvent::EVENT_MOUSE_MOVED;
           }
           event.mouse.x = al_ev.mouse.x;
           event.mouse.y = al_ev.mouse.y;
           event.mouse.dx = al_ev.mouse.dx;
           event.mouse.dy = al_ev.mouse.dy;
           break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
           event.type = ExternalEvent::EVENT_REQUEST_EXIT;
           break;

        case ALLEGRO_EVENT_TIMER:
           event.type = ExternalEvent::EVENT_TIMER_TICK;
           event.timer.count = al_ev.timer.count;
           break;

        case ALLEGRO_EVENT_KEY_CHAR:
           // this "doubling" is due to the fact we use a traditional enum in a namespace
           // so that we can convert to int implicitly
           event.key.code = al_ev.keyboard.keycode;
           event.key.modifiers = al_ev.keyboard.modifiers;
           event.key.unichar = al_ev.keyboard.unichar;
           event.type = ExternalEvent::EVENT_KEY_CHAR;
           event.key.repeated = al_ev.keyboard.repeat;
           break;

        case ALLEGRO_EVENT_KEY_UP:
           event.key.code = al_ev.keyboard.keycode;
           event.key.modifiers = al_ev.keyboard.modifiers;
           event.type = ExternalEvent::EVENT_KEY_RELEASED;
           break;

        case ALLEGRO_EVENT_KEY_DOWN:
           event.key.code = al_ev.keyboard.keycode;
           event.key.modifiers = al_ev.keyboard.modifiers;
           event.type = ExternalEvent::EVENT_KEY_PRESSED;
           break;


        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            event.type = ExternalEvent::EVENT_MOUSE_RELEASED;
            event.mouse.button = al_ev.mouse.button;
            event.mouse.x = al_ev.mouse.x;
            event.mouse.y = al_ev.mouse.y;
           break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
           event.type = ExternalEvent::EVENT_MOUSE_PRESSED;
           event.mouse.button = al_ev.mouse.button;
           event.mouse.x = al_ev.mouse.x;
           event.mouse.y = al_ev.mouse.y;
           break;
        case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
           event.type = ExternalEvent::EVENT_HALT_DRAWING;
            break;

        case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
           event.type = ExternalEvent::EVENT_RESUME_DRAWING;
            break;

        case ALLEGRO_EVENT_DISPLAY_RESIZE:
           event.type = ExternalEvent::EVENT_DISPLAY_RESIZE;
            break;

        default:
           return false;

    }
    return true;
}

double get_time()
{
    return al_get_time();
}

}


