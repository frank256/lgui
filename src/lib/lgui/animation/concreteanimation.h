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

#ifndef LGUI_CONCRETEANIMATION_H
#define LGUI_CONCRETEANIMATION_H

#include "animationhandler.h"
#include "animation.h"

namespace lgui {

/** An animation that adds itself to the AnimationHandler when it is played and removes itself when it is finished. */
class ConcreteAnimation : public Animation {
        friend class dtl::AnimationHandler;

    public:
        /** This is called by AnimationHandler to update the animation. */
        virtual void update(double timestamp, double elapsed_time) = 0;

        void start() override {
            if (!is_playing()) {
                Animation::start();
                dtl::AnimationHandler::instance().register_animation(*this);
            }
        }

        void end() override {
            if (is_playing()) {
                dtl::AnimationHandler::instance().deregister_animation(*this);
                Animation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                dtl::AnimationHandler::instance().deregister_animation(*this);
                Animation::cancel();
            }
        }

        void start_reverse() override {
            if (!is_playing()) {
                dtl::AnimationHandler::instance().register_animation(*this);
                Animation::start_reverse();
            }
        }

        void end_reverse() override {
            if (is_playing()) {
                dtl::AnimationHandler::instance().deregister_animation(*this);
                Animation::end_reverse();
            }
        }

        bool is_registered() const { return mis_registered; }

        bool can_delete() const override {
            return !is_playing() && !mis_registered;
        }

    private:
        bool mis_registered = false;
};

}

#endif //LGUI_CONCRETEANIMATION_H
