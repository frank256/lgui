#ifndef LGUI_ANIMATIONHANDLER_H
#define LGUI_ANIMATIONHANDLER_H

#include <memory>
#include <vector>

namespace lgui {

class TimerTickEvent;

class Animation;

namespace dtl {

class AnimationHandler {
    public:
        AnimationHandler() {
            manimations.reserve(128);
        }

        void register_animation(Animation& animation);
        void deregister_animation(Animation& animation);

        void update(const TimerTickEvent& timer_event);

        static AnimationHandler& instance() {
            return minstance;
        }

    private:
        std::vector<Animation*> manimations;
        double mlast_timestamp = 0;

        static AnimationHandler minstance;
};

}
}

#endif //LGUI_ANIMATIONHANDLER_H
