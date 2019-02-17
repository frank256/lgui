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

#ifndef LGUI_FOCUSMANAGER_H
#define LGUI_FOCUSMANAGER_H

#include <vector>
#include "platform/error.h"
#include "lgui/focusevent.h"

namespace lgui {
class Widget;

namespace dtl {

class EventHandler;


/** Class managing focus. Normally, you won't have to deal with those, as EventDistributor will create one
 *  internally. */
class FocusManager {
    public:
        explicit FocusManager(dtl::EventHandler* distr);

        void add(Widget& widget);
        void remove(Widget& widget);

        Widget* focus_widget() { return mfocus_widget; }
        const Widget* focus_widget() const { return mfocus_widget; }
        bool is_focused(const Widget& widget) const
        {
            return mfocus_widget == &widget;
        }
        bool is_parent_of_focus_widget(const Widget* w) const;

        bool request_focus(Widget& widget, bool steal_if_not_focusable = false);
        void focus_none();

        Widget* modal_focus_widget() { return mmodal_focus_widget; }
        const Widget* modal_focus_widget() const { return mmodal_focus_widget; }

        bool has_modal_focus(const Widget& widget) const
        {
            return mmodal_focus_widget == &widget;
        }
        bool is_parent_of_modal_focus_widget(const Widget* w) const;
        bool is_modal_focus_child(const Widget* w) const;

        bool request_modal_focus(Widget& w);
        bool release_modal_focus(Widget& widget);

        void widget_became_inactive(Widget& w);
        void widget_became_invisible(Widget& w);

        bool know_widget(const Widget& widget) const;

        // return true if focus could be moved to another widget
        bool tab_move_focus(bool rev);

    private:
        Widget* find_next_widget_ready_for_tab_focus(Widget* start, bool backwards);
        bool may_widget_receive_tab_focus(const Widget& w);

        bool is_child_of_top_or_modal_widget(const Widget* w);
        void signal_modal_focus_changed();

        static void send_focus_event(Widget* w, FocusEvent::Type type);

        Widget* mfocus_widget, * mmodal_focus_widget;
        std::vector<Widget*> mwidgets;
        dtl::EventHandler* mdistr;
};

}
}

#endif // LGUI_FOCUSMANAGER_H
