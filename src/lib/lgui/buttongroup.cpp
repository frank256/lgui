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

#include "buttongroup.h"
#include "lgui/widgets/abstractbutton.h"
#include <algorithm>

namespace lgui {

ButtonGroup::ButtonGroup()
        : mchecked_button(nullptr) {}

ButtonGroup::~ButtonGroup() {
    remove_all_buttons();
}


void ButtonGroup::add_button(AbstractButton* button) {
    if (std::find(mbuttons.begin(), mbuttons.end(), button) == mbuttons.end()) {
        mbuttons.push_back(button);
        button->_set_button_group(this);
        button->set_checkable(true);
    }
}

void ButtonGroup::remove_button(AbstractButton* button) {
    if (std::find(mbuttons.begin(), mbuttons.end(), button) != mbuttons.end()) {
        if (mchecked_button == button)
            clear_checked_button();
        mbuttons.remove(button);
        button->_set_button_group(nullptr);
    }
}

void ButtonGroup::remove_all_buttons() {
    clear_checked_button();
    for (auto& bt : mbuttons)
        bt->_set_button_group(nullptr);
    mbuttons.clear();
}

int ButtonGroup::checked_button_id() const {
    if (mchecked_button)
        return mchecked_button->id();
    else
        return -1;
}

void ButtonGroup::clear_checked_button() {
    if (mchecked_button)
        mchecked_button->set_checked(false);
    mchecked_button = nullptr;
}

bool ButtonGroup::check_button(AbstractButton* button) {
    auto it = std::find(mbuttons.begin(), mbuttons.end(), button);
    if (it != mbuttons.end()) {
        if (!button->is_checked())
            button->set_checked(true);
        return true;
    }
    return false;
}

bool ButtonGroup::check_button_id(int id) {
    auto it = std::find_if(mbuttons.begin(), mbuttons.end(),
                           [id](const AbstractButton* bt) { return bt->id() == id; });
    if (it != mbuttons.end()) {
        if (!(*it)->is_checked())
            (*it)->set_checked(true);
        return true;
    }
    return false;
}

void ButtonGroup::_button_checked(AbstractButton* button) {
    if (mchecked_button != button) {
        if (mchecked_button)
            mchecked_button->set_checked(false);
        mchecked_button = button;
        // would call us again, but we're safe because of the if-statement
        if (button && !button->is_checked())
            button->set_checked(true);
    }
}

void ButtonGroup::_button_activated(AbstractButton* button) {
    on_button_activated.emit(button);
    on_button_activated_id.emit(button->id());
}

}
