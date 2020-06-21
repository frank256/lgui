#include <lgui/animation/animationbuilder.h>
#include <lgui/basiccontainer.h>
#include "lgui/platform/error.h"
#include "layouttransition.h"

namespace lgui {

void LayoutTransition::set_root_widget(Widget* widget) {
    if (mroot_widget) {
        mroot_widget->visit_down([this](Widget& w) {
            w._set_layout_transition(nullptr);
        });
    }
    mroot_widget = widget;
    if (widget) {
        widget->visit_down([this](Widget& w) {
            ASSERT(w.layout_transition() == nullptr);
            w._set_layout_transition(this);
        });
    }
}

void LayoutTransition::widget_layout(Widget& w, const Rect& old_rect, const Rect& new_rect) {
    debug("layout %p, or: %d, %d, %d, %d, nr: %d, %d, %d, %d\n", &w, old_rect.x(), old_rect.y(), old_rect.w(), old_rect.h(),
            new_rect.x(), new_rect.y(), new_rect.w(), new_rect.h());
    LayoutAnimationState* state = get_widget_state(w);
    if (state) {
        if (state->state == WidgetState::Appearing) {
            w.set_rect(new_rect);
            // Register appearing animation.
        }
        else if (state->state == WidgetState::ChangingRect) {
            if (state->animation && state->animation->is_playing()) {
//                if (ValueAnimation<Rect>)
//                state->animation->cancel();
//
            }
            // Transitioning animation: cancel, update target_rect
        }
    }
    else {
        if (old_rect.size() == Size(0, 0)) {
            w.set_rect(new_rect);
            return;
        }
        if (old_rect == new_rect) {
            w.set_rect(new_rect);
            return;
        }

        state = &create_widget_state(w);
        state->state = WidgetState::ChangingRect;
        // Create animation with target
        ValueAnimation<Rect>& animation = ValueAnimationBuilderWithContext<Rect>(manimation_context)
                .with_value_setter([&w](Rect r) {
//                    debug("Setting rect: %d, %d, %d, %d\n", r.x(), r.y(), r.w(), r.h());
                    w.set_rect(r);
                })
                .from(old_rect)
                .to(new_rect)
                .with_duration(0.3)
                .build();
        animation.set_animation_listener(this);
        mani_rev_map.emplace(std::make_pair(&animation, state));
        state->animation = &animation;
    }
}

void LayoutTransition::widget_added(Widget& w) {
    register_for(w, WidgetState::Appearing);
}

void LayoutTransition::widget_ungone(Widget& w) {
    register_for(w, WidgetState::Appearing);
}

void LayoutTransition::widget_about_to_be_removed(Widget& w) {
//    register_for(w, WidgetState::DisappearingToBeRemoved);
    if (auto container = dynamic_cast<BasicContainer*>(w.parent())) {
        container->remove_child(w);
    }
}

void LayoutTransition::widget_about_to_be_gone(Widget& w) {
    w._set_gone();
//    register_for(w, WidgetState::DisappearingToBeGone);
}

LayoutTransition::LayoutAnimationState& LayoutTransition::register_for(Widget& widget, WidgetState ws) {
    LayoutAnimationState* state = get_widget_state(widget);
    if (!state) {
        state = &create_widget_state(widget);
    }
    state->state = ws;
    return *state;
}

LayoutTransition::LayoutAnimationState* LayoutTransition::get_widget_state(Widget& w) {
    if (mani_map.find(&w) != mani_map.end())
        return &mani_map[&w];
    return nullptr;
}

LayoutTransition::LayoutAnimationState& LayoutTransition::create_widget_state(Widget& w) {
    return mani_map.emplace(&w, LayoutAnimationState{{}, &w, nullptr}).first->second;
}

void LayoutTransition::animation_ended(Animation& animation) {
    auto it = mani_rev_map.find(&animation);
    if (it != mani_rev_map.end()) {
        LayoutAnimationState* las = it->second;
        ASSERT(las->animation == &animation);
        mani_map.erase(it->second->w);
        mani_rev_map.erase(it);
        debug("Removed animation, %d left\n", mani_map.size());
    }
    manimation_context.clear();
    if (mani_map.empty()) {
        debug("None in progress!");
        mis_transition_in_progress = false;
    }
}

void LayoutTransition::widget_done_layout(Widget& w) {
    if (mroot_widget == &w && !mis_transition_in_progress) {
        int n = 0;
        for(auto it : mani_map) {
            if (!it.second.animation->is_playing()) {
                it.second.animation->start();
                ++n;
            }
        }
        if (n > 0) {
            debug("Started %d animations!\n", n);
            mis_transition_in_progress = true;
        }
    }
}

}
