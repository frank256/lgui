#ifndef LGUITEST_ANIMATIONBUILDER_H
#define LGUITEST_ANIMATIONBUILDER_H

#include "valueanimation.h"
#include "animationcomposition.h"
#include "lgui/platform/error.h"
#include "animationcontext.h"

namespace lgui {

template<class Self>
class AnimationCompositionBuilderBase {
    public:

        Self& first(Animation& first) {
            mcomposition->add_at_start(first);
            mlast_reference = &first;
            return static_cast<Self&>(*this);
        }

        Self& together_with(Animation& ani) {
            ASSERT(mlast_reference);
            mcomposition->add_with(ani, *mlast_reference);
            return static_cast<Self&>(*this);
        }

        template<typename ...Anis>
        Self& together_with(Animation& ani, Anis... remAnis) {
            together_with(ani);
            if constexpr(sizeof...(remAnis) > 0) {
                return together_with(remAnis...);
            }
            return static_cast<Self&>(*this);
        }

        Self& then(Animation& ani) {
            ASSERT(mlast_reference);
            mcomposition->add_after(ani, *mlast_reference);
            mlast_reference = &ani;
            return static_cast<Self&>(*this);
        }

        Self& then_call(const AnimationComposition::Callback& callback) {
            ASSERT(mlast_reference);
            mcomposition->set_callback_after(callback, *mlast_reference);
            return static_cast<Self&>(*this);
        }

    protected:
        AnimationComposition* mcomposition = nullptr;
        Animation* mlast_reference = nullptr;

};

class AnimationCompositionBuilderWithContext
        : public AnimationCompositionBuilderBase<AnimationCompositionBuilderWithContext> {
    public:
        using Self = AnimationCompositionBuilderWithContext;

        explicit AnimationCompositionBuilderWithContext(AnimationContext& context)
                : mcontext(context), mcompostion_instance{std::make_unique<AnimationComposition>()} {
            mcomposition = &*mcompostion_instance;
        }

        Self& first(std::unique_ptr<Animation>&& first) {
            mlast_reference = &*first;
            mcomposition->add_at_start(std::move(first));
            return static_cast<Self&>(*this);
        }

        Self& together_with(std::unique_ptr<Animation>&& ani) {
            ASSERT(mlast_reference);
            mcomposition->add_with(std::move(ani), *mlast_reference);
            return static_cast<Self&>(*this);
        }

        template<typename ...Anis>
        Self& together_with(std::unique_ptr<Animation>&& ani, Anis&& ... remAnis) {
            together_with(std::move(ani));
            if constexpr(sizeof...(remAnis) > 0) {
                return together_with(remAnis...);
            }
            return static_cast<Self&>(*this);
        }

        Self& then(std::unique_ptr<Animation>&& ani) {
            ASSERT(mlast_reference);
            Animation* _ani = &*ani;
            mcomposition->add_after(std::move(ani), *mlast_reference);
            mlast_reference = _ani;
            return *this;
        }

        AnimationComposition& build() {
            mcontext.take(std::move(mcompostion_instance));
            return *mcomposition;
        }

    private:
        AnimationContext& mcontext;
        std::unique_ptr<AnimationComposition> mcompostion_instance;
};

class AnimationCompositionConfigurer : public AnimationCompositionBuilderBase<AnimationCompositionConfigurer> {
    public:
        /** C'tor with a reference: this is more a configurer than a builder: pass in and modify an existing object. */
        explicit AnimationCompositionConfigurer(AnimationComposition& animation_composition) {
            mcomposition = &animation_composition;
        }

        /** Just return the reference that was passed in. */
        AnimationComposition& done() {
            return *mcomposition;
        }
};

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

    protected:
        ValueAnimation<T>* manimation;
};

template<typename T>
class ValueAnimationConfigurer : public ValueAnimationBuilderBase<T, ValueAnimationConfigurer<T>> {
    public:
        using Self = ValueAnimationConfigurer<T>;

        explicit ValueAnimationConfigurer(ValueAnimation<T>& animation)
                : ValueAnimationBuilderBase<T, Self>(animation) {
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

}

#endif //LGUITEST_ANIMATIONBUILDER_H
