#ifndef LGUITEST_ANIMATIONFACILITIES_H
#define LGUITEST_ANIMATIONFACILITIES_H

#include "animationcontext.h"
#include "animationbuilder.h"

namespace lgui {

class AnimationFacilities {
    public:
        AnimationCompositionBuilderWithContext animation_composition() {
            return AnimationCompositionBuilderWithContext(mcontext);
        }

        template<typename T>
        ValueAnimationBuilderWithContext<T> value_animation() {
            return ValueAnimationBuilderWithContext<T>(mcontext);
        }

        void clear_context() {
            mcontext.clear();
        }

    private:
        AnimationContext mcontext;
};

}


#endif //LGUITEST_ANIMATIONFACILITIES_H
