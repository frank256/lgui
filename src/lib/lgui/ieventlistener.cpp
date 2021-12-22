/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#include "ieventlistener.h"

namespace lgui {

    void IEventListener::mouse_pressed(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_released(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_moved(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_dragged(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_entered(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_left(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_clicked(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_wheel_up(MouseEvent& event)
    { (void) event; }

    void IEventListener::mouse_wheel_down(MouseEvent& event)
    { (void) event; }

    void IEventListener::timer_ticked(const TimerTickEvent& event)
    { (void) event; }

    void IEventListener::focus_gained(FocusEvent& event)
    { (void) event; }

    void IEventListener::focus_lost(FocusEvent& event)
    { (void) event; }

    void IEventListener::modal_focus_gained(FocusEvent& event)
    { (void) event; }

    void IEventListener::modal_focus_lost(FocusEvent& event)
    { (void) event; }

    void IEventListener::drag_entered(DragDropEvent& event)
    { (void) event; }

    void IEventListener::drag_moved(DragDropEvent& event)
    { (void) event; }

    void IEventListener::drag_left(DragDropEvent& event)
    { (void) event; }

    void IEventListener::dropped(DragDropEvent& event)
    { (void) event; }

    void IEventListener::drag_ended(DragDropEvent& event)
    { (void) event; }

}

