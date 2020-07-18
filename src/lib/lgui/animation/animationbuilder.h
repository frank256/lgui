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

#ifndef LGUI_ANIMATIONBUILDER_H
#define LGUI_ANIMATIONBUILDER_H

#include "valueanimation.h"
#include "lgui/platform/error.h"
#include "animationcontext.h"
#include "transformationanimation.h"

#include "animationsequence.h"
#include "simultaneousanimations.h"

namespace lgui {

template<typename T> class AnimationBuilderBase {
    public:
        virtual std::unique_ptr<T> build() = 0;
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

        Self& then_call(const Animation::Callback& callback) {
            manimation->set_end_callback(callback);
            return static_cast<Self&>(*this);
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

        ValueAnimation<T>& get() {
            return *this->manimation;
        }
};

template<typename T>
class ValueAnimationBuilder : public ValueAnimationBuilderBase<T, ValueAnimationBuilder<T>>, public AnimationBuilderBase<ValueAnimation<T>> {
    public:
        using Self = ValueAnimationBuilder<T>;

        ValueAnimationBuilder()
                : manimation_instance(std::make_unique<ValueAnimation<T>>()) {
            this->manimation = &*manimation_instance.get();
        }

        std::unique_ptr<ValueAnimation<T>> build() override {
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

        Self& then_call(const Animation::Callback& callback) {
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

        TransformationAnimation& get() {
            return *this->manimation;
        }
};

class TransformationAnimationBuilder : public TransformationAnimationBuilderBase<TransformationAnimationBuilder>, public AnimationBuilderBase<TransformationAnimation> {
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

        std::unique_ptr<TransformationAnimation> build() override {
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
void animation_adder(Aggregate& aggregate, std::unique_ptr<Animation>&& ani) {
    aggregate.add(std::move(ani));
}

template<class Aggregate, typename T>
void animation_adder(Aggregate& aggregate, AnimationBuilderBase<T>& animation_builder_base) {
    aggregate.add(std::move(animation_builder_base.build()));
}

// Declare it before the other vararg variant...
template<class Aggregate, typename T, typename ...Animations>
void animation_adder(Aggregate& aggregate, AnimationBuilderBase<T>& animation_builder_base, Animations&& ... anis);

template<class Aggregate, typename ...Animations>
void animation_adder(Aggregate& aggregate, std::unique_ptr<Animation>&& ani, Animations&& ... anis) {
    aggregate.add(std::move(ani));
    animation_adder(aggregate, std::forward<Animations>(anis)...);
}

template<class Aggregate, typename T, typename ...Animations>
void animation_adder(Aggregate& aggregate, AnimationBuilderBase<T>& animation_builder_base, Animations&& ... anis) {
    aggregate.add(std::move(animation_builder_base.build()));
    animation_adder(aggregate, std::forward<Animations>(anis)...);
}

}

template<class Self>
class AnimationSequenceBuilderBase {
    public:
        template<typename... Args>
        Self& then(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            dtl::animation_adder(*mseq, std::forward<Args>(args)...);
            return static_cast<Self&>(*this);
        }

        Self& then_call(const Animation::Callback& callback) {
            mseq->set_end_callback(callback);
            return static_cast<Self&>(*this);
        }

    protected:
        AnimationSequence* mseq;
};

class AnimationSequenceBuilder : public AnimationSequenceBuilderBase<AnimationSequenceBuilder>, public AnimationBuilderBase<AnimationSequence> {
    public:
        AnimationSequenceBuilder()
                : AnimationSequenceBuilderBase() {
            mseq_ptr = std::make_unique<AnimationSequence>();
            mseq = &*mseq_ptr;
        }

        template<typename... Args>
        explicit AnimationSequenceBuilder(Args&& ... args)
                : AnimationSequenceBuilder() {
            then(std::forward<Args>(args)...);
        }

        std::unique_ptr<AnimationSequence> build() override {
            return std::move(this->mseq_ptr);
        }

    private:
        std::unique_ptr<AnimationSequence> mseq_ptr;
};

class AnimationSequenceBuilderWithContext : public AnimationSequenceBuilderBase<AnimationSequenceBuilderWithContext> {
    public:
        explicit AnimationSequenceBuilderWithContext(AnimationContext& context)
                : AnimationSequenceBuilderBase(), mcontext(context) {
            mseq_ptr = std::make_unique<AnimationSequence>();
            mseq = &*mseq_ptr;
        }

        template<typename... Args>
        explicit AnimationSequenceBuilderWithContext(AnimationContext& context, Args&& ... args)
                : AnimationSequenceBuilderWithContext(context) {
            then(std::forward<Args>(args)...);
        }

        AnimationSequence& build() {
            mcontext.take(std::move(mseq_ptr));
            return *mseq;
        }

    private:
        AnimationContext& mcontext;
        std::unique_ptr<AnimationSequence> mseq_ptr;
};

template<class Self>
class SimultaneousAnimationsBuilderBase {
    public:
        template<typename... Args>
        Self& with(Args&& ... args) {
            static_assert(sizeof...(args) > 0);
            dtl::animation_adder(*msimul, std::forward<Args>(args)...);
            return static_cast<Self&>(*this);
        }

        Self& then_call(const Animation::Callback& callback) {
            msimul->set_end_callback(callback);
            return static_cast<Self&>(*this);
        }

    protected:
        SimultaneousAnimations* msimul;
};

class SimultaneousAnimationsBuilder : public SimultaneousAnimationsBuilderBase<SimultaneousAnimationsBuilder>, public AnimationBuilderBase<SimultaneousAnimations> {
    public:
        SimultaneousAnimationsBuilder()
                : SimultaneousAnimationsBuilderBase() {
            msimul_ptr = std::make_unique<SimultaneousAnimations>();
            msimul = &*msimul_ptr;
        }

        template<typename... Args>
        explicit SimultaneousAnimationsBuilder(Args&& ... args)
                : SimultaneousAnimationsBuilder() {
            with(std::forward<Args>(args)...);
        }

        std::unique_ptr<SimultaneousAnimations> build() override {
            return std::move(this->msimul_ptr);
        }

    private:
        std::unique_ptr<SimultaneousAnimations> msimul_ptr;
};

class SimultaneousAnimationsBuilderWithContext : public SimultaneousAnimationsBuilderBase<SimultaneousAnimationsBuilderWithContext> {
    public:
        explicit SimultaneousAnimationsBuilderWithContext(AnimationContext& context)
                : SimultaneousAnimationsBuilderBase(), mcontext(context) {
            msimul_ptr = std::make_unique<SimultaneousAnimations>();
            msimul = &*msimul_ptr;
        }

        template<typename... Args>
        explicit SimultaneousAnimationsBuilderWithContext(AnimationContext& context, Args&& ... args)
                : SimultaneousAnimationsBuilderWithContext(context) {
            then(std::forward<Args>(args)...);
        }

        SimultaneousAnimations& build() {
            mcontext.take(std::move(msimul_ptr));
            return *msimul;
        }

    private:
        AnimationContext& mcontext;
        std::unique_ptr<SimultaneousAnimations> msimul_ptr;
};

template<typename... Args>
SimultaneousAnimationsBuilder simultaneous(Args&& ... args) {
    static_assert(sizeof...(args) > 0);
    SimultaneousAnimationsBuilder builder = SimultaneousAnimationsBuilder();
    builder.with(std::forward<Args>(args)...);
    return builder;
}

template<typename... Args>
std::unique_ptr<AnimationSequence> sequence(Args&& ... args) {
    static_assert(sizeof...(args) > 0);
    auto sequence = std::make_unique<AnimationSequence>();
    dtl::animation_adder(*sequence, std::forward<Args>(args)...);
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
