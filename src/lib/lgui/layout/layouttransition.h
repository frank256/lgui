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

#ifndef LGUI_LAYOUTTRANSITION_H
#define LGUI_LAYOUTTRANSITION_H

#include <unordered_map>
#include <variant>
#include <lgui/animation/valueanimation.h>
#include <lgui/animation/animationcontext.h>
#include "lgui/widget.h"

namespace lgui {

/** Experimental class to animate layout changes when adding removing or widgets in a subtree.
    This class will intercept adding and removing of widgets as well as the resulting layout passes.
    Use Widget::set

    Widgets that are added or become visible (from GONE) will be technically added with a fade opacity of zero.
    This will trigger a layout pass which is intercepted and animated. This layout pass will also assign space to
    the widget that was added. After this has occurred, the widget will be faded in.

    Widgets that are removed or become GONE will be faded out and only then actually removed (or set to GONE).
    The actual removal from the layout triggers a layout pass which is intercepted and animated.

    Note that only one LayoutTransition can be set on one subtree (and it will go down the whole tree from its root).

    Note that there can still be problems like glitches when adding or removing a widget while a previous operation is
    still in progress or when triggering a layout run by other actions that interfere with the LayoutTransition.

    Note that the delayed removal or setting to GONE of widgets may lead to different behavior that has to be accounted
    for.
*/
class LayoutTransition : public AnimationListener {
    public:
        /** Sets the LayoutTransition the on subtree that starts with the passed widget. */
        void set_root_widget(Widget* widget);

        void widget_layout(Widget& w, const Rect& old_rect, const Rect& new_rect);
        void widget_done_layout(Widget& w);
        void widget_added(Widget& w);
        void widget_ungone(Widget& w);
        void widget_about_to_be_removed(Widget& w);
        void widget_about_to_be_gone(Widget& w);

        /** Return whether a transition is still in progress. */
        bool is_transition_in_progress() const { return mis_transition_in_progress; }

        // Internal.
        void animation_ended(Animation& animation) override;

    private:
        using Callback = std::function<void()>;
        enum class WidgetState {
                WaitingToAppear, Appearing, DisappearingToBeGone, DisappearingToBeRemoved, ChangingRect
        };
        struct LayoutAnimationState { ;
            WidgetState state;
            Widget* w;
            ValueAnimationBase* animation;
        };

        void handle_animation_ended(LayoutAnimationState* p_state);
        void start_appearing_animations();

        LayoutAnimationState* get_widget_state(Widget& w);
        LayoutAnimationState& create_widget_state(Widget& w);

        void add_animation_for_widget(Widget& w, ValueAnimationBase& a, WidgetState widget_state);
        ValueAnimation <Rect>& create_rect_transition_animation(Widget& w, const Rect& old_rect, const Rect& new_rect);
        ValueAnimation<float>& create_fadein_animation(Widget& w);
        ValueAnimation<float>& create_fadeout_animation(Widget& w);

        std::unordered_map<lgui::Widget*, LayoutAnimationState> mani_map;
        std::unordered_map<lgui::Animation*, LayoutAnimationState*> mani_rev_map;
        Widget* mroot_widget = nullptr;
        AnimationContext manimation_context;
        bool mis_transition_in_progress = false, mis_intercepting_next_layout_pass = false, mtrigger_layout_again = false,
                martificial_end = false;
        int mlayout_animation_counter = 0;
};

}

#endif //LGUI_LAYOUTTRANSITION_H
