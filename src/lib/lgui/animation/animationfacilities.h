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
        SimultaneousAnimations& simultaneous(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            std::unique_ptr<SimultaneousAnimations> simul = std::make_unique<SimultaneousAnimations>();
            lgui::dtl::ani_adder(*simul, std::forward<Args>(args)...);
            SimultaneousAnimations& ref = *simul;
            mcontext.take(std::move(simul));
            return ref;
        }

        template<typename... Args>
        AnimationSequence& sequence(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            std::unique_ptr<AnimationSequence> sequence = std::make_unique<AnimationSequence>();
            lgui::dtl::ani_adder(*sequence, std::forward<Args>(args)...);
            AnimationSequence& ref = *sequence;
            mcontext.take(std::move(sequence));
            return ref;
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
