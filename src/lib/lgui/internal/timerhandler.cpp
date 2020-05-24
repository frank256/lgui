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

#include <lgui/vector_utils.h>
#include "timerhandler.h"
#include "lgui/widget.h"
#include "timerhandler.h"
#include "lgui/timertickevent.h"

namespace lgui {

namespace dtl {

void TimerHandler::subscribe_to_timer_ticks(Widget& w)
{
    if(!contains(mwidgets_subscribed_to_timer_ticks, &w)) {
        if (mdistributing_timer_ticks) {
            if(!contains(mwidgets_timer_ticks_subscriptions_queue, &w)) {
                mwidgets_timer_ticks_subscriptions_queue.push_back(&w);
            }
        }
        else {
            mwidgets_subscribed_to_timer_ticks.push_back(&w);
            erase_remove(mwidgets_subscribed_to_timer_ticks, (Widget*) nullptr);
        }
    }
}

void TimerHandler::unsubscribe_from_timer_ticks(Widget& w) {
    if (!mdistributing_timer_ticks) {
        erase_remove(mwidgets_subscribed_to_timer_ticks, &w);
        erase_remove(mwidgets_subscribed_to_timer_ticks, (Widget*) nullptr);
    }
    else {
        for (auto& subscribed_widget : mwidgets_subscribed_to_timer_ticks) {
            if (subscribed_widget == &w) {
                subscribed_widget = nullptr;
                break;
            }
        }
        erase_remove(mwidgets_timer_ticks_subscriptions_queue, &w);
    }
}

void TimerHandler::handle_timer_tick(const ExternalEvent& event)
{
    mdistributing_timer_ticks = true;
    TimerTickEvent tte(event.timestamp, event.timer.count);

    manimation_handler.update(tte);

    for (Widget* w : mwidgets_subscribed_to_timer_ticks) {
        if (w && event.timer.count % w->timer_tick_skip_mod() == 0)
            w->timer_ticked(tte);
    }
    mdistributing_timer_ticks = false;
    if (!mwidgets_timer_ticks_subscriptions_queue.empty()) {
        for (Widget* w : mwidgets_timer_ticks_subscriptions_queue) {
            if (w && w->is_added_to_gui())
                mwidgets_subscribed_to_timer_ticks.push_back(w);
        }
        mwidgets_timer_ticks_subscriptions_queue.clear();
    }
}

}

}