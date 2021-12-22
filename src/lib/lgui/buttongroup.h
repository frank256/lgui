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

#ifndef LGUI_BUTTONGROUP_H
#define LGUI_BUTTONGROUP_H

#include <vector>
#include "lgui/signal.h"

namespace lgui {

class AbstractButton;

/** A button group to keep track of e.g. radio buttons: only one button can be checked at the time.
 *  You need to add buttons to a group via add_button(). The group provides signals to see which of its
 *  buttons have been activated. */
class ButtonGroup {
    public:
        ButtonGroup();
        virtual ~ButtonGroup();

        Signal<AbstractButton*> on_button_activated;
        Signal<int> on_button_activated_id;

        void add_button(AbstractButton* button);
        void remove_button(AbstractButton* button);
        void remove_all_buttons();

        AbstractButton* checked_button() { return mchecked_button; }
        const AbstractButton* checked_button() const { return mchecked_button; }

        int checked_button_id() const;

        void clear_checked_button();

        /** Check a particular button. Returns false when the button could not be found in the group. */
        bool check_button(AbstractButton* button);
        /** Check a particular button using its ID. Returns false when the button could not be found in
         *  the group.  */
        bool check_button_id(int id);

        virtual void _button_checked(AbstractButton* button);
        virtual void _button_activated(AbstractButton* button);

    protected:

        std::list<AbstractButton*> mbuttons;
        AbstractButton* mchecked_button;
};

}

#endif // LGUI_BUTTONGROUP_H


