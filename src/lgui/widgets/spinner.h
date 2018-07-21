/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_SPINNER_H
#define LGUI_SPINNER_H

#include "textfield.h"
#include "buttons/helperbutton.h"

namespace lgui {

class Spinner : public TextField
{
    public:
        Signal<int> on_value_changed;

        Spinner();

        void draw(const DrawEvent &de) const override;

        Widget* get_child_at(int x, int y) override;
        void child_about_to_die(Widget& child) override;
        void resized(const Size& old_size) override;

        int value() const { return mvalue; }
        void set_value(int value);

        int min_value() const { return mmin_value; }
        void set_min_value(int min_value);

        int max_value() const { return mmax_value; }
        void set_max_value(int max_value);

        int steps() const { return msteps; }
        void set_steps(int steps) { msteps = steps; }

        struct TimerUpdateStage { double time_passed; int steps; };
        static const int MAX_TIMER_UPDATE_STAGES = 3;

        /** Enable automatic updating via timer tick events when one of the buttons is held down.
         *  Be sure to pass timer events into the GUI in your event loop and set
         * `Widget::set_timer_tick_skip_mod()` to a value low enough, otherwise, this will not work.
         *  You can pass in 3 instances of TimerUpdateStage. The `time_passed` value indicates how much time
         *  shall pass before the value is increased / decreased according to the corresponding value of `steps`.
         *  The values of `time_passed` are expected to be in ascending order.
         *  Example:
         *  `set_timer_update_when_held({ { 2.0, 5 }, { 3.0, 10 }, { 4.0, 20 } });`
         *  This means that the value will decrease / increase by 5 after 2 seconds if one of the buttons is
         *  pressed (held down), increase / decrease by 10 after 3 seconds have passed and increase/decrease
         *  by 20 after 4 seconds have passed.
         */
        void set_timer_update_config(const std::array<TimerUpdateStage, MAX_TIMER_UPDATE_STAGES>& config) {
            mtimer_update_config = config;
        }

        /** Return the configuration for automatic updating via timer tick events when one of the buttons is
         *  held down. */
        const std::array<TimerUpdateStage, MAX_TIMER_UPDATE_STAGES>& timer_update_config() const {
            return mtimer_update_config;
        }

    protected:
        void key_char(KeyEvent &event) override;
        void mouse_wheel_up(MouseEvent &event) override;
        void mouse_wheel_down(MouseEvent &event) override;
        void timer_ticked(const TimerTickEvent &event) override;
        bool is_char_insertable(int c) const override;
        void style_changed()  override;
        void visit_down(const std::function<void (Widget&)>& f) override;

        bool validate(const std::string &s) const override;
    private:
        void change_value(int new_val, bool select=false);

        void increase();
        void decrease();
        void increase_button_down();
        void decrease_button_down();
        void helper_button_up();

        void text_changed(const std::string& text);

        void layout_buttons();
        void write_value(bool select = false);
        int get_held_down_stage_steps(double time);

        using TextField::set_text;
        using TextField::on_text_changed;
        using TextField::set_validation_enabled;
        using TextField::set_validator;

        HelperButton mbt_increase, mbt_decrease;
        int mvalue, mmin_value, mmax_value, msteps;
        const HelperButton* mheld_down;
        double mheld_down_since;
        int mheld_down_stage;
        std::array<TimerUpdateStage, MAX_TIMER_UPDATE_STAGES> mtimer_update_config;
        bool mwriting_value;

        static std::array<TimerUpdateStage, MAX_TIMER_UPDATE_STAGES> mdefault_timer_update_config;
};

}

#endif // LGUI_SPINNER_H
