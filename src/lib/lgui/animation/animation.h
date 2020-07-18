#ifndef LGUI_ABSTRACTANIMATION_H
#define LGUI_ABSTRACTANIMATION_H

#include <functional>
#include "animationlistener.h"
#include "ianimation.h"

namespace lgui {

class Animation : public IAnimation {
    public:
        using Callback = std::function<void()>;

        void start() override {
            if (!is_playing()) {
                mis_reversed = false;
                IAnimation::start();
                if (manimation_listener) {
                    manimation_listener->animation_started(*this);
                }
            }
        }

        void end() override {
            if (is_playing()) {
                if (manimation_listener) {
                    manimation_listener->animation_ended(*this);
                }
                if (mend_callback) {
                    mend_callback();
                }
                IAnimation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                if (manimation_listener) {
                    manimation_listener->animation_cancelled(*this);
                }
                IAnimation::cancel();
            }
        }

        virtual void start_reverse() {
            if (!is_playing()) {
                mis_reversed = true;
                IAnimation::start();
                if (manimation_listener) {
                    manimation_listener->animation_started_reversed(*this);
                }
            }
        }

        virtual void end_reverse() {
            if (is_playing()) {
                if (manimation_listener) {
                    manimation_listener->animation_ended_reversed(*this);
                }
                if (mend_reversed_callback) {
                    mend_reversed_callback();
                }
                IAnimation::end();
            }
        }

        virtual void reverse() {
            if (is_playing()) {
                mis_reversed = !mis_reversed;
                if (manimation_listener) {
                    manimation_listener->animation_reversed(*this);
                }
            }
        }

        bool is_reversed() const {
            return mis_reversed;
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

        const Callback& end_reversed_callback() const {
            return mend_reversed_callback;
        }
        void set_end_reversed_callback(const Callback& end_reversed_callback) {
            mend_reversed_callback = end_reversed_callback;
        }

        int index() const {
            return mindex;
        }
        void set_index(int index) {
            mindex = index;
        }

    private:
        Callback mend_callback;
        Callback mend_reversed_callback;
        AnimationListener* manimation_listener = nullptr;
        int mindex;
        bool mis_reversed = false;
};

}

#endif //LGUI_ABSTRACTANIMATION_H
