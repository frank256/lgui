#ifndef LGUITEST_VALUEANIMATION_H
#define LGUITEST_VALUEANIMATION_H

#include "animation.h"
#include <functional>
#include <utility>

namespace lgui {

class ValueAnimationBase : public Animation {
    public:

        void start() override { mt = 0; Animation::start(); }
        void end() override { mt = 1; Animation::end(); }
        void cancel() override { Animation::cancel(); }

        void update(double timestamp, double elapsed_time) override;
        void set_interpolator(const std::function<float(float)>& interpolator) {
            minterpolator = interpolator;
        }
        float duration() const {
            return mduration;
        }
        void set_duration(float duration) {
            mduration = duration;
        }

    protected:
        virtual void update_from_t(float t) = 0;

    private:
        std::function<float(float)> minterpolator = [](float t) { return t; };
        float mt = 0.0;
        float mduration = 0.3;
};

template<typename T>
class ValueAnimation : public ValueAnimationBase {
    public:
        ValueAnimation() : mstart_value(0.0), mend_value(1.0) {}

        explicit ValueAnimation(const std::function<void(T)>& value_setter)
                : mvalue_setter(value_setter) {}

        ValueAnimation(const std::function<void(T)>& value_setter,
                       const T& start_value, const T& end_value)
                : mvalue_setter(value_setter),
                  mstart_value(start_value), mend_value(end_value) {}

        void set_start_value(T value) { mstart_value = value; }
        void set_end_value(T value) { mend_value = value; }

        T start_value() const { return mstart_value; }
        T end_value() const { return mend_value; }

        void set_value_setter(const std::function<void(T)>& value_setter) {
            mvalue_setter = value_setter;
        }
        void set_value_evaluator(const std::function<T(float)>& value_evaluator) {
            mvalue_evaluator = value_evaluator;
        }

    protected:
        void update_from_t(float t) override { mvalue_setter(mvalue_evaluator(t)); }

    private:
        std::function<void(T)> mvalue_setter;
        std::function<T(float)> mvalue_evaluator = [this](float t) { return mstart_value + (mend_value - mstart_value) * t; };
        T mstart_value, mend_value;
};


}

#endif //LGUITEST_VALUEANIMATION_H
