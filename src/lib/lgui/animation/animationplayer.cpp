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

#include "animationplayer.h"
#include "lgui/vector_utils.h"
#include "lgui/timertickevent.h"
#include "concreteanimation.h"

namespace lgui {
namespace  dtl {

std::unique_ptr<AnimationPlayer> AnimationPlayer::minstance;

void AnimationPlayer::register_animation(ConcreteAnimation& animation) {
    if (!animation.mis_registered) {
        manimations.push_back(&animation);
        animation.mis_registered = true;
    }
}

void AnimationPlayer::deregister_animation(ConcreteAnimation& animation) {
    erase_remove(manimations, &animation);
    animation.mis_registered = false;
}

void AnimationPlayer::update(const TimerTickEvent& timer_event) {
    double elapsed = timer_event.timestamp() - mlast_timestamp;
    if (mlast_timestamp == 0) {
        elapsed = 0;
    }
    for (ConcreteAnimation* ani : manimations) {
        ani->update(timer_event.timestamp(), elapsed);
    }
    mlast_timestamp = timer_event.timestamp();
}

}
}

