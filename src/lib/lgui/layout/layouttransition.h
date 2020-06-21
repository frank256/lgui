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
        using Callback = std::function<void ()>;
        enum class WidgetState {
                Appearing, DisappearingToBeGone, DisappearingToBeRemoved, ChangingRect
        };
        struct LayoutAnimationState { ;
            WidgetState state;
            Widget* w;
            ValueAnimationBase* animation;
        };

        LayoutAnimationState& register_for(Widget& widget, WidgetState ws);
        LayoutAnimationState* get_widget_state(Widget& w);
        LayoutAnimationState& create_widget_state(Widget& w);

        std::unordered_map<lgui::Widget*, LayoutAnimationState> mani_map;
        std::unordered_map<lgui::Animation*, LayoutAnimationState*> mani_rev_map;
        Widget* mroot_widget = nullptr;
        AnimationContext manimation_context;
        bool mis_transition_in_progress = false;
};

}

#endif //LGUI_LAYOUTTRANSITION_H
