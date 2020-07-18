#ifndef LGUI_ANIMATIONFACILITIES_H
#define LGUI_ANIMATIONFACILITIES_H

#include "animationcontext.h"
#include "animationbuilder.h"

namespace lgui {

class AnimationFacilities {
    public:

        template<typename T>
        ValueAnimationBuilderWithContext<T> animate_value() {
            return ValueAnimationBuilderWithContext<T>(mcontext);
        }

        TransformationAnimationBuilderWithContext animate_transform() {
            return TransformationAnimationBuilderWithContext(mcontext);
        }

        template<typename... Args>
        SimultaneousAnimationsBuilderWithContext simultaneous(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            return SimultaneousAnimationsBuilderWithContext (mcontext, std::forward<Args>(args)...);
        }

        template<typename... Args>
        AnimationSequenceBuilderWithContext sequence(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            return AnimationSequenceBuilderWithContext (mcontext, std::forward<Args>(args)...);
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
