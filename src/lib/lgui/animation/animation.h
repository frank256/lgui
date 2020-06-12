#ifndef LGUI_ANIMATION_H
#define LGUI_ANIMATION_H

#include "animation_listener.h"
#include "animationhandler.h"
#include "ianimation.h"

namespace lgui {

class Animation : public IAnimation {
        friend class dtl::AnimationHandler;

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
            if (manimation_listener) {
                manimation_listener->animation_ended(*this);
            }
            dtl::AnimationHandler::instance().deregister_animation(*this);
            IAnimation::end();
        }

        void cancel() override {
            if (manimation_listener) {
                manimation_listener->animation_cancelled(*this);
            }
            dtl::AnimationHandler::instance().deregister_animation(*this);
            IAnimation::cancel();
        }

        bool is_registered() const { return mis_registered; }

        virtual void update(double timestamp, double elapsed_time) = 0;


        bool can_delete() const override {
            return !is_playing() && !mis_registered;
        }

    private:
        AnimationListener* manimation_listener = nullptr;
        bool mis_registered = false;
};

}

#endif //LGUI_ANIMATION_H
