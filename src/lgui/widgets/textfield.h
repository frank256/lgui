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

#ifndef LGUI_TEXTFIELD_H
#define LGUI_TEXTFIELD_H

#include "lgui/widget.h"
#include <string>
#include "platform/font.h"
#include "lgui/signal.h"
#include <functional>

namespace lgui {

    /** A widget providing a text field for single-line text input.
     *  You can set a maximum length, work with selections, and react to text being changed.
     */
    class TextField : public Widget
    {
        public:
            explicit TextField(const std::string& initial_text="", const Font* font = nullptr);

            Signal<const std::string& > on_text_changed;
            Signal<const std::string& > on_activated;

            void draw(const DrawEvent& de) const override;

            /** Retrieve the current text. Note this is a reference. */
            const std::string& text() const { return mtext; }

            /** Sets the current text.
             *  @param text the text to set
             *  @param sel_all whether to select the new text */
            void set_text(const std::string& text, bool sel_all=true);

            /** Returns the cursor position.
             *  @return the cursor position; valid positions are [0, text().size()]   */
            size_t cursor_position() const { return mcursor_pos; }

            /** Retrieve the anchor position. This is where the selection "starts" (it always "ends" at the
             *  cursor).
             * @return the anchor positon; if std::string::npos is returned, it means no selection currently
             *         exists. */
            size_t sel_anchor_position() const { return msel_anchor; }

            /** Sets the cursor position.
             *  @param pos the cursor position; valid positions are [0, text().size()]   */
            void set_cursor_position(size_t pos);

            /** Sets both anchor and cursor positions. A selection will always span from anchor to cursor
             *  position. Pass std::string::npos for the anchor to select nothing and just move the cursor. */
            void select(size_t anchor, size_t cursor);

            /** Clears the selection. */
            void select_none();
            /** Selects all of the text in the text field. */
            void select_all();

            /** Retrieve the maximum allowed length - in code-points. */
            size_t max_length() const { return mmax_length; }
            /** Sets the maximum allowed length - in code points.
             *  Default value is 32767, there is a maximum of 65535 imposed. */
            void set_max_length(size_t length);

            bool is_validation_enabled() const { return mvalidation_enabled; }

            /** Return whether validation is enabled. It will automatically be enabled / disabled when you set
             *  a validator (that is empty). This can be used to temporarily switch off validation for example.
             */
            void set_validation_enabled(bool enable) { mvalidation_enabled = enable; }

            /** Sets a validator on the text field. For every change that the user is going to apply (typing /
             *  removing a character or removing a selection), the validator is called with the expected
             *  result of the operation before it is actually carried out. It should return `true` to allow
             *  the operation or `false` to deny it. Validation is automatically enabled if this is set.
             *  Pass in a nullptr to disable validation. */
            void set_validator(const std::function<bool (const std::string&)>& validator);

            /** Change the size. Currently, TextField will not change its height. */
            void set_size(Size s) override;
            using Widget::set_size;

            void set_font(const Font* font) override;

            Size min_size_hint() override;
            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;

        protected:
            void key_char(KeyEvent& event) override;
            void key_pressed(KeyEvent& event) override;
            void mouse_pressed(MouseEvent& event) override;
            void mouse_dragged(MouseEvent& event) override;
            void mouse_released(MouseEvent& event) override;
            void mouse_clicked(MouseEvent& event) override;
            void style_changed() override;
            void resized(const Size& old_size) override;
            void timer_ticked(const TimerTickEvent& event) override;

            /** Called before an attempt is made to insert the character; can be used to filter the range of
                characters generally allowed to be inserted. Returning true from this method does not mean the
                character will actually be inserted. Default implementation will allow all characters. */
            virtual bool is_char_insertable(int c) const;

            virtual bool validate(const std::string& s) const;

            Padding mpadding, mtext_margins;
        private:
            bool may_delete_char(size_t pos) const;
            bool may_replace_selection(const std::string* repl=nullptr) const;
            bool may_insert_char(int c) const;

            void delete_char();
            void delete_selection();
            void insert_char(int c);

            void double_clicked();

            void maybe_scroll(bool deleted_something=false);
            int get_char_pos(int x) const;
            size_t find_next_word_boundary(bool backwards);
            void update_cursor_pos_px();
            static bool is_move_key(int key);

            std::string mtext;
            size_t mcursor_pos, msel_anchor;
            int mcursor_pos_px, mscroll_pos_px;
            size_t mmax_length;
            bool mdouble_clicked;
            double mlast_clicked_timestamp, mlast_pressed_timestamp;
            int mcursor_width;
            std::function<bool (const std::string& text)> mvalidator;
            bool mvalidation_enabled, mcursor_blinking_status;
            // global?
            static const double constexpr mclick_intervall = 0.4;
    };
}

#endif // LGUI_TEXTFIELD_H

