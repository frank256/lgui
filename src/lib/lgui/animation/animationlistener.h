#ifndef LGUI_ANIMATION_LISTENER_H
#define LGUI_ANIMATION_LISTENER_H

namespace lgui {

class Animation;

class AnimationListener {
    public:
        virtual void animation_started(Animation&) { };
        virtual void animation_cancelled(Animation&) { };
        virtual void animation_ended(Animation&) { };

        virtual void animation_started_reversed(Animation&) { };
        virtual void animation_ended_reversed(Animation&) { };
        virtual void animation_reversed(Animation&) { };
};
}

#endif //LGUI_ANIMATION_LISTENER_H
