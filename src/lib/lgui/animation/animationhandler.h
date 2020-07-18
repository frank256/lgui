#ifndef LGUI_ANIMATIONHANDLER_H
#define LGUI_ANIMATIONHANDLER_H

#include <memory>
#include <vector>

namespace lgui {

class TimerTickEvent;

class ConcreteAnimation;

namespace dtl {

class AnimationHandler {
    public:
        AnimationHandler() {
            manimations.reserve(128);
        }

        void register_animation(ConcreteAnimation& animation);
        void deregister_animation(ConcreteAnimation& animation);

        void update(const TimerTickEvent& timer_event);

        static AnimationHandler& instance() {
            return minstance;
        }

    private:
        std::vector<ConcreteAnimation*> manimations;
        double mlast_timestamp = 0;

        static AnimationHandler minstance;
};

}
}

#endif //LGUI_ANIMATIONHANDLER_H
