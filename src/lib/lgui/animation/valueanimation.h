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

#ifndef LGUI_VALUEANIMATION_H
#define LGUI_VALUEANIMATION_H

#include <functional>
#include <utility>

#include "concreteanimation.h"
#include "lgui/lgui_types.h"
#include "lgui/platform/color.h"

namespace lgui {

/** Value animation base class. */
class ValueAnimationBase : public ConcreteAnimation {
    public:
        using Interpolator = std::function<float(float)>;

        void start() override {
            mt = 0;
            ConcreteAnimation::start();
        }

        void end() override {
            mt = 1;
            ConcreteAnimation::end();
        }

        void cancel() override { ConcreteAnimation::cancel(); }

        void start_reverse() override {
            mt = 1;
            ConcreteAnimation::start_reverse();
        }

        void end_reverse() override {
            mt = 0;
            ConcreteAnimation::end_reverse();
        }

        void reverse() override {
            Animation::reverse();
        }

        void update(double timestamp, double elapsed_time) override;

        /** Set an interpolation function. This is a function mapping from/to the interval of 0..1. It can be used to alter
         *  the course of the animation, i.e. to make it progress in a non-linear way (e.g. appear to start/end slower
         *  while appearing faster in the middle). The default is the identity function so that the animation will be
         *  linear. */
        void set_interpolator(const Interpolator& interpolator) {
            minterpolator = interpolator;
        }

        /** Return the animation's duration in seconds. */
        float duration() const {
            return mduration;
        }

        /** Set the animation's duration in seconds. */
        void set_duration(float duration) {
            mduration = duration;
        }

        /** Return how far the animation has progressed as a value between 0 and 1. */
        float t() const {
            return mt;
        }

    protected:
        virtual void update_from_t(float t) = 0;

    private:
        Interpolator minterpolator = [](float t) { return t; };
        float mt = 0.0;
        float mduration = 0.3;
};

/** Generic value animation. Providing a value setter function is mandatory for the animation to have any effect.
    Per default, you can animate any type that has a meaningful implementation of +, - operators and * (multiplication)
    with a float value. It is, however, possible to add evaluator template specializations for other types as well which
    will alleviate these requirements. */
template<typename T>
class ValueAnimation : public ValueAnimationBase {
    public:
        using ValueSetter = std::function<void(T)>;

        ValueAnimation() = default;

        /** C'tor directly setting a value setter. */
        explicit ValueAnimation(const ValueSetter& value_setter)
                : mvalue_setter(value_setter) {}

        /** C'tor directly setting a value setter and the start and end values. */
        ValueAnimation(const ValueSetter& value_setter,
                       const T& start_value, const T& end_value)
                : mvalue_setter(value_setter),
                  mstart_value(start_value), mend_value(end_value) {}

        /** Set the start value for the animation. */
        void set_start_value(T value) { mstart_value = value; }
        /** Set the end value for the animation. */
        void set_end_value(T value) { mend_value = value; }

        /** Return the start value for the animation. */
        T start_value() const { return mstart_value; }
        /** Return the end value for the animation. */
        T end_value() const { return mend_value; }

        /** Set a value setter*/
        void set_value_setter(const ValueSetter& value_setter) {
            mvalue_setter = value_setter;
        }

    protected:
        void update_from_t(float t) override { mvalue_setter(evaluate(t)); }

        virtual T evaluate(float t) {
            return mstart_value + (mend_value - mstart_value) * t;
        }

    private:
        ValueSetter mvalue_setter;
        T mstart_value, mend_value;
};

/** Evaluator template specialization so you can animate Rect values with ValueAnimation. */
template<>
inline Rect ValueAnimation<Rect>::evaluate(float t) {
    PointF p = (PointF(mend_value.pos()) - PointF(mstart_value.pos())) * t;
    PointF s = (PointF(mend_value.size().to_point()) - PointF(mstart_value.size().to_point())) * t;
    return Rect(mstart_value.pos() + p.to_point_rounded(),
                mstart_value.size() + Size(s.to_point_rounded()));
}

/** Evaluator template specialization so you can animate Color values with ValueAnimation. */
template<>
inline Color ValueAnimation<Color>::evaluate(float t) {
    return {
            .r = mstart_value.r + (mend_value.r - mstart_value.r) * t,
            .g = mstart_value.g + (mend_value.g - mstart_value.g) * t,
            .b = mstart_value.b + (mend_value.b - mstart_value.b) * t,
            .a = mstart_value.a + (mend_value.a - mstart_value.a) * t
    };
}

}

#endif //LGUI_VALUEANIMATION_H
