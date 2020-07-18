#ifndef LGUI_ANIMATIONBUILDER_H
#define LGUI_ANIMATIONBUILDER_H

#include "valueanimation.h"
#include "lgui/platform/error.h"
#include "animationcontext.h"
#include "transformationanimation.h"

#include "animationsequence.h"
#include "simultaneousanimations.h"

namespace lgui {

template<class T, class Self>
class ValueAnimationBuilderBase {
    public:
        Self& from(T from) {
            manimation->set_start_value(from);
            return static_cast<Self&>(*this);
        }

        Self& to(T to) {
            manimation->set_end_value(to);
            return static_cast<Self&>(*this);
        }

        Self& with_value_setter(const typename ValueAnimation<T>::ValueSetter& setter) {
            manimation->set_value_setter(setter);
            return static_cast<Self&>(*this);
        }

        Self& with_duration(float seconds) {
            manimation->set_duration(seconds);
            return static_cast<Self&>(*this);
        }

        Self& with_interpolator(const typename ValueAnimation<T>::Interpolator& interpolator) {
            manimation->set_interpolator(interpolator);
            return static_cast<Self&>(*this);
        }

        Self& then_call(const AbstractAnimation::Callback& callback) {
            manimation->set_end_callback(callback);
        }

    protected:
        ValueAnimation<T>* manimation;
};

template<typename T>
class ValueAnimationConfigurer : public ValueAnimationBuilderBase<T, ValueAnimationConfigurer<T>> {
    public:
        using Self = ValueAnimationConfigurer<T>;

        explicit ValueAnimationConfigurer(ValueAnimation<T>& animation) {
            this->manimation = &animation;
        }

        ValueAnimation<T>& value_animation() {
            return *this->manimation;
        }
};

template<typename T>
class ValueAnimationBuilder : public ValueAnimationBuilderBase<T, ValueAnimationBuilder<T>> {
    public:
        using Self = ValueAnimationBuilder<T>;

        ValueAnimationBuilder()
                : manimation_instance(std::make_unique<ValueAnimation<T>>()) {
            this->manimation = &*manimation_instance.get();
        }

        std::unique_ptr<ValueAnimation<T>> build() {
            this->manimation = nullptr;
            return std::move(this->manimation_instance);
        }

    private:
        std::unique_ptr<ValueAnimation<T>> manimation_instance;
};

template<typename T>
class ValueAnimationBuilderWithContext : public ValueAnimationBuilderBase<T, ValueAnimationBuilderWithContext<T>> {
    public:
        using Self = ValueAnimationBuilder<T>;

        explicit ValueAnimationBuilderWithContext(AnimationContext& context)
                : mcontext(context), manimation_instance(std::make_unique<ValueAnimation<T>>()) {
            this->manimation = manimation_instance.get();
        }

        ValueAnimation<T>& build() {
            mcontext.take(std::move(manimation_instance));
            return *this->manimation;
        }

    private:
        AnimationContext& mcontext;
        std::unique_ptr<ValueAnimation<T>> manimation_instance;
};

template<class Self>
class TransformationAnimationBuilderBase {
    public:

        Self& scale(float from, float to) {
            manimation->set_start_scale(PointF(from, from));
            manimation->set_end_scale(PointF(to, to));
            return static_cast<Self&>(*this);
        }

        Self& scale(PointF from, PointF to) {
            manimation->set_start_scale(from);
            manimation->set_end_scale(to);
            return static_cast<Self&>(*this);
        }

        Self& translate(PointF from, PointF to) {
            manimation->set_start_translation(from);
            manimation->set_end_translation(to);
            return static_cast<Self&>(*this);
        }

        Self& translate_z(float from, float to) {
            manimation->set_start_translation_z(from);
            manimation->set_end_translation_z(to);
            return static_cast<Self&>(*this);
        }

        Self& rotate(float from, float to) {
            manimation->set_start_rotation_z(from);
            manimation->set_end_rotation_z(to);
            return static_cast<Self&>(*this);
        }

        Self& rotate_y(float from, float to) {
            manimation->set_start_rotation_y(from);
            manimation->set_end_rotation_y(to);
            return static_cast<Self&>(*this);
        }

