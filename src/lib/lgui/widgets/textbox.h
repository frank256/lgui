/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUI_TEXTBOX_H
#define LGUI_TEXTBOX_H

#include <string>
#include <vector>
#include "scrollbar.h"
#include "../cursorblinkhelper.h"
#include "../widgetpc.h"

namespace lgui {

/** A basic multi-line text editor. */
class TextBox : public WidgetPC {
    public:
        explicit TextBox(const std::string& initial_text = "", const Font* font = nullptr);

        Signal<const std::string&> on_text_changed;

        void draw(const DrawEvent& de) const override;

        void set_text(const std::string& text);
        const std::string& text() const { return mtext; }

        void set_font(const Font* font) override;

        void select_none();
        void select_all();

        void scroll_up();
        void scroll_down();

        void set_read_only(bool read_only);
        bool is_read_only() const { return mread_only; }

        /** Configures how the text is split into lines.
            With the modes `Characters` and `FittingWords`, a horizontal scroll bar will never be needed.
            (If the TextBox's width is sufficient for displaying at least one instance of every character
             ocurring in the text.) */
        enum WrapMode {
            None,        /**< only split at '\n' */
            Characters,  /**< split like a console (may occur anywhere; character-based) */
            WholeWords,  /**< split at word boundaries only */
            FittingWords /**< split at word boundaries, but do allow to break up words that are too wide
                                  to fit on one line */
        };

        /** Configures how to wrap the text.
         *  @param wrap_mode will configure how the text is split into lines. See WrapMode. The default is
         *         WrapMode::FittingWords.
         */
        void set_wrap_mode(WrapMode wrap_mode);

        /** Retrieve the %TextBox's instance of CursorBlinkHelper to query cursor blinking. */
        const CursorBlinkHelper& cursor_blink() const { return mcursor_blink_helper; }

        /** Retrieve the %TextBox's instance of CursorBlinkHelper to configure cursor blinking. */
        CursorBlinkHelper& cursor_blink() { return mcursor_blink_helper; }


        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
        Size min_size_hint() override;

    protected:
        void style_changed() override;
        void resized(const Size& old_size) override;

        void mouse_pressed(MouseEvent& event) override;
        void mouse_released(MouseEvent& event) override;
        void mouse_dragged(MouseEvent& event) override;
        void mouse_wheel_down(MouseEvent& event) override;
        void mouse_wheel_up(MouseEvent& event) override;
        void timer_ticked(const TimerTickEvent& event) override;
        void focus_gained(FocusEvent& event) override;
        void focus_lost(FocusEvent& event) override;

        void key_char(KeyEvent& event) override;

    private:
        void update_rows(bool second_pass = false);
        int width_available() const;
        int height_available() const;

        void make_rows_wordwrap(const std::string& text, int max_width, WrapMode wm);
        void make_rows_newlines(const std::string& text);

        void do_update_rows();
        void add_row(const std::string& text, size_t offs, size_t size, int split_c_w);
        bool move_caret_keyboard(const KeyEvent& ke);
        bool keyboard_hotkeys(const KeyEvent& ke);

        enum class PosColMode {
                Fixed, Wrap, Search
        };
        void position_caret_keyboard(Point nrc, PosColMode pcm);
        bool update_caret_position_from_mouse(Point mp, bool clip_outside_margins);
        void update_caret_tpx_location();
        void update_selection();
        void remove_selection();
        bool remove_chr(bool backspace);
        int type_chr(int c);
        int insert_string(const std::string& str);
        void scroll_to_caret();

        bool has_selection() const;
        std::string get_selection_text() const;
        static bool is_move_key(int key);

        Point tpx_to_rowcol(Point px, bool clip_to_text) const;
        Point rowcol_to_tpx(Point rc) const;
        size_t rowcol_to_textoffs(Point rc) const;
        Point texoffs_to_rowcol(size_t offs) const;
        Point clip_row_col(Point rc) const;

        int line_height() const;
        int text_width(const std::string& str) const;

        Size text_dims() const;

        void position_scrollbars();

        void x_scrolled(int new_x_pos);
        void y_scrolled(int new_y_pos);

        std::string mtext;

        ScrollBar mhorz_scrollbar, mvert_scrollbar;
        Padding mpadding, mscrollbar_padding, mtext_margins;
        Point mscroll;

        Point mcaret_rowcol;
        Point mcaret_tpx;

        Point manchor_rowcol, manchor_tpx;

        std::vector<std::string> mtext_lines;
        std::vector<int> mtext_lines_chrlen; // length of lines in chrs
        // width of chrs the lines were split at (not part of line)
        std::vector<uint8_t> mtext_line_split_cwidth;
        std::vector<Rect> mselection_tpx;

        int mmax_line_width;
        int mcursor_width;
        WrapMode mwrap_mode;

        CursorBlinkHelper mcursor_blink_helper;

        bool mread_only;
};

}

#endif // LGUI_TEXTBOX_H
