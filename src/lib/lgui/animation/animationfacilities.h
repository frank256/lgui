#ifndef LGUI_ANIMATIONFACILITIES_H
#define LGUI_ANIMATIONFACILITIES_H

#include "animationcontext.h"
#include "animationbuilder.h"

namespace lgui {

class AnimationFacilities {
    public:
        AnimationCompositionBuilderWithContext compose() {
            return AnimationCompositionBuilderWithContext(mcontext);
        }

        template<typename T>
        ValueAnimationBuilderWithContext<T> animate_value() {
            return ValueAnimationBuilderWithContext<T>(mcontext);
        }

        TransformationAnimationBuilderWithContext animate_transform() {
            return TransformationAnimationBuilderWithContext(mcontext);
        }

        void clear_context() {
            mcontext.clear();
        }

        void print_context_info() {
            mcontext.test_print();
        }

    private:
        AnimationContext mcontext;
};

}


#endif //LGUI_ANIMATIONFACILITIES_H