        Self& rotate_x(float from, float to) {
            manimation->set_start_rotation_x(from);
            manimation->set_end_rotation_x(to);
            return static_cast<Self&>(*this);
        }

        Self& with_duration(float seconds) {
            manimation->set_duration(seconds);
            return static_cast<Self&>(*this);
        }

        Self& with_interpolator(const TransformationAnimation::Interpolator& interpolator) {
            manimation->set_interpolator(interpolator);
            return static_cast<Self&>(*this);
        }

        Self& target(WidgetTransformation& wt) {
            manimation->set_target(wt);
            return static_cast<Self&>(*this);
        }

        Self& then_call(const AbstractAnimation::Callback& callback) {
            manimation->set_end_callback(callback);
            return static_cast<Self&>(*this);
        }

    protected:
        TransformationAnimation* manimation;
};

class TransformationAnimationConfigurer : public TransformationAnimationBuilderBase<TransformationAnimationConfigurer> {
    public:
        using Self = TransformationAnimationConfigurer;

        explicit TransformationAnimationConfigurer(TransformationAnimation& animation)
                : TransformationAnimationBuilderBase() {
            manimation = &animation;
        }

        TransformationAnimation& transformation_animation() {
            return *this->manimation;
        }
};

class TransformationAnimationBuilder : public TransformationAnimationBuilderBase<TransformationAnimationBuilder> {
    public:
        using Self = TransformationAnimationBuilder;

        explicit TransformationAnimationBuilder(WidgetTransformation& transformation)
                : TransformationAnimationBuilder() {
            target(transformation);
        }

        TransformationAnimationBuilder()
                : TransformationAnimationBuilderBase(),
                  manimation_instance(std::make_unique<TransformationAnimation>()) {
            this->manimation = manimation_instance.get();
        }

        std::unique_ptr<TransformationAnimation> build() {
            this->manimation = nullptr;
            return std::move(this->manimation_instance);
        }

    private:
        std::unique_ptr<TransformationAnimation> manimation_instance;
};


class TransformationAnimationBuilderWithContext
        : public TransformationAnimationBuilderBase<TransformationAnimationBuilderWithContext> {
    public:
        using Self = TransformationAnimationBuilderWithContext;

        explicit TransformationAnimationBuilderWithContext(AnimationContext& context)
                : TransformationAnimationBuilderBase(),
                  mcontext(context), manimation_instance(std::make_unique<TransformationAnimation>()) {
            this->manimation = manimation_instance.get();
        }

        TransformationAnimation& build() {
            mcontext.take(std::move(manimation_instance));
            return *this->manimation;
        }

    private:
        AnimationContext& mcontext;
        std::unique_ptr<TransformationAnimation> manimation_instance;
};

namespace dtl {

template<class Aggregate>
void ani_adder(Aggregate& aggregate) {
    (void) aggregate;
}

template<class Aggregate, class Animation, typename ...Animations>
void ani_adder(Aggregate& aggregate, Animation ani, Animations&& ... anis) {
    aggregate.add(std::move(ani));
    ani_adder(aggregate, std::forward<Animations>(anis)...);
}

}

template<typename... Args>
std::unique_ptr<SimultaneousAnimations> simultaneous(Args&& ... args) {
    static_assert(sizeof...(args) > 0);
    std::unique_ptr<SimultaneousAnimations> simul = std::make_unique<SimultaneousAnimations>();
    dtl::ani_adder(*simul, std::forward<Args>(args)...);
    return simul;
}

template<typename... Args>
std::unique_ptr<AnimationSequence> sequence(Args&& ... args) {
    static_assert(sizeof...(args) > 0);
    auto sequence = std::make_unique<AnimationSequence>();
    dtl::ani_adder(*sequence, std::forward<Args>(args)...);
    return sequence;
}

template<typename T>
ValueAnimationBuilder<T> animate_value() {
    return ValueAnimationBuilder<T>();
}

inline TransformationAnimationBuilder animate_transform() {
    return TransformationAnimationBuilder();
}

inline TransformationAnimationBuilder animate_transform(WidgetTransformation& wt) {
    return TransformationAnimationBuilder(wt);
}

}

#endif //LGUI_ANIMATIONBUILDER_H
