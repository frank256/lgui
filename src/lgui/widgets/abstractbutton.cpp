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

#include "abstractbutton.h"
#include "platform/keycodes.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"
#include "lgui/buttongroup.h"

namespace lgui {

    AbstractButton::AbstractButton()
        : mkey_pressed(0), mid(-1),
          mdown(false), mleft_pressed(false), mchecked(false),
          mcheckable(false), mgroup(nullptr)
    {
        set_focusable(true);
    }

    AbstractButton::~AbstractButton()
    {
        if(mgroup)
            mgroup->remove_button(this);
    }

    void AbstractButton::set_checked(bool checked)
    {
        if(mchecked != checked) {
            mchecked = checked;
            if(mcheckable && mgroup && mchecked)
                mgroup->_button_checked(this);
            on_checked_changed.emit(checked);
        }
    }

    void AbstractButton::mouse_entered(MouseEvent& event)
    {
        (void) event;
        if(mleft_pressed) {
            down();
            mleft_pressed = false;
        }
    }

    void AbstractButton::mouse_left(MouseEvent& event)
    {
        (void) event;
        if(mdown)
            mleft_pressed = true;
        up();
    }

    void AbstractButton::mouse_pressed(MouseEvent& event)
    {
        down();
        focus();
        // fix bug: when button "appears" under mouse
        //          without being registered by GUI,
        //          we want it to be hovered when pressed
        set_hovered(true);
        event.consume();
    }

    void AbstractButton::mouse_released(MouseEvent& event)
    {
        up();
        mleft_pressed = false;
        event.consume();
    }


    void AbstractButton::mouse_clicked(MouseEvent& event)
    {
        if(is_hovered()) {
            event.consume();
            activated();
        }
        if(mcheckable)
            change_checked_due_to_input();
    }

    void AbstractButton::key_pressed(KeyEvent& event)
    {
        int key = event.key_code();
        if((key == Keycodes::KEY_ENTER ||
           key == Keycodes::KEY_ENTER_PAD) ||
                key == Keycodes::KEY_SPACE) {
            mkey_pressed = key;
            down();
            event.consume();
        }
    }

    void AbstractButton::key_released(KeyEvent& event)
    {
        if(mkey_pressed == event.key_code() && mdown) {
            up();
            mkey_pressed = 0;
            if(mcheckable)
                change_checked_due_to_input();
            activated();
            event.consume();
        }
    }

    void AbstractButton::focus_lost(FocusEvent& event)
    {   // FIXME: do the right thing?
        (void) event;
        mkey_pressed = 0;
        up();
        set_hovered(false);
    }

    void AbstractButton::activated()
    {
         on_activated.emit();
         on_activated_src.emit(this);
         if(mgroup)
             mgroup->_button_activated(this);
    }

    void AbstractButton::change_checked_due_to_input()
    {
        set_checked(!is_checked());
    }

    void AbstractButton::down()
    {
        if (!mdown) {
            mdown = true;
            on_down.emit();
        }
    }

    void AbstractButton::up()
    {
        if (mdown) {
            mdown = false;
            on_up.emit();
        }
    }
}
