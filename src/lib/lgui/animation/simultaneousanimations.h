/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUI_SIMULTANEOUSANIMATIONS_H
#define LGUI_SIMULTANEOUSANIMATIONS_H

#include <vector>
#include "animation.h"

namespace lgui {

/** Animation that wraps several other animations to play them all simultaneously. It will technically end after all
    animations have stopped playing. Note that *no* care is taken to play animations in reverse if they do not have
    the same duration. Note that this is currently thought of to be more or less immutable once animations are added.
 */
class SimultaneousAnimations : public Animation, public AnimationListener {
    public:
        SimultaneousAnimations() = default;
        SimultaneousAnimations(const SimultaneousAnimations& other) = delete;

        /** Add an animation to be played simultaneously, not taking ownership. */
        void add(Animation* ani) {
            ani->set_animation_listener(this);
            mchildren.push_back(ani);
        }

        /** Add an animation to be played simultaneously, taking ownership. Note the unique_ptr will be moved from. */
        void add(std::unique_ptr<Animation>&& ani) {
            add(ani.get());
            mowned_children.push_back(std::move(ani));
        }

        void start() override {
            if (!is_playing()) {
                mplaying_count = 0;
                for (auto& ani : mchildren) {
                    ani->start();
                    ++mplaying_count;
                }
                Animation::start();
            }
        }

        void end() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->end();
                    --mplaying_count;
                }
                Animation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->cancel();
                    --mplaying_count;
                }
                Animation::cancel();
            }
        }

        void start_reverse() override {
            if (!is_playing()) {
                mplaying_count = 0;
                for (auto& ani : mchildren) {
                    ani->start_reverse();
                    ++mplaying_count;
                }
                Animation::start_reverse();
            }
        }

        void end_reverse() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->end_reverse();
                    --mplaying_count;
                }
                Animation::end_reverse();
            }
        }

        void reverse() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    if (ani->is_playing()) {
                        ani->reverse();
                    }
                }
                Animation::reverse();
            }
        }

        /** Used internally. */
        void animation_ended(Animation&) override {
            --mplaying_count;
            if (mplaying_count == 0) {
                Animation::end();
            }
        }

        /** Used internally. */
        void animation_ended_reversed(Animation&) override {
            --mplaying_count;
            if (mplaying_count == 0) {
                Animation::end_reverse();
            }
        }

    protected:
        std::vector<Animation*> mchildren;
        std::vector<std::unique_ptr<Animation>> mowned_children;
        int mplaying_count = 0;
};

}

#endif //LGUI_SIMULTANEOUSANIMATIONS_H
