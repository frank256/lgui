/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

#ifndef LGUI_ABSTRACTBUTTON_H
#define LGUI_ABSTRACTBUTTON_H

#include "lgui/widget.h"
#include "lgui/signal.h"

namespace lgui {

    class ButtonGroup;

    /** An abstract button, base class for all buttons.
     *
     *  An AbstractButton combines two behaviors: push buttons and checkable buttons.
     *
     *  A push button is a button that will simply emit a signal when pushed.
     *  It doesn't care about other buttons and knows only three states: Standard, Hovered, Down.
     *  For convenience, the abstract button provides two signals:  on_activated and on_activated_src.
     *  The latter will send a pointer to the button-self along. Both are emitted and you can connect to the
     *  one you prefer.
     *
     *  A checkable button has an additional checked state which is independent from the other states
     *  (hovered, down). You can tell an AbstractButton to behave like a checkable button by calling
     *  set_checkable(true). It will then emit the on_checked_changed signals, but will also continue to emit
     *  the on_activated and on_activated_src signals.
     *
     *  A checkable button may belong to a ButtonGroup that will ensure only one of them is checked at the
     *  time. Note that you are supposed to add the buttons to a %ButtonGroup using the methods of ButtonGroup.
     *  An AbstractButton can only belong to one group at the same time.
     *
     *  Every AbstractButton has a padding property and an integer id.
     *
     *  @todo: id should be mapped to the buttons in ButtonGroup
     */
    class AbstractButton : public Widget
    {
        public:
            AbstractButton();

            explicit AbstractButton(const Padding& padding)
                : AbstractButton() { mpadding = padding; }

            explicit AbstractButton(Padding&& padding)
                : AbstractButton() { mpadding = padding; }

            ~AbstractButton() override;

            void draw(const DrawEvent& de) const override = 0;

            Signal <> on_activated;
            Signal <AbstractButton *> on_activated_src;
            Signal <bool /* checked */> on_checked_changed;
            Signal <> on_down;
            Signal <> on_up;

            /** Return whether the button is currently down (pressed). */
            bool is_down() const { return mdown; }

            int id() const { return mid; }
            void set_id(int id) { mid = id; }

            /** Return whether the button is checkable. */
            bool is_checkable() const { return mcheckable; }

            /** Change whether the button is checkable. This is a property that
             *  should only be once set in a constructor of a subclass. */
            void set_checkable(bool checkable) {
                mcheckable = checkable;
            }

            /** Return whether the button is checked. */
            bool is_checked() const { return mchecked; }

            /** Change whether the button is checked. */
            void set_checked(bool checked);

            /** Return the button group the button belongs to, if any.
                @see ButtonGroup  */
            ButtonGroup *button_group() { return mgroup; }
            const ButtonGroup *button_group() const { return mgroup; }
            /** Change the button group the button belongs to. You don't want
             *  to call this normally, but rather call ButtonGroup::add_button(). */
            void _set_button_group(ButtonGroup *group) { mgroup = group; }

        protected:
            Padding mpadding;

            virtual void activated();
            virtual void change_checked_due_to_input();

            void mouse_entered(MouseEvent& event) override;
            void mouse_left(MouseEvent& event) override;
            void mouse_clicked(MouseEvent& event) override;
            void mouse_pressed(MouseEvent& event) override;
            void mouse_released(MouseEvent& event) override;

            void key_pressed(KeyEvent& event) override;
            void key_released(KeyEvent& event) override;

            void focus_lost(FocusEvent& event) override;
        private:
            void down();
            void up();

            int mkey_pressed, mid;
            bool mdown, mleft_pressed, mchecked, mcheckable;
            ButtonGroup *mgroup;
    };

}

#endif // LGUI_ABSTRACTBUTTON_H
