#include "timertickevent.h"
#include <cinttypes>

namespace lgui {

std::string TimerTickEvent::to_string() const {
    char buf[512];


    snprintf(buf, 512, "-- %.2f: TimerTickEvent count: %" PRId64,
             timestamp(), tick_count());
    return std::string(buf);
}

}
