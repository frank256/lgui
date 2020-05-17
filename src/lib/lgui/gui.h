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

#ifndef LGUI_GUI_H
#define LGUI_GUI_H

#include <deque>
#include <unordered_set>
#include <functional>

#include "widget.h"
#include "lgui/platform/color.h"
#include "lgui/internal/eventhandler.h"

namespace lgui {
    class Graphics;

    class DragRepresentation;
    using TopWidget = Widget;

    /** The main %GUI class. You'll need one of those. Push external events to it and it will distribute them
     *  to the widgets. Call draw_widgets() to draw the GUI. Push (and pop) top-widgets. */
    class GUI
    {
        public:
            GUI();
            ~GUI() = default;

            /** Draws the GUI. */
            void draw_widgets(Graphics& gfx);

            /** Processes the external event passed and distributes resulting
             *  events to the widgets. */
            void push_external_event(const ExternalEvent& event);

            /** This can be called to trigger processing of deferred actions (layout,
             *  pushing / popping top widgets,changing drawing order, updating under-mouse buffer).
             *  This is automatically called after processing each external event. However, there are
             *  occasions where it might be useful to manually trigger processing of these actions. This has
             *  no effect if called during handling an external event. */
             void handle_deferred();

            /** Pushes a new top widget. The current top widget, if there is any,
             *  will be pushed onto the top-widget stack and top will become the
             *  current top widget.
             *  @param top the widget to become the new top-widget
             *  @param shinethrough indicates whether the stack of previous top
             *         widgets shall be drawn behind the top widget
             *  @param cover_col If shinethrough is true, a filled rectangle of
             *         this color is drawn on top of the previous top widget.
             *         This can be used to darken the background.
             *  @note If called in response to an external event, the method
             *        merely internally queues the action to avoid confusion on the call stack.
             *        It will actually be executed later on leaving push_external_event().
             *        So don't rely on the top widget already having been changed
             *        atfer the call.
             */
            void push_top_widget(TopWidget& top, bool shinethrough=false, Color cover_col=grey_premult(0.0, 0.0));

            /** Pops the top widget. The current top widget, if there is any,
             *  will be taken out of the GUI. If there is a previous top widget
             *  on top-widget stack, it will become the new top widget.
             *  current top widget.
             *  @note If called in response to an external event, the method
             *        merely internally queues the action to avoid confusion on the call stack.
             *        It will actually be executed later on leaving push_external_event().
             *        So don't rely on the top widget already having been changed
             *        atfer the call.
             */
            void pop_top_widget();

            /** Returns the current top-widget. */
            TopWidget* top_widget() { return mtop_widget; } // used by focus manager

            //void add_global_key_listener(Widget* w);
            //void remove_global_key_listener(Widget* w);

            /** Internally called by Widget::invalidate_under_mouse(). */
            void set_under_mouse_invalid() { munder_mouse_invalid = true; }

            /** Call this to change whether non-handled tab key presses will move focus.
             *  Default is yes. */
            void set_tab_moves_focus(bool tmf) { mevent_handler.set_tab_moves_focus(tmf); }
            bool does_tab_move_focus() const { return mevent_handler.does_tab_move_focus(); }

            void print_info() { mevent_handler.print_info(); }

            // internal
            void _handle_widget_deregistered(Widget& widget, bool going_to_be_destroyed=false);
            void _handle_widget_invisible_or_inactive(Widget& w);
            void _bring_to_front(Widget& w);
            void _send_to_back(Widget& w);
            void _request_layout(Widget& w);
            bool _request_modal_widget(Widget& w);
            bool _release_modal_widget(Widget& w);
            void _subscribe_to_timer_ticks(Widget& w) { mevent_handler._subscribe_to_timer_ticks(w); }
            void _unsubscribe_from_timer_ticks(Widget& w) { mevent_handler._unsubscribe_from_timer_ticks(w); }
            void _enqueue_deferred(const std::function <void ()>& callback);
        private:
            void set_top(TopWidget* top);

            void handle_deferred_actions();
            void handle_relayout();
            void handle_deferred_callbacks();

            dtl::EventHandler mevent_handler;

            TopWidget* mtop_widget;
            Widget* mmodal_widget;

            struct TopWidgetStackEntry {
                TopWidget* top_widget;
                Color cover_col;
                Widget* focus_widget;
                int top_stack_start_drawing_idx;
            };

            std::deque <TopWidgetStackEntry> mtop_widget_stack;
            int mdraw_widget_stack_start;

            struct DeferredAction {
                enum Actions { PushTop, PopTop, BringToFront, SendToBack } action;
                Widget* widget;
                TopWidget* top_widget;
                bool shinethrough;
                Color cover_col;

                explicit DeferredAction(Actions a)
                    : action(a), widget(nullptr), top_widget(nullptr) {}
                DeferredAction(Actions a, Widget* w)
                    : action(a), widget(w), top_widget(nullptr) {}
                DeferredAction(Actions a, TopWidget* w, bool _st, const Color& col)
                    : action(a), widget(nullptr), top_widget(w), shinethrough(_st),
                      cover_col(col) {}
            };
            std::deque <DeferredAction> mdeferred_actions;

            std::vector <std::function<void ()>> mdeferred_callbacks;

            std::unordered_set <Widget*> mrelayout_widgets;

            bool munder_mouse_invalid, mhandling_events, mlayout_in_progress, mhandling_deferred_callbacks;
    };

}

#endif // LGUI_GUI_H
