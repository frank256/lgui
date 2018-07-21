/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_IKEYLISTENER_H
#define LGUI_IKEYLISTENER_H

namespace lgui {

    class KeyEvent;

    /** The key listener is a separate base class of the IEventListener so it is possible to have global key
     *  listeners (e.g. for hotkeys).*/
    class IKeyListener {
        public:
            virtual ~IKeyListener() = default;

            /** Sent to a widget with input focus when a key has been pressed. */
            virtual void key_pressed(KeyEvent& event) {(void) event; }
            /** Sent to a widget with input focus when a key has been released. */
            virtual void key_released(KeyEvent& event) {(void) event; }
            /** Sent to a widget with input focus when a character has been produced by a sequence of
             *  keypresses. This might also be the result of keys being repeated. The event will send proper
             *  unicode characters that are being typed. It might be the result of a combination of keys being
             *  pressed (e.g. shift being pressed and held and 'a' pressed to produce an upper-case 'A'),
             *  whereas key_pressed() and key_released() only refer to individual "physical" keys. */
            virtual void key_char(KeyEvent& event) {(void) event; }
    };

}
#endif // LGUI_IKEYLISTENER_H

