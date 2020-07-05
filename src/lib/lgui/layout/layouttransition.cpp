#include <lgui/animation/animationbuilder.h>
#include <lgui/basiccontainer.h>
#include "lgui/platform/error.h"
#include "layouttransition.h"

namespace lgui {

void LayoutTransition::set_root_widget(Widget* widget) {
    if (mroot_widget) {
        // TODO: Cancel? Finish all animations.
        mroot_widget->visit_down([](Widget& w) {
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
//    debug("layout %p, or: %d, %d, %d, %d, nr: %d, %d, %d, %d\n", &w, old_rect.x(), old_rect.y(), old_rect.w(), old_rect.h(),
//            new_rect.x(), new_rect.y(), new_rect.w(), new_rect.h());
    if (!mis_intercepting_next_pass) {
        w.set_rect(new_rect);
        // Anything else? Post layout?
        return;
    }
    LayoutAnimationState* state = get_widget_state(w);
    if (state) {
//        debug("State for this widget already existing, state: %d, ani: %p, is playing: %d\n", state->state,
//              state->animation,
//              state->animation ? state->animation->is_playing() : false);
        if (state->state == WidgetState::WaitingToAppear) {
            w.set_rect(new_rect);
            return;
        }
        else if (state->state == WidgetState::ChangingRect) {
            if (state->animation && state->animation->is_playing()) {
//                debug("Registering a new rect?\n");
//                if (ValueAnimation<Rect>)
//                state->animation->cancel();
//
            }
            // Transitioning animation: cancel, update target_rect
        }
    }
    else {
//        debug("State not existing.\n");
        if (old_rect.size() == Size(0, 0)) {
            w.set_rect(new_rect);
            return;
        }
        if (old_rect == new_rect) {
            w.set_rect(new_rect);
            return;
        }
        // Create animation with target
        ValueAnimation<Rect>& animation = create_rect_transition_animation(w, old_rect, new_rect);
        add_animation_for_widget(w, animation, WidgetState::ChangingRect);
        ++mlayout_animation_counter;
    }
}

void LayoutTransition::widget_added(Widget& w) {
    mis_intercepting_next_pass = true;
    debug("Widget %p added\n", &w);
//    register_for(w, WidgetState::Appearing);
    ASSERT(w.layout_transition() == nullptr || w.layout_transition() == this);
    w._set_layout_transition(this);
    ValueAnimation<float>& ani = create_fadein_animation(w);
    add_animation_for_widget(w, ani, WidgetState::WaitingToAppear);
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
//    w.set_opacity(0);
}

void LayoutTransition::widget_ungone(Widget& w) {
    mis_intercepting_next_pass = true;
    debug("Widget %p ungone\n", &w);
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
//    register_for(w, WidgetState::Appearing);
}

void LayoutTransition::widget_about_to_be_removed(Widget& w) {
    debug("Widget %p about to be removed\n", &w);
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr && state->state == WidgetState::DisappearingToBeRemoved) {
        return;
    }
    // How are different cases handled?
    ValueAnimation<float>& ani = create_fadeout_animation(w);
    add_animation_for_widget(w, ani, WidgetState::DisappearingToBeRemoved);
    ani.start();
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
    mis_intercepting_next_pass = true;
}

void LayoutTransition::widget_about_to_be_gone(Widget& w) {
    debug("Widget %p about to be gone\n", &w);
//    w._set_gone();
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr && state->state == WidgetState::DisappearingToBeGone) {
        return;
    }
    // How are different cases handled?
    ValueAnimation<float>& ani = create_fadeout_animation(w);
    add_animation_for_widget(w, ani, WidgetState::DisappearingToBeGone);
    ani.start();
    mis_intercepting_next_pass = true;
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
//    register_for(w, WidgetState::DisappearingToBeGone);
}

void LayoutTransition::animation_ended(Animation& animation) {
    auto it = mani_rev_map.find(&animation);
    if (it != mani_rev_map.end()) {
        LayoutAnimationState* las = it->second;
        ASSERT(las->animation == &animation);
        handle_animation_ended(las);
        mani_map.erase(it->second->w);
        mani_rev_map.erase(it);
//        debug("Removed animation, %d left\n", mani_map.size());
    }
//    debug("mlayout_animation_counter: %d\n", mlayout_animation_counter);
    if (mani_map.empty()) {
//        debug("None in progress!");
        mis_transition_in_progress = false;
        manimation_context.clear();
    }

    if (mlayout_animation_counter == 0) {
        if (mtrigger_layout_again) {
            // Won' work if still in progress ...
            debug("Triggering layout again!\n");
            mtrigger_layout_again = false;
            mis_intercepting_next_pass = true;
            mroot_widget->request_layout();
        }
        else {
            start_appearing_animations();
        }
    }
}

void LayoutTransition::handle_animation_ended(LayoutTransition::LayoutAnimationState* state) {
    Widget* w = state->w;
    if (state->state == WidgetState::DisappearingToBeGone) {
        w->_set_gone();
        if (mis_transition_in_progress) {
            mtrigger_layout_again = true;
        }
    }
    else if (state->state == WidgetState::DisappearingToBeRemoved) {
        if (auto container = dynamic_cast<BasicContainer*>(w->parent())) {
            container->_remove_child(*w);
            if (mis_transition_in_progress) {
                mtrigger_layout_again = true;
            }
            else {
                container->request_layout();
            }
        }
    } else if (state->state == WidgetState::ChangingRect) {
        --mlayout_animation_counter;
    }
}

void LayoutTransition::widget_done_layout(Widget& w) {
    if (mroot_widget == &w && !mis_transition_in_progress && mis_intercepting_next_pass) {
        mis_intercepting_next_pass = false;
        int n = 0;
        for (auto it : mani_map) {
            if (it.second.state == WidgetState::ChangingRect && !it.second.animation->is_playing()) {
                it.second.animation->start();
                ++n;
            }
        }
        if (n > 0) {
            debug("Started %d animations!\n", n);
            mis_transition_in_progress = true;
        } else {
            start_appearing_animations();
        }
    }
}

void LayoutTransition::start_appearing_animations()  {
    for (auto& entry : mani_map) {
        if (entry.second.state == WidgetState::WaitingToAppear) {
            entry.second.state = WidgetState::Appearing;
            if (!entry.second.animation->is_playing()) {
                entry.second.animation->start();
            }
        }
    }
}

void LayoutTransition::add_animation_for_widget(Widget& w, ValueAnimationBase& animation, WidgetState widget_state) {
//    debug("Request to add %d state ani for %p\n", widget_state, &w);
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr) {
//        debug("Old state: %d, new state: %d\n", state->state, widget_state);
        state->animation->end(); // Will clear new animation - dangerous!
        ASSERT(get_widget_state(w) == nullptr);
    }
//    ASSERT(state == nullptr);
    state = &create_widget_state(w);
    state->state = widget_state;
    animation.set_animation_listener(this);
    mani_rev_map.emplace(std::make_pair(&animation, state));
    state->animation = &animation;
}

LayoutTransition::LayoutAnimationState* LayoutTransition::get_widget_state(Widget& w) {
    if (mani_map.find(&w) != mani_map.end())
        return &mani_map[&w];
    return nullptr;
}

LayoutTransition::LayoutAnimationState& LayoutTransition::create_widget_state(Widget& w) {
    return mani_map.emplace(&w, LayoutAnimationState{{}, &w, nullptr}).first->second;
}

ValueAnimation<Rect>& LayoutTransition::create_rect_transition_animation(Widget& w, const Rect& old_rect,
                                                                         const Rect& new_rect) {
    return ValueAnimationBuilderWithContext<Rect>(manimation_context)
            .with_value_setter([&w](Rect r) {
                //                    debug("Setting rect: %d, %d, %d, %d\n", r.x(), r.y(), r.w(), r.h());
                w.set_rect(r);
            })
            .from(old_rect)
            .to(new_rect)
            .with_duration(0.5)
            .build();
}


ValueAnimation<float>& LayoutTransition::create_fadeout_animation(Widget& w) {
    return ValueAnimationBuilderWithContext<float>(manimation_context)
            .with_value_setter([&w](float a) {
                w.set_opacity(a);
            })
            .from(1.0)
            .to(0.0)
            .with_duration(0.5)
            .build();
}

ValueAnimation<float>& LayoutTransition::create_fadein_animation(Widget& w) {
    w.set_opacity(0);
    return ValueAnimationBuilderWithContext<float>(manimation_context)
            .with_value_setter([&w](float a) {
                w.set_opacity(a);
            })
            .from(0)
            .to(1.0)
            .with_duration(0.5)
            .build();
}

}