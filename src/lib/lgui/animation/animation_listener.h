#ifndef LGUI_ANIMATION_LISTENER_H
#define LGUI_ANIMATION_LISTENER_H

namespace lgui {

class Animation;

class AnimationListener {
    public:
        virtual void animation_started(Animation&) { };
        virtual void animation_cancelled(Animation&) { };
        virtual void animation_ended(Animation&) { };
};
}

#endif //LGUI_ANIMATION_LISTENER_H
