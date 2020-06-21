#ifndef LGUI_VALUEANIMATION_H
#define LGUI_VALUEANIMATION_H

#include <functional>
#include <utility>

#include "animation.h"
#include "lgui/lgui_types.h"

namespace lgui {

class ValueAnimationBase : public Animation {
    public:
        using Interpolator = std::function<float(float)>;

        void start() override {
            mt = 0;
            Animation::start();
        }
        void end() override {
            mt = 1;
            Animation::end();
        }
        void cancel() override { Animation::cancel(); }

        void update(double timestamp, double elapsed_time) override;
        void set_interpolator(const Interpolator& interpolator) {
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
        Interpolator minterpolator = [](float t) { return t; };
        float mt = 0.0;
        float mduration = 0.3;
};

template<typename T>
class ValueAnimation : public ValueAnimationBase {
    public:
        using ValueSetter = std::function<void(T)>;

        ValueAnimation() = default;

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

    protected:
        void update_from_t(float t) override { mvalue_setter(evaluate(t)); }

        virtual T evaluate(float t) {
            return mstart_value + (mend_value - mstart_value) * t;
        }

    private:
        std::function<void(T)> mvalue_setter;
        T mstart_value, mend_value;
};

template<>
inline Rect ValueAnimation<Rect>::evaluate(float t) {
    PointF p = (PointF(mend_value.pos()) - PointF(mstart_value.pos())) * t;
    PointF s = (PointF(mend_value.size().to_point()) - PointF(mstart_value.size().to_point())) * t;
    return Rect(mstart_value.pos() + p.to_point_rounded(),
               mstart_value.size() + Size(s.to_point_rounded()));
}


}

#endif //LGUI_VALUEANIMATION_H
