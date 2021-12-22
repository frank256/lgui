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

#include "layout.h"

#include "lgui/basiccontainer.h"

#include "layouttransition.h"

namespace lgui {

Layout::Layout()
        : mtarget(nullptr), mis_laying_out(false),
          mupdate_on_child_add_remove(true), mupdate_on_child_resize(true),
          mupdate_on_child_relocation(true) {

}

Layout::~Layout() {
    if (mtarget)
        mtarget->set_layout(nullptr); // will unregister all widgets
}

void Layout::layout(const Rect& r) {
    if (!mis_laying_out && mtarget) {
        mis_laying_out = true;
        do_layout(r);
        mis_laying_out = false;
    }
}

void Layout::_child_added_to_target(Widget& widget) {
    widget.add_widget_listener(this);

    if (mupdate_on_child_add_remove && mtarget)
        mtarget->request_layout();
}

void Layout::_child_removed_from_target(Widget& widget) {
    widget.remove_widget_listener(this);
    if (mupdate_on_child_add_remove && mtarget)
        mtarget->request_layout();
}

void Layout::_set_target(BasicContainer* target) {
    // we assume that we're called by Widget::set_layout
    // or the relevant subclasses, so we handle only our stuff here
    if (mtarget) {
        // retargeting a layout doesn't make much sense
        // but we should handle it anyway
        _cleanup_old_target();
    }
    mtarget = target;
    if (target) {
        // ok, now add all children, temporarily disabling
        // updating the layout in the process
        bool u = update_on_child_add_remove();
        set_update_on_child_add_remove(false);
        _new_target();
        // restore setting
        set_update_on_child_add_remove(u);
    }
}

void Layout::_new_target() {
    for (Widget* w : (*mtarget)) {
        _child_added_to_target(*w);
    }
}

void Layout::_cleanup_old_target() {
    if (mtarget) {
        for (Widget* w : (*mtarget)) {
            w->remove_widget_listener(this);
        }
    }
}

void Layout::size_changed_wl(Widget& w) {
    (void) w;
    if (mupdate_on_child_resize && is_ready_for_update(w))
        mtarget->request_layout();
}

void Layout::pos_changed_wl(Widget& w) {
    (void) w;
    if (mupdate_on_child_relocation && is_ready_for_update(w))
        mtarget->request_layout();
}

void Layout::visibility_changed_wl(Widget& w, bool gone_changed) {
    (void) w;
    if (gone_changed && is_ready_for_update(w))
        mtarget->request_layout();
}

bool Layout::is_ready_for_update(Widget& w) const {
    return !mis_laying_out && mtarget
           && (!w.layout_transition() ||
               (w.layout_transition() && !w.layout_transition()->is_transition_in_progress()));
}

}


