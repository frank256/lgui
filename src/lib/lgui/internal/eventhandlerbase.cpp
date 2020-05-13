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

#include "eventhandlerbase.h"
#include "lgui/gui.h"

namespace lgui {
namespace dtl {

EventHandlerBase::EventHandlerBase(GUI& gui)
: mfocus_mngr(*this), mgui(gui), mtop_widget(nullptr), mmodal_widget(nullptr)
{
}

EventHandlerBase::~EventHandlerBase()
{
    // Do this?
    if(mtop_widget)
        mtop_widget->_recursive_configure({nullptr, nullptr});
    mtop_widget = nullptr;
}

bool EventHandlerBase::_request_modal_widget(Widget& w)
{
    if(mmodal_widget || mfocus_mngr.modal_focus_widget())
        return false;
    ASSERT(w.parent() == nullptr);
    ASSERT(!w.is_added_to_gui());
    ASSERT(!mfocus_mngr.know_widget(w));
    mmodal_widget = &w;
    mmodal_widget->_recursive_configure({&mfocus_mngr, &mgui});
    mfocus_mngr.request_modal_focus(*mmodal_widget);
    return true;
}

bool EventHandlerBase::_release_modal_widget(Widget& w)
{
    if(mmodal_widget != &w)
        return false;
    mfocus_mngr.release_modal_focus(*mmodal_widget);
    mmodal_widget->_recursive_configure({nullptr, nullptr});
    mmodal_widget = nullptr;
    return true;
}

Widget*EventHandlerBase::focus_widget()
{
    return mfocus_mngr.focus_widget();
}

Widget* EventHandlerBase::modal_focus_widget()
{
    return mfocus_mngr.modal_focus_widget();
}

void EventHandlerBase::set_top_widget(TopWidget* top)
{
    before_top_widget_changes();

    if(mfocus_mngr.modal_focus_widget())
        mfocus_mngr.release_modal_focus(*mfocus_mngr.modal_focus_widget());
    mfocus_mngr.focus_none();

    if(mtop_widget)
        mtop_widget->_recursive_configure({nullptr, nullptr});

    mtop_widget = nullptr;

    if(top) {
        ASSERT(!top->is_added_to_gui());
        ASSERT(!mfocus_mngr.know_widget(*top));
        mtop_widget = top;
        Widget::ConfigInfo ci { &mfocus_mngr, &mgui };
        mtop_widget->_recursive_configure(ci);

        top->set_closed(false);
        Widget* fc = top->focus_child();
        if(!fc)
            fc = top;
        if(fc->is_focusable()) {
            // Is focus child still valid?
            if(fc->is_child_of_recursive(top))
                mfocus_mngr.request_focus(*fc);
            else
                top->set_focus_child(nullptr); // Remove invalid info
        }
        after_top_widget_changed();
    }
}


// This function will return ONE widget that is considered to be under the mouse.
Widget* EventHandlerBase::get_widget_at(Position pos)
{
    if(mmodal_widget) {
        int rel_x = pos.x() - mmodal_widget->pos_x(),
            rel_y = pos.y() - mmodal_widget->pos_y();

        Widget* target = Widget::get_leaf_widget_at_recursive(mmodal_widget, rel_x, rel_y);
        if(!target)
            target = mmodal_widget;
        return target;
    }

    if(mtop_widget) {
        int rel_x = pos.x() - mtop_widget->pos_x(),
            rel_y = pos.y() - mtop_widget->pos_y();
        Widget* target = Widget::get_leaf_widget_at_recursive(mtop_widget, rel_x, rel_y);
        if(target) {
            // Prevent non-modal-widget under mouse - modal focus widget will catch it.
            if(mfocus_mngr.modal_focus_widget()) {
                if(!mfocus_mngr.is_modal_focus_child(target))
                    return mfocus_mngr.modal_focus_widget();
            }
            return target;
        }
    }
    return nullptr;
}


}
}

