#include "valueanimation.h"

namespace lgui {

void ValueAnimationBase::update(double timestamp, double elapsed_time) {
    (void) timestamp;
    if (is_playing()) {
        if (!is_reversed()) {
            mt = float((double(mduration) * double(mt) + elapsed_time) / double(mduration));
            bool ended = mt >= 1.0;
            mt = std::min(1.0f, mt);
            update_from_t(minterpolator(mt));
            if (ended) {
                end();
            }
        }
        else {
            mt = float((double(mduration) * double(mt) - elapsed_time) / double(mduration));
            bool ended = mt <= 0.0;
            mt = std::max(0.0f, mt);
            update_from_t(minterpolator(mt));
            if (ended) {
                end_reverse();
            }
        }
    }
}

}