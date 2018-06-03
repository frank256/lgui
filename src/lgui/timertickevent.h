#ifndef LGUI_TIMERTICKEVENT_H
#define LGUI_TIMERTICKEVENT_H

#include <string>
#include <cinttypes>

namespace lgui {

class TimerTickEvent
{
    public:
        TimerTickEvent(double timestamp, int64_t tick_count)
            : mtimestamp(timestamp), mtick_count(tick_count)
        {}

        std::string to_string() const;

        double timestamp() const {
            return mtimestamp;
        }

        int64_t tick_count() const {
            return mtick_count;
        }

    private:
        double mtimestamp;
        int64_t mtick_count;
};

}

#endif // LGUI_TIMERTICKEVENT_H
