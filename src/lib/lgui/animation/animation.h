#ifndef LGUI_ANIMATION_H
#define LGUI_ANIMATION_H

#include "animation_listener.h"
#include "animationhandler.h"
#include "abstractanimation.h"

namespace lgui {

class Animation : public AbstractAnimation {
        friend class dtl::AnimationHandler;

    public:
        virtual void update(double timestamp, double elapsed_time) = 0;

        void start() override {
            AbstractAnimation::start();
            dtl::AnimationHandler::instance().register_animation(*this);
        }

        void end() override {
            dtl::AnimationHandler::instance().deregister_animation(*this);
            AbstractAnimation::end();
        }

        void cancel() override {
            dtl::AnimationHandler::instance().deregister_animation(*this);
            AbstractAnimation::cancel();
        }

        bool is_registered() const { return mis_registered; }

        bool can_delete() const override {
            return !is_playing() && !mis_registered;
        }

    private:
        bool mis_registered = false;
};

}

#endif //LGUI_ANIMATION_H
