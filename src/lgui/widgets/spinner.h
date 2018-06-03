/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

        virtual void draw(const DrawEvent &de) const override;

        Widget* get_child_at(int x, int y) override;
        void child_about_to_die(Widget& child) override;
        void resized(const Size& old_size) override;

        int value() const { return mvalue; }
        void set_value(int value);

        int min_value() const { return mmin_value; }
        void set_min_value(int min_value);

        int max_value() const { return mmax_value; }
        void set_max_value(int max_value);

    protected:
        void key_char(KeyEvent &event) override;
        void mouse_wheel_up(MouseEvent &event) override;
        void mouse_wheel_down(MouseEvent &event) override;
        bool is_char_insertable(int c) const override;
        void style_changed()  override;
        void _recursive_configure(const Widget::ConfigInfo& ci) override;

        bool validate(const std::string &s) const override;
    private:
        void change_value(int new_val, bool select=false);

        void text_changed(const std::string& text);

        void increase_pressed();
        void decrease_pressed();
        void layout_buttons();
        void write_value(bool select = false);
        using TextField::set_text;
        using TextField::on_text_changed;
        using TextField::set_validation_enabled;
        using TextField::set_validator;

        HelperButton mbt_increase, mbt_decrease;
        int mvalue, mmin_value, mmax_value;
        bool mwriting_value;
};

}

#endif // LGUI_SPINNER_H
