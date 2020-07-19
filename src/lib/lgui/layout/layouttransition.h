#ifndef LGUI_LAYOUTTRANSITION_H
#define LGUI_LAYOUTTRANSITION_H

#include <unordered_map>
#include <variant>
#include <lgui/animation/valueanimation.h>
#include <lgui/animation/animationcontext.h>
#include "lgui/widget.h"

namespace lgui {

class LayoutTransition : public AnimationListener {
    public:
        void set_root_widget(Widget* widget);

        void widget_layout(Widget& w, const Rect& old_rect, const Rect& new_rect);
        void widget_done_layout(Widget& w);
        void widget_added(Widget& w);
        void widget_ungone(Widget& w);
        void widget_about_to_be_removed(Widget& w);
        void widget_about_to_be_gone(Widget& w);

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
