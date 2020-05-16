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

#ifndef LGUI_EVENTHANDLERBASE_H
#define LGUI_EVENTHANDLERBASE_H

#include <lgui/widget.h>
#include "focusmanager.h"

namespace lgui {

class GUI;

namespace dtl {

using TopWidget = Widget;

/** Wraps the core of a GUI state; the most important method provided is get_widget_at. */
class EventHandlerBase {
    public:

        TopWidget* top_widget() { return mtop_widget; }
        Widget* modal_widget() { return mmodal_widget; }
        Widget* modal_focus_widget();
        Widget* focus_widget();

        Widget* get_widget_at(Point pos);

        bool _request_modal_widget(Widget& w);
        bool _release_modal_widget(Widget& w);
        void set_top_widget(TopWidget* top);
        virtual void _handle_modal_focus_changed() = 0;

    protected:
        explicit EventHandlerBase(GUI& gui);
        virtual ~EventHandlerBase();

        void reset_modal_widget() { mmodal_widget = nullptr; }

        virtual void before_top_widget_changes() = 0;
        virtual void after_top_widget_changed() = 0;

        FocusManager mfocus_mngr;
    private:
        GUI& mgui;
        Widget* mtop_widget, *mmodal_widget;
};

}
}

#endif //LGUI_EVENTHANDLERBASE_H
