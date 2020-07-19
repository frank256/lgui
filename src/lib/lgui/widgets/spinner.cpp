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

#include "spinner.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "lgui/timertickevent.h"
#include "lgui/platform/events.h"
#include <climits>

namespace lgui {

std::array<Spinner::TimerUpdateStage, Spinner::MAX_TIMER_UPDATE_STAGES> Spinner::mdefault_timer_update_config
        {{{1.0, 1}, {2.0, 2}, {3.0, 5}}};

Spinner::Spinner()
        : mbt_increase(Style::HelperButtonType::IncreaseButton),
          mbt_decrease(Style::HelperButtonType::DecreaseButton),
          mvalue(0), mmin_value(INT_MIN), mmax_value(INT_MAX), msteps(1),
          mheld_down(nullptr), mheld_down_since(0.0), mheld_down_stage(0),
          mtimer_update_config{mdefault_timer_update_config},
          mwriting_value(false) {
    configure_new_child(mbt_increase);
    configure_new_child(mbt_decrease);
    mbt_increase.on_down.connect([this]() { increase_button_down(); });
    mbt_decrease.on_down.connect([this]() { decrease_button_down(); });
    mbt_increase.on_up.connect([this]() { helper_button_up(); });
    mbt_decrease.on_up.connect([this]() { helper_button_up(); });
    mbt_increase.set_focusable(false);
    mbt_decrease.set_focusable(false);
    on_text_changed.connect(&Spinner::text_changed, *this);
    style_changed();
}

void Spinner::draw(const DrawEvent& de) const {
    TextField::draw(de);

    draw_child(mbt_increase, de);
    draw_child(mbt_decrease, de);
}

void Spinner::style_changed() {
    TextField::style_changed();
    mbt_increase.set_style(&style());
    mbt_decrease.set_style(&style());

    int helper_width = std::max(style().get_helper_button_min_size(Style::HelperButtonType::IncreaseButton).w(),
                                style().get_helper_button_min_size(Style::HelperButtonType::DecreaseButton).w());
    mpadding.set_right(mpadding.right() + helper_width);
    layout_buttons();
}

void Spinner::visit_down(const std::function<void(Widget&)>& f) {
    TextField::visit_down(f);
    mbt_increase.visit_down(f);
    mbt_decrease.visit_down(f);
}

bool Spinner::validate(const std::string& s) const {
    // Could use a regex, but don't want to recompile it every time and do not want to expose it in the header.
    // Apart from that, it's really simple...
    if (s.empty())
        return true;
    if (s[0] < '0' || s[0] > '9') {
        if (mmin_value >= 0 || s[0] != '-')
            return false;
    }
    for (size_t i = 1; i < s.size(); ++i) {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}

void Spinner::change_value(int new_val, bool select) {
    new_val = std::max(mmin_value, new_val);
    new_val = std::min(mmax_value, new_val);

    if (mvalue != new_val) {
        mbt_increase.set_active(new_val < mmax_value);
        mbt_decrease.set_active(new_val > mmin_value);
        mvalue = new_val;
        write_value(select);
        on_value_changed.emit(mvalue);
    }
}

void Spinner::text_changed(const std::string& text) {
    if (!mwriting_value) {
        try {
            int value = std::stoi(text);
            change_value(value);
        } catch (std::exception& e) {
            (void) e;
            change_value(0, true);
        }
    }
}

void Spinner::increase() {
    change_value(mvalue + msteps);
}

void Spinner::decrease() {
    change_value(mvalue - msteps);
}


void Spinner::increase_button_down() {
    increase();
    mheld_down = &mbt_increase;
    mheld_down_since = get_time();
    focus();
}

void Spinner::decrease_button_down() {
    decrease();
    mheld_down = &mbt_decrease;
    mheld_down_since = get_time();
    focus();
}

void Spinner::helper_button_up() {
    mheld_down = nullptr;
    mheld_down_stage = 0;
}

Widget* Spinner::get_child_at(PointF p) {
    if (mbt_increase.is_visible() && mbt_increase.rect().contains(p))
        return &mbt_increase;
    else if (mbt_decrease.is_visible() && mbt_decrease.rect().contains(p))
        return &mbt_decrease;
    else
        return TextField::get_child_at(p);
}

void Spinner::child_about_to_die(Widget& child) {
    if (&child == &mbt_increase || &child == &mbt_decrease)
        return;
    TextField::child_about_to_die(child);
}

void Spinner::resized(const Size& old_size) {
    layout_buttons();
    TextField::resized(old_size);
}

void Spinner::set_value(int value) {
    change_value(value);
    write_value();
}

void Spinner::set_min_value(int min_value) {
    mmin_value = min_value;
    change_value(mvalue);
}

void Spinner::set_max_value(int max_value) {
    mmax_value = max_value;
    change_value(mvalue);
}

void Spinner::key_char(KeyEvent& event) {
    if (event.key_code() == Keycodes::KEY_UP) {
        increase();
        event.consume();
    }
    else if (event.key_code() == Keycodes::KEY_DOWN) {
        decrease();
        event.consume();
    }
    else {
        TextField::key_char(event);
    }
}

void Spinner::mouse_wheel_up(MouseEvent& event) {
    increase();
    event.consume();
}

void Spinner::mouse_wheel_down(MouseEvent& event) {
    decrease();
    event.consume();
}

void Spinner::timer_ticked(const TimerTickEvent& event) {
    TextField::timer_ticked(event);
    if (mheld_down) {
        int steps = get_held_down_stage_steps(event.timestamp());
        if (mheld_down == &mbt_decrease)
            change_value(mvalue - steps);
        else
            change_value(mvalue + steps);
    }
}

int Spinner::get_held_down_stage_steps(double time) {
    double delta = time - mheld_down_since;
    int steps = 0;
    for (int stage = mheld_down_stage; stage < MAX_TIMER_UPDATE_STAGES; ++stage) {
        if (delta > mtimer_update_config[stage].time_passed) {
            steps = mtimer_update_config[stage].steps;
            mheld_down_stage = stage;
        }
    }
    return steps;
}

bool Spinner::is_char_insertable(int c) const {
    return (c >= '0' && c <= '9') || c == '-';
}

void Spinner::layout_buttons() {
    Size incr = style().get_helper_button_min_size(Style::IncreaseButton);
    Size decr = style().get_helper_button_min_size(Style::DecreaseButton);
    int h = (font().line_height() + mpadding.vert()) / 2;
    incr.set_h(h);
    decr.set_h(h);
    mbt_increase.set_size(incr);
    mbt_decrease.set_size(decr);
    mbt_increase.set_pos(width() - incr.w(), 0);
    mbt_decrease.set_pos(width() - incr.w(), height() - mbt_decrease.height());
}

void Spinner::write_value(bool select) {
    mwriting_value = true;
    TextField::set_text(std::to_string(mvalue), select);
    mwriting_value = false;
}

}
