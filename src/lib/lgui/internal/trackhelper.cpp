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

#include <vector>
#include <stack>
#include <queue>
#include "trackhelper.h"
#include "lgui/widget.h"

namespace lgui {
namespace dtl {

PointF map_from_absolute(Widget* widget, Point p) {
    WidgetTreeTraversalStack stack;
    trace_back_traversal(widget, p, stack);
    return stack.topmost_widget_pos();
}

void register_widget_parents_first(std::vector<Widget*>& widgets, Widget* w) {
    std::vector<Widget*>::const_iterator dest_pos = widgets.end();
    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
        if (w->parent() == *it) {
            dest_pos = it.base();
            break;
        }
        if ((*it)->parent() == w) {
            dest_pos = it.base();
            --dest_pos;
            break;
        }
    }
    widgets.insert(dest_pos, w);
}

void trace_back_traversal(Widget* widget, Point pos, WidgetTreeTraversalStack& traversal_stack) {
    // Go up from widget to root and push path onto traversal stack - backwards.
    traversal_stack.new_backwards_traversal();
    Widget* w = widget;
    while (w) {
        traversal_stack.push_backwards(w, PointF());
        w = w->parent();
    }
    traversal_stack.backwards_traversal_finished();
    // Now go down the recorded path from root to widget again and save all local positions of pos.
    PointF p = PointF(pos);
    for (int i = 0; i < traversal_stack.get_no_entries(); ++i) {
        WidgetTreeTraversalStack::Entry& entry = traversal_stack.get(i);
        p = entry.w->map_from_parent(p);
        entry.p = p;
    }
}

}
}
