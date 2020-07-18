#ifndef LGUI_ANIMATION_LISTENER_H
#define LGUI_ANIMATION_LISTENER_H

namespace lgui {

class AbstractAnimation;

class AnimationListener {
    public:
        virtual void animation_started(AbstractAnimation&) { };
        virtual void animation_cancelled(AbstractAnimation&) { };
        virtual void animation_ended(AbstractAnimation&) { };
};
}

#endif //LGUI_ANIMATION_LISTENER_H
