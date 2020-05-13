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

#include <algorithm>

#include "gui.h"

#include "dragrepresentation.h"
#include "drawevent.h"
#include "lgui/platform/graphics.h"

//#define _LGUI_DBG_DRAW_FOCUS
//#define DEBUG_LAYOUT

#ifdef DEBUG_LAYOUT
#define DBG_LAYOUT(...) debug("GUI: " __VA_ARGS__)
#else
#define DBG_LAYOUT(...)
#endif

#ifdef LGUI_DEBUG_ENABLE_RTTI
#include <typeinfo>
#endif

namespace lgui {

    GUI::GUI()
        : mevent_handler(*this),
          mtop_widget(nullptr), mmodal_widget(nullptr),
          mdraw_widget_stack_start(0),
          munder_mouse_invalid(false),
          mhandling_events(false),
          mlayout_in_progress(false),
          mhandling_deferred_callbacks(false)
    {}

    void GUI::draw_widgets(Graphics& gfx)
    {
        for(unsigned int i = mdraw_widget_stack_start; i < mtop_widget_stack.size(); i++) {
            const auto& e = mtop_widget_stack[i];
            const Widget& w = *e.top_widget;
            if(w.is_visible()) {
                Widget::draw_child(w, DrawEvent(gfx, false, 1.0));
                gfx.filled_rect(w.rect(), e.cover_col);
            }
        }

        if(mtop_widget && mtop_widget->is_visible())
            Widget::draw_child(*mtop_widget, DrawEvent(gfx, false, 1.0));
        if(mmodal_widget)
            Widget::draw_child(*mmodal_widget, DrawEvent(gfx, false, 1.0));

#ifdef _LGUI_DBG_DRAW_FOCUS
        // debug: highlight focus widget
        if(mfocus_mngr.focus_widget()) {
            Rect r = mfocus_mngr.focus_widget()->get_absolute_rect();
            gfx.filled_rect(r, rgba_premult(1.0, 0.0, 1.0, 0.25));
        }
#endif
        // Draw object representing ongoing drag-operation, if there's any.
        const DragRepresentation* drag_repr = mevent_handler.drag_representation();
        if(drag_repr) {
            gfx.push_draw_area(drag_repr->rect(), true);
            drag_repr->draw(gfx);
            gfx.pop_draw_area();
        }
    }

    void GUI::push_external_event(const ExternalEvent& event)
    {
        mhandling_events = true;
        mevent_handler.push_external_event(event);
        mhandling_events = false;

        handle_deferred();
    }

    void GUI::handle_deferred()
    {
        if (mhandling_events)
            return;
        handle_deferred_actions();
        handle_relayout();
        handle_deferred_callbacks();
        if(munder_mouse_invalid) {
            mevent_handler.update_under_mouse();
            munder_mouse_invalid = false;
        }
    }

    void GUI::set_top(TopWidget* top)
    {
        mevent_handler.set_top_widget(top);
        mtop_widget = top;
    }

    void GUI::push_top_widget(TopWidget& top, bool shinethrough, Color cover_col)
    {
        if(mhandling_events) {
            mdeferred_actions.emplace_back(DeferredAction::PushTop, &top, shinethrough, cover_col);
            return;
        }
        if(mtop_widget) {
            // Modal focus not preserved.
            mtop_widget_stack.push_back({ mtop_widget, cover_col, mevent_handler.focus_widget(),
                                          mdraw_widget_stack_start });
            // Background is covered: don't need to draw other widgets
            if(!shinethrough || cover_col.a >= 0.99)
                mdraw_widget_stack_start = mtop_widget_stack.size();
        }
        set_top(&top);
        // this triggers a relayout
        _request_layout(top);
        handle_relayout();
    }

    void GUI::pop_top_widget() {
        if(!mtop_widget)
            return;
        if(mhandling_events) {
            mdeferred_actions.emplace_back(DeferredAction::PopTop);
            return;
        }

        TopWidget* old_top = mtop_widget;
        TopWidget* new_top = nullptr;

        const TopWidgetStackEntry* entry = nullptr;

        if(old_top)
            old_top->set_closed(true);

        if(!mtop_widget_stack.empty()) {
            entry = &mtop_widget_stack.back();
            new_top = entry->top_widget;
        }

        set_top(new_top);

        if(entry) {
            mdraw_widget_stack_start = entry->top_stack_start_drawing_idx;
            mtop_widget_stack.pop_back();
            _request_layout(*new_top);
            handle_relayout();
        }
    }


