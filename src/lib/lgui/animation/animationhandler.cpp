#include "animationhandler.h"
#include "lgui/vector_utils.h"
#include "lgui/timertickevent.h"
#include "concreteanimation.h"

namespace lgui {
namespace  dtl {

AnimationHandler AnimationHandler::minstance;

void AnimationHandler::register_animation(ConcreteAnimation& animation) {
    if (!animation.mis_registered) {
        manimations.push_back(&animation);
        animation.mis_registered = true;
    }
}

void AnimationHandler::deregister_animation(ConcreteAnimation& animation) {
    erase_remove(manimations, &animation);
    animation.mis_registered = false;
}

void AnimationHandler::update(const TimerTickEvent& timer_event) {
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

