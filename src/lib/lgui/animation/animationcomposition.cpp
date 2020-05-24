#include "animationcomposition.h"

#include "lgui/vector_utils.h"

namespace lgui {

uint64_t AnimationComposition::mid = 1;

void AnimationComposition::animation_started(Animation& animation) {
    AnimationListener::animation_started(animation);
    mcurrently_playing.emplace_back(&animation);
}

void AnimationComposition::animation_cancelled(Animation& animation) {
    AnimationListener::animation_cancelled(animation);
}

void AnimationComposition::animation_ended(Animation& animation) {
    AnimationListener::animation_ended(animation);
    erase_remove(mcurrently_playing, &animation);
    auto find_result = mani_finished_to_node.find(&animation);
    if (find_result != mani_finished_to_node.end()) {
        Node& node = *find_result->second;
        if (node.finished_callback) {
            node.finished_callback();
        }
        start_node(node);
    }
}

void AnimationComposition::add_at_start(Animation& animation) {
    if (!mstart_node) {
        mstart_node = new_node();
    }
    if (!contains(mstart_node->started, &animation)) {
        mstart_node->started.emplace_back(&animation);
        mani_start_to_node.emplace(std::make_pair(&animation, mstart_node));
    }
    Node* finish_node = new_node();
    add_finished_at_node(animation, *finish_node);
    animation.set_animation_listener(this);
}

void AnimationComposition::add_with(Animation& animation, Animation& with) {
    auto find_result = mani_start_to_node.find(&with);
    if (find_result != mani_start_to_node.end()) {
        add_started_at_node(animation, *find_result->second);
        Node* finish_node = new_node();
        add_finished_at_node(animation, *finish_node);
        animation.set_animation_listener(this);
    }
}

void AnimationComposition::add_after(Animation& animation, Animation& after) {
    auto find_result = mani_finished_to_node.find(&after);
    if (find_result != mani_finished_to_node.end()) {
        add_started_at_node(animation, *find_result->second);
        Node* finish_node = new_node();
        add_finished_at_node(animation, *finish_node);
        animation.set_animation_listener(this);
    }
}

void AnimationComposition::set_callback_after(const AnimationComposition::Callback& callback,
                                              Animation& after) {
    auto find_result = mani_finished_to_node.find(&after);
    if (find_result != mani_finished_to_node.end()) {
        find_result->second->finished_callback = callback;
    }
}

void AnimationComposition::end() {
    IAnimation::end();
}

void AnimationComposition::start() {
    if (mstart_node) {
        start_node(*mstart_node);
    }
}

void AnimationComposition::cancel() {
    IAnimation::cancel();
    for (Animation* ani : mcurrently_playing) {
        ani->cancel();
    }
}

void AnimationComposition::start_node(AnimationComposition::Node& node) {
    for (Animation* ani : node.started) {
        ani->start();
    }
}

void AnimationComposition::add_started_at_node(Animation& animation,
                                               AnimationComposition::Node& start_node) {
    start_node.started.emplace_back(&animation);
    mani_start_to_node.emplace(std::make_pair(&animation, &start_node));
}

void AnimationComposition::add_finished_at_node(Animation& animation,
                                                AnimationComposition::Node& finish_node) {
    finish_node.finished.emplace_back(&animation);
    mani_finished_to_node.emplace(std::make_pair(&animation, &finish_node));
}

AnimationComposition::Node* AnimationComposition::new_node() {
    mnodes.emplace_back(Node());
    Node& node = mnodes.back();
    node.id = mid++;
    return &node;
}
}
