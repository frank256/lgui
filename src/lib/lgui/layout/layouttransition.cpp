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

#include <lgui/animation/animationbuilder.h>
#include <lgui/basiccontainer.h>
#include "lgui/platform/error.h"
#include "layouttransition.h"

namespace lgui {

void LayoutTransition::set_root_widget(Widget* widget) {
    if (mroot_widget) {
        // TODO: cancel / finish animations?
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
    if (!mis_intercepting_next_layout_pass) {
        // We're not intercepting layout right now, do without animation.
        w.set_rect(new_rect);
        w.post_layout();
        return;
    }
    LayoutAnimationState* state = get_widget_state(w);
    if (state) {
        if (state->state == WidgetState::ChangingRect) {
            warning("Layout animation to start while another one is still in progress for %p.\n", &w);
            return;
        }
        else {
            // Especially expected (intended) for WidgetState::WaitingToAppear
            w.set_rect(new_rect);
            // FIXME: DO something else here?
            return;
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
        // Create animation with target
        ValueAnimation<Rect>& animation = create_rect_transition_animation(w, old_rect, new_rect);
        add_animation_for_widget(w, animation, WidgetState::ChangingRect);
        ++mlayout_animation_counter;
    }
}

void LayoutTransition::widget_added(Widget& w) {
    ASSERT(w.layout_transition() == nullptr || w.layout_transition() == this);
    mis_intercepting_next_layout_pass = true;
    w._set_layout_transition(this);
    ValueAnimation<float>& ani = create_fadein_animation(w);
    add_animation_for_widget(w, ani, WidgetState::WaitingToAppear);
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
}

void LayoutTransition::widget_ungone(Widget& w) {
    ValueAnimation<float>& ani = create_fadein_animation(w);
    add_animation_for_widget(w, ani, WidgetState::WaitingToAppear);
    mis_intercepting_next_layout_pass = true;
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
}

void LayoutTransition::widget_about_to_be_removed(Widget& w) {
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr && state->state == WidgetState::DisappearingToBeRemoved) {
        return;
    }
    else if (state != nullptr) {
        // FIXME: handle different cases?
    }
    // How are different cases handled?
    ValueAnimation<float>& ani = create_fadeout_animation(w);
    add_animation_for_widget(w, ani, WidgetState::DisappearingToBeRemoved);
    ani.start();
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
    mis_intercepting_next_layout_pass = true;
}

void LayoutTransition::widget_about_to_be_gone(Widget& w) {
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr && state->state == WidgetState::DisappearingToBeGone) {
        return;
    }
    else if (state != nullptr) {
        // FIXME: handle different cases?
    }
    ValueAnimation<float>& ani = create_fadeout_animation(w);
    add_animation_for_widget(w, ani, WidgetState::DisappearingToBeGone);
    ani.start();
    mis_intercepting_next_layout_pass = true;
    if (mis_transition_in_progress) {
        mtrigger_layout_again = true;
    }
}

void LayoutTransition::animation_ended(Animation& animation) {
    auto it = mani_rev_map.find(&animation);
    if (it != mani_rev_map.end()) {
        LayoutAnimationState* las = it->second;
        ASSERT(las->animation == &animation);
        handle_animation_ended(las);
        mani_map.erase(it->second->w);
        mani_rev_map.erase(it);
    }
    if (mani_map.empty() && !martificial_end) {
        mis_transition_in_progress = false;
        manimation_context.clear();
    }

    if (mlayout_animation_counter == 0) {
        start_appearing_animations(); // Do not want to be stuck.
        if (mtrigger_layout_again && !mis_transition_in_progress) {
            mtrigger_layout_again = false;
            mis_intercepting_next_layout_pass = true;
            mroot_widget->request_layout();
        }
    }
}

void LayoutTransition::handle_animation_ended(LayoutTransition::LayoutAnimationState* state) {
    Widget* w = state->w;
    if (state->state == WidgetState::DisappearingToBeGone) {
        w->_set_gone(); // Will trigger layout if transition is not in progress.
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
                container->request_layout(); // Better trigger here, layout might not be listening.
            }
        }
    }
    else if (state->state == WidgetState::ChangingRect) {
        state->w->post_layout();
        --mlayout_animation_counter;
    }
}

void LayoutTransition::widget_done_layout(Widget& w) {
    if (mroot_widget == &w && !mis_transition_in_progress && mis_intercepting_next_layout_pass) {
        mis_intercepting_next_layout_pass = false;
        int n = 0;
        for (auto it : mani_map) {
            if (it.second.state == WidgetState::ChangingRect && !it.second.animation->is_playing()) {
                it.second.animation->start();
                ++n;
            }
        }
        if (n > 0) {
            mis_transition_in_progress = true;
        }
        else {
            start_appearing_animations();
        }
    }
}

void LayoutTransition::start_appearing_animations() {
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
    LayoutAnimationState* state = get_widget_state(w);
    if (state != nullptr) {
        martificial_end = true;
        state->animation->end();
        martificial_end = false;
        ASSERT(get_widget_state(w) == nullptr);
    }
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
                w.set_fade_opacity(a);
            })
            .from(1.0)
            .to(0.0)
            .with_duration(0.5)
            .build();
}

ValueAnimation<float>& LayoutTransition::create_fadein_animation(Widget& w) {
    w.set_fade_opacity(0);
    return ValueAnimationBuilderWithContext<float>(manimation_context)
            .with_value_setter([&w](float a) {
                w.set_fade_opacity(a);
            })
            .from(0)
            .to(1.0)
            .with_duration(0.5)
            .build();
}

}