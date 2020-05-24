#ifndef LGUITEST_ANIMATION_H
#define LGUITEST_ANIMATION_H

#include "animation_listener.h"
#include "animationhandler.h"
#include "ianimation.h"

namespace lgui {

class Animation : public IAnimation {
    public:
        AnimationListener* animation_listener() const { return manimation_listener; }

        void set_animation_listener(AnimationListener* animation_listener) {
            manimation_listener = animation_listener;
        }

        void start() override {
            IAnimation::start();
            if (manimation_listener) {
                manimation_listener->animation_started(*this);
            }
            dtl::AnimationHandler::instance().register_animation(*this);
        }

        void end() override {
            IAnimation::end();
            if (manimation_listener) {
                manimation_listener->animation_ended(*this);
            }
            dtl::AnimationHandler::instance().deregister_animation(*this);
        }

        void cancel() override {
            IAnimation::cancel();
            if (manimation_listener) {
                manimation_listener->animation_cancelled(*this);
            }
            dtl::AnimationHandler::instance().deregister_animation(*this);
        }

        virtual void update(double timestamp, double elapsed_time) = 0;

    private:
        AnimationListener* manimation_listener = nullptr;
};

}

#endif //LGUITEST_ANIMATION_H
