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

#ifndef LGUI_KEYEVENT_H
#define LGUI_KEYEVENT_H

#include "inputevent.h"
#include <string>
#include "lgui/platform/keycodes.h"

namespace lgui {

    /** A key event. A key that is repeated will continue to send `Char` events. Only the widget that
     *  currently has the focus will receive KeyEvents. */
    class KeyEvent : public InputEvent
    {
        public:

            enum Type { Pressed, Released, Char };

            KeyEvent(double timestamp, Type type, KeyCode key_code, int modifiers, int unichar, bool repeated)
                : InputEvent(timestamp, modifiers), mtype(type), mkey_code(key_code),
                  munichar(unichar), mrepeated(repeated)
            {}

            KeyCode key_code() const { return mkey_code; }
            Type type() const { return mtype; }
            int unichar() const { return munichar; }
            bool repeated() const { return mrepeated; }

            std::string to_string() const;

        private:
            Type mtype;
            KeyCode mkey_code;
            int munichar;
            bool mrepeated;
    };

}

#endif // LGUI_KEYEVENT_H