    void GUI::handle_deferred_actions()
    {
        ASSERT(!mhandling_events);
        for(const DeferredAction& da : mdeferred_actions) {
            switch(da.action) {
                case DeferredAction::PushTop:
                    push_top_widget(*da.top_widget, da.shinethrough, da.cover_col);
                    break;
                case DeferredAction::PopTop:
                    pop_top_widget();
                    break;
                case DeferredAction::BringToFront:
                    _bring_to_front(*da.widget);
                    break;
                case DeferredAction::SendToBack:
                    _send_to_back(*da.widget);
                    break;
            }
        }
        mdeferred_actions.clear();
    }

    void GUI::handle_relayout()
    {
        if(!mrelayout_widgets.empty()) {
            DBG_LAYOUT("################ LAYOUT BEGINS\n");
            mlayout_in_progress = true;
            for (auto relayout_widget : mrelayout_widgets) {
#ifdef LGUI_DEBUG_ENABLE_RTTI
                DBG_LAYOUT("##### LAYOUT RUNS for %p (%s)\n", relayout_widget, typeid(*relayout_widget).name());
#else
                DBG_LAYOUT("##### LAYOUT RUNS for %p\n", relayout_widget);
#endif
                relayout_widget->_relayout();
            }
            mrelayout_widgets.clear();
            DBG_LAYOUT("################ LAYOUT ENDS\n");
            mlayout_in_progress = false;
        }
    }

    void GUI::handle_deferred_callbacks()
    {
        mhandling_deferred_callbacks = true;
        for (auto& callback : mdeferred_callbacks) {
            callback();
        }
        mdeferred_callbacks.clear();
        mhandling_deferred_callbacks = false;
    }

    void GUI::_enqueue_deferred(const std::function<void ()>& callback)
    {
        if (mhandling_deferred_callbacks) {
            warning("Trying to register a deferred callback while processing deferred callbacks.");
        }
        else {
            mdeferred_callbacks.emplace_back(callback);
        }
    }


    void GUI::_bring_to_front(Widget& w)
    {
        if(mhandling_events) {
            mdeferred_actions.emplace_back(DeferredAction::BringToFront, &w);
            return;
        }
        if(w.parent())
            w.parent()->_bring_child_to_front(w);
    }

    void GUI::_send_to_back(Widget& w)
    {
        if(mhandling_events) {
            mdeferred_actions.emplace_back(DeferredAction::SendToBack, &w);
            return;
        }
        if(w.parent())
            w.parent()->_send_child_to_back(w);
    }

    void GUI::_request_layout(Widget& w)
    {
        DBG_LAYOUT("REQUEST-LAYOUT: %p %s\n", &w, mlayout_in_progress ?
                   "--> ignored, due to layout being in process. " : "queued.");
        if(!mlayout_in_progress)
            mrelayout_widgets.insert(&w);
    }

    bool GUI::_request_modal_widget(Widget& w)
    {
        bool success = mevent_handler._request_modal_widget(w);
        mmodal_widget = mevent_handler.modal_widget();
        return success;
    }

    bool GUI::_release_modal_widget(Widget& w)
    {
        bool success = mevent_handler._release_modal_widget(w);
        mmodal_widget = mevent_handler.modal_widget();
        return success;
    }

    // Used to rid our buffers of widgets that are going to be destroyed or deregistered
    // from the GUI (i.e. their parents). Called by the internal methods of Widget at the right places.
    // It's a better idea not to destroy source widgets in the middle of drag-drop operations.
    void GUI::_handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed)
    {
        mevent_handler._handle_widget_deregistered(widget, going_to_be_destroyed);

        if(mmodal_widget == &widget)
            mmodal_widget = nullptr;


        auto layout_it = mrelayout_widgets.find(&widget);
        if(layout_it != mrelayout_widgets.end())
            mrelayout_widgets.erase(layout_it);

        if(going_to_be_destroyed) {
            // This is ugly, but needs to be done.

            while(mtop_widget == &widget) {
                pop_top_widget();
            }

            mtop_widget_stack.erase(std::remove_if(mtop_widget_stack.begin(), mtop_widget_stack.end(), [&widget](const TopWidgetStackEntry& e) {
                return e.top_widget == &widget;
            }), mtop_widget_stack.end());
            mdeferred_actions.erase(std::remove_if(mdeferred_actions.begin(), mdeferred_actions.end(), [&widget](const DeferredAction& da) {
                return da.top_widget == &widget || da.widget == &widget;
            }), mdeferred_actions.end());
        }

    }

    void GUI::_handle_widget_invisible_or_inactive(Widget& widget)
    {
        mevent_handler._handle_widget_invisible_or_inactive(widget);
    }

}
