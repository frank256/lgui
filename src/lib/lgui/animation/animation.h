/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-20 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LGUI_ANIMATION_H
#define LGUI_ANIMATION_H

#include <functional>
#include "animationlistener.h"
#include "ianimation.h"

namespace lgui {

/** Animation base class implementing reverse playing, callbacks when the animation is done, and an animation listener. */
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

        /** Start the animation playing in reverse. */
        virtual void start_reverse() {
            if (!is_playing()) {
                mis_reversed = true;
                IAnimation::start();
                if (manimation_listener) {
                    manimation_listener->animation_started_reversed(*this);
                }
            }
        }

        /** Finish the animation playing in reverse. */
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

        /** Reverse the animation while it is playing. */
        virtual void reverse() {
            if (is_playing()) {
                mis_reversed = !mis_reversed;
                if (manimation_listener) {
                    manimation_listener->animation_reversed(*this);
                }
            }
        }

        /** Return whether the animation is playing in reverse. Only meaningful if it is playing. */
        bool is_reversed() const {
            return mis_reversed;
        }

        AnimationListener* animation_listener() const { return manimation_listener; }

        /** Set an animation listener on the animation. This is mostly intended for internal use. */
        void set_animation_listener(AnimationListener* animation_listener) {
            manimation_listener = animation_listener;
        }

        const Callback& end_callback() const {
            return mend_callback;
        }

        /** Register a callback that will be called after the animation has finished playing (not in reverse). */
        void set_end_callback(const Callback& end_callback) {
            mend_callback = end_callback;
        }

        const Callback& end_reversed_callback() const {
            return mend_reversed_callback;
        }

        /** Register a callback that will be called after the animation has finished playing in reverse. */
        void set_end_reversed_callback(const Callback& end_reversed_callback) {
            mend_reversed_callback = end_reversed_callback;
        }

        /** Return the index of the animation. Mostly intended for internal use. */
        int index() const {
            return mindex;
        }
        /** Set the index of the animation. Mostly intended for internal use. */
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

#endif //LGUI_ANIMATION_H
