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

//#define lgui_DEBUG_ENABLE_RTTI


#include "eventfilter.h"

#include "lgui/platform/error.h"

#include "mouseevent.h"
#include "keyevent.h"
#include "dragdropevent.h"
#include "focusevent.h"

#ifdef LGUI_DEBUG_ENABLE_RTTI
#include <typeinfo>
#endif

#include "widget.h"

namespace lgui {

bool EventFilter::mouse_event(Widget& w, MouseEvent& event) {
    (void) w;
    (void) event;
    return true;
}

bool EventFilter::key_event(Widget& w, KeyEvent& event) {
    (void) w;
    (void) event;
    return true;
}

bool EventFilter::focus_event(Widget& w, FocusEvent& event) {
    (void) w;
    (void) event;
    return true;
}

bool EventFilter::dragdrop_event(Widget& w, DragDropEvent& event) {
    (void) w;
    (void) event;
    return true;
}


bool DebugEventFilter::mouse_event(Widget& w, MouseEvent& event) {
    if (mdont_dump_move && (event.type() == MouseEvent::Moved ||
                            event.type() == MouseEvent::Dragged))
        return true;
#ifdef lgui_DEBUG_ENABLE_RTTI
    debug("Widget %p (%s) received %s\n", w, typeid(*w).name(), event.to_string().c_str());
#else
    debug("Widget %p received %s\n", &w, event.to_string().c_str());
#endif
    return true;
}

bool DebugEventFilter::key_event(Widget& w, KeyEvent& event) {
#ifdef lgui_DEBUG_ENABLE_RTTI
    debug("Widget %p (%s) received %s\n", w, typeid(*w).name(), event.to_string().c_str());
#else
    debug("Widget %p received %s\n", &w, event.to_string().c_str());
#endif
    return true;
}

bool DebugEventFilter::focus_event(Widget& w, FocusEvent& event) {
#ifdef lgui_DEBUG_ENABLE_RTTI
    debug("Widget %p (%s) received %s\n", w, typeid(*w).name(), event.to_string().c_str());
#else
    debug("Widget %p received %s\n", &w, event.to_string().c_str());
#endif
    return true;
}

bool DebugEventFilter::dragdrop_event(Widget& w, DragDropEvent& event) {
    if (mdont_dump_move && event.type() == DragDropEvent::Moved)
        return true;
#ifdef lgui_DEBUG_ENABLE_RTTI
    debug("Widget %p (%s) received %s\n", w, typeid(*w).name(), event.to_string().c_str());
#else
    debug("Widget %p received %s\n", &w, event.to_string().c_str());
#endif
    return true;
}

}
