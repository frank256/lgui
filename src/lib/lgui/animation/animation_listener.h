#ifndef LGUITEST_ANIMATION_LISTENER_H
#define LGUITEST_ANIMATION_LISTENER_H

namespace lgui {

class Animation;

class AnimationListener {
    public:
        virtual void animation_started(Animation&) { };
        virtual void animation_cancelled(Animation&) { };
        virtual void animation_ended(Animation&) { };
};
}

#endif //LGUITEST_ANIMATION_LISTENER_H
