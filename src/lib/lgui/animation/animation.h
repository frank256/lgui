#ifndef LGUI_ABSTRACTANIMATION_H
#define LGUI_ABSTRACTANIMATION_H

#include <functional>
#include "animation_listener.h"
#include "ianimation.h"

namespace lgui {

class Animation : public IAnimation {
    public:
        using Callback = std::function<void ()>;

        void start() override {
            IAnimation::start();
            if (manimation_listener) {
                manimation_listener->animation_started(*this);
            }
        }

        void end() override {
            if (manimation_listener) {
                manimation_listener->animation_ended(*this);
            }
            IAnimation::end();
            if (mend_callback) {
                mend_callback();
            }
        }

        void cancel() override {
            if (manimation_listener) {
                manimation_listener->animation_cancelled(*this);
            }
            IAnimation::cancel();
        }

        AnimationListener* animation_listener() const { return manimation_listener; }

        void set_animation_listener(AnimationListener* animation_listener) {
            manimation_listener = animation_listener;
        }

        const Callback& end_callback() const {
            return mend_callback;
        }

        void set_end_callback(const Callback& end_callback) {
            mend_callback = end_callback;
        }

        int index() const {
            return mindex;
        }
        void set_index(int index) {
            mindex = index;
        }

    private:
        Callback mend_callback;
        AnimationListener* manimation_listener = nullptr;
        int mindex;
};

}

#endif //LGUI_ABSTRACTANIMATION_H
