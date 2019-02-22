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

#include "focusmanager.h"

#include "platform/error.h"

#include "lgui/widget.h"
#include "lgui/focusevent.h"

#include "eventhandlerbase.h"
#include "lgui/basiccontainer.h" // for TopWidget

namespace lgui {
namespace dtl {

FocusManager::FocusManager(dtl::EventHandlerBase& handler)
    : mfocus_widget(nullptr), mmodal_focus_widget(nullptr),
      mhandler(handler) {}


void FocusManager::add(Widget& widget)
{
    mwidgets.push_back(&widget);
}

void FocusManager::remove(Widget& widget)
{
    ASSERT(know_widget(widget));

    if (mmodal_focus_widget == &widget)
        release_modal_focus(widget);
    if (mfocus_widget == &widget)
        focus_none();
    for (auto it = mwidgets.begin(); it != mwidgets.end(); ++it) {
        if (*it == &widget) {
            mwidgets.erase(it);
            break;
        }
    }
}

bool FocusManager::is_parent_of_focus_widget(const Widget* w) const
{
    if (mfocus_widget)
        return mfocus_widget->is_child_of_recursive(w);
    else
        return false;
}

bool FocusManager::request_focus(Widget& widget, bool steal_if_not_focusable)
{
    if (mfocus_widget == &widget)
        return true;
    ASSERT(know_widget(widget));
    if (!is_child_of_top_or_modal_widget(&widget))
        return false;
    if (mmodal_focus_widget) {
        if (!is_modal_focus_child(&widget)) {
            warning("Requesting focus for a widget that is no modal child");
            return false;
        }
    }
    if (!widget.is_focusable() && !steal_if_not_focusable)
        return false;
    if (mfocus_widget)
        send_focus_event(mfocus_widget, FocusEvent::FocusLost);
    if (!widget.is_focusable()) {
        //focus stolen, but can't give it to new widget
        mfocus_widget = nullptr;
        return false;
    }
    mfocus_widget = &widget;
    send_focus_event(&widget, FocusEvent::FocusGained);
    return true;
}

void FocusManager::focus_none()
{
    if (mfocus_widget) {
        send_focus_event(mfocus_widget, FocusEvent::FocusLost);
        mfocus_widget = nullptr;
    }
}

bool FocusManager::is_parent_of_modal_focus_widget(const Widget* w) const
{
    if (mmodal_focus_widget)
        return mmodal_focus_widget->is_child_of_recursive(w);
    else
        return false;
}

bool FocusManager::is_modal_focus_child(const Widget* w) const
{
    ASSERT(w);
    if (!mmodal_focus_widget)
        return false;
    while (w != nullptr) {
        if (w == mmodal_focus_widget)
            return true;
        w = w->parent();
    }
    return false;
}

bool FocusManager::request_modal_focus(Widget& w)
{
    ASSERT(know_widget(w));
    if (!is_child_of_top_or_modal_widget(&w))
        return false;
    if (mmodal_focus_widget != &w) {
        if (mmodal_focus_widget) {
            if (is_modal_focus_child(&w)) {
                send_focus_event(mmodal_focus_widget, FocusEvent::ModalFocusLost);
                mmodal_focus_widget = nullptr;
            }
            else {
                // FIXME: is that correct?
                warning("Requesting modal focus while it is being held by another non-parent widget!");
                return false;
            }
        }
        mmodal_focus_widget = &w;
        signal_modal_focus_changed();
        send_focus_event(mmodal_focus_widget, FocusEvent::ModalFocusGained);
        if (mfocus_widget != mmodal_focus_widget)
            request_focus(*mmodal_focus_widget, true);
    }
    return true;
}

bool FocusManager::release_modal_focus(Widget& widget)
{
    ASSERT(know_widget(widget));
    if (&widget == mmodal_focus_widget) {
        if (mfocus_widget && is_modal_focus_child(mfocus_widget))
            focus_none();
        send_focus_event(mmodal_focus_widget, FocusEvent::ModalFocusLost);
        mmodal_focus_widget = nullptr;
        signal_modal_focus_changed();
        return true;
    }
    else {
        warning("Trying to release modal focus from wrong widget!");
        return false;
    }
}

void FocusManager::widget_became_inactive(Widget& w)
{
    if (is_parent_of_focus_widget(&w))
        focus_none();
    if (is_parent_of_modal_focus_widget(&w))
        release_modal_focus(*mmodal_focus_widget);
}

void FocusManager::widget_became_invisible(Widget& w)
{
    widget_became_inactive(w);
}

bool FocusManager::know_widget(const Widget& widget) const
{
    for (const Widget* w : mwidgets) {
        if (&widget == w)
            return true;
    }
    return false;
}

bool FocusManager::tab_move_focus(bool rev)
{
    if (mfocus_widget && !mfocus_widget->may_tab_out_of())
        return false;
    Widget* w = find_next_widget_ready_for_tab_focus(mfocus_widget, rev);
    if (w)
        return request_focus(*w);
    else
        return false;
}


Widget* FocusManager::find_next_widget_ready_for_tab_focus(Widget* start, bool backwards)
{
    if (mwidgets.empty())
        return nullptr;
    // first find start widget index (FIXME: save index of focus widget?)
    int idx = -1;
    if (start) {
        for (int i = 0; i < signed(mwidgets.size()); i++) {
            if (mwidgets[i] == start) {
                idx = i;
                break;
            }
        }
        ASSERT(idx >= 0);
    }
    else {
        start = mwidgets[0]; // exists as empty() is false
        idx = 0;
    }
    Widget* w = start;
    int n = signed(mwidgets.size()); // we try each widget once
    do {
        if (backwards)
            idx--;
        else
            idx++;
        if (idx < 0)
            idx = signed(mwidgets.size()) - 1;
        else if (idx >= signed(mwidgets.size()))
            idx = 0;
        w = mwidgets[idx];
        n--;
    } while (n > 0 && !may_widget_receive_tab_focus(*w));
    if (w != start)
        return w;
    else
        return nullptr;
}

bool FocusManager::may_widget_receive_tab_focus(const Widget& w)
{
    if (!(w.is_focusable() && w.may_tab_into()))
        return false;
    if (!is_child_of_top_or_modal_widget(&w))
        return false;
    if (mmodal_focus_widget && !is_modal_focus_child(&w))
        return false;

    const Widget* p = &w;
    while (p != nullptr) {
        if (!p->is_visible() || !p->is_active())
            return false;
        p = p->parent();
    }
    return true;
}

// Not really needed.
bool FocusManager::is_child_of_top_or_modal_widget(const Widget* w)
{
    if (mhandler.modal_widget())
        return w->is_child_of_recursive(mhandler.modal_widget());
    else if (mhandler.top_widget())
        return w->is_child_of_recursive(mhandler.top_widget());
    return false;
}

void FocusManager::signal_modal_focus_changed()
{
    mhandler._handle_modal_focus_changed();
}

void FocusManager::send_focus_event(Widget* w, FocusEvent::Type type)
{
    FocusEvent fe(type);
    w->send_focus_event(fe);
}

}

}