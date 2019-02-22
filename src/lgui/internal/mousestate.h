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

#ifndef LGUI_MOUSESTATETRACKER_H
#define LGUI_MOUSESTATETRACKER_H

#include "lgui/lgui.h"

namespace lgui {

namespace dtl {

class MouseState {
    public:
        MouseState()
            : mdragged_button(0), mpressed_button(0),
              mtimestamp(0.0) {}

        const Position& pos() const { return mpos; }
        void set_pos(const Position& pos)
        {
            mpos = pos;
        }

        int dragged_button() const { return mdragged_button; }
        void set_dragged_button(int dragged_button)
        {
            mdragged_button = dragged_button;
        }

        double timestamp() const { return mtimestamp; }
        void set_timestamp(double timestamp) { mtimestamp = timestamp; }

        int pressed_button() const { return mpressed_button; }
        void set_pressed_button(int pressed_button)
        {
            mpressed_button = pressed_button;
        }

    private:
        Position mpos;
        int mdragged_button, mpressed_button;
        double mtimestamp;
};


}

}


#endif //LGUI_MOUSESTATETRACKER_H
