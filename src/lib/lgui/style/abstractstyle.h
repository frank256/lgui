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

#ifndef LGUI_ABSTRACTSTYLE_H
#define LGUI_ABSTRACTSTYLE_H

#include "styleargs.h"
#include "lgui/platform/color.h"

namespace lgui {

class Font;
class Graphics;

/** Basic abstract style class. Its subclass Style is passed to widgets to draw themselves. */
class AbstractStyle {
    public:
        enum HelperButtonType {
            TabBarLeft, TabBarRight, DropDownButton, IncreaseButton, DecreaseButton
        };

        /** Return the default font for the style. */
        virtual const Font& default_font() const = 0;

        /** Return the spacing used between lines of multiline text. */
        virtual int multiline_line_spacing() const = 0;

        /** Return the color used for text in labels. */
        virtual Color label_text_color(bool disabled, float opacity) const = 0;

        /** Return the color used for text in buttons. */
        virtual Color button_text_color(const WidgetState& state, float opacity) const = 0;

        /** Return the color used for text in text fields. */
        virtual Color text_field_text_color(const WidgetState& state, float opacity) const = 0;
        /** Return the color used for selected text in text fields. */
        virtual Color text_field_selection_color(const WidgetState& state, float opacity) const = 0;
        /** Return the color used for drawing the cursor in text fields. */
        virtual Color text_field_cursor_color(const WidgetState& state, float opacity) const = 0;

        struct TextFieldPadding {
            Padding widget, text_margins;
        };
        /** Return the padding for text fields. */
        virtual TextFieldPadding text_field_padding() const = 0;
        /** Return the width of the cursor for text fields. */
        virtual int text_field_cursor_width() const = 0;
        /** Draw the background of a text field (i.e. behind the text). */
        virtual void draw_text_field_bg(Graphics& gfx, const StyleArgs& args) const = 0;
        /** Draw the foreground of a text field (i.e. in front of the text). */
        virtual void draw_text_field_fg(Graphics& gfx, const StyleArgs& args) const = 0;

        struct TextBoxPadding {
            Padding widget, scroll_bars, text_margins;
        };
        /** Return the padding for a text box. */
        virtual TextBoxPadding text_box_padding() const = 0;
        /** Draw the background of a text box (i.e. behind the text). */
        virtual void draw_text_box_bg(Graphics& gfx, const StyleArgs& args) const = 0;
        /** Draw the foreground of a text box (i.e. in front of the text). */
        virtual void draw_text_box_fg(Graphics& gfx, const StyleArgs& args) const = 0;

        /** Return the minimum size a checkbox should occupy using the given font and label. */
        virtual Size get_check_box_min_size(const Font& font, const std::string& text) const = 0;
        /** Draw a check box. */
        virtual void draw_check_box(Graphics& gfx, const StyleArgs& args,
                                    const std::string& text) const = 0;

        /** Return the minimum size a radio button should occupy using the given font and label. */
        virtual Size get_radio_button_min_size(const Font& font, const std::string& text) const = 0;
        /** Draw a radio button. */
        virtual void draw_radio_button(Graphics& gfx, const StyleArgs& args,
                                       const std::string& text) const = 0;

        /** Return the minimum size a push button should occupy using the given font and label. */
        virtual Size get_push_button_min_size(const Font& font, const std::string& text) const = 0;
        /** Draw a push button. */
        virtual void draw_push_button(Graphics& gfx, const StyleArgs& args,
                                      const std::string& text) const = 0;

        /** Return the height of an item in a string list view widget using the specified font. */
        virtual int get_string_list_item_height(const Font& font) const = 0;
        /** Return the width of an item in a string list view widget. */
        virtual int get_string_list_item_width(const Font& font, const std::string& str) const = 0;
        /** Draw an item of a string list widget. */
        virtual void draw_string_list_item(Graphics& gfx, const StyleArgs& args, int indent,
                                           const std::string& str) const = 0;

        /** Return the padding for a list widget. */
        virtual Padding get_list_padding() const = 0;
        /** Return the padding for a list box widget. */
        virtual Padding get_list_box_padding() const = 0;

        /** Draw the background of a list box widget (i.e. behind the actual list widget). */
        virtual void draw_list_box_bg(Graphics& gfx, const StyleArgs& args) const = 0;
        /** Draw the foreground of a list box widget (i.e. in front of the actual list widget). */
        virtual void draw_list_box_fg(Graphics& gfx, const StyleArgs& args) const = 0;

        /** Return the padding for a drop down widget. */
        virtual Padding get_drop_down_padding() const = 0;
        /** Return the minimum size of a drop down widget when folded up. */
        virtual Size get_drop_down_folded_min_size(const Font& dd_font) const = 0;

        /** Draw a drop down widget (without the dropped-down list box). */
        virtual void draw_drop_down(Graphics& gfx, const StyleArgs& args,
                                    const Padding& padding, const std::string& text,
                                    bool is_dropped_down, bool above) const = 0;

        /** Return the width of a scroll bar. */
        virtual int get_scroll_bar_width() const = 0;
        /** Draw the body of a scroll bar. */
        virtual void draw_scroll_bar_body(Graphics& gfx, const StyleArgs& args, bool horizontal) const = 0;
        /** Draw the handle of a scroll bar. */
        virtual void draw_scroll_bar_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const = 0;

        /** Return the padding of a container widget. */
        virtual Padding get_container_padding() const = 0;
        /** Draw the background of a container widget. */
        virtual void draw_container_bg(Graphics& gfx, const StyleArgs& args) const = 0;

        /** Return the padding of a frame widget. */
        virtual Padding get_frame_padding() const = 0;
        /** Return the padding around a frame widget's title bar text. */
        virtual Padding get_frame_title_padding() const = 0;
        /** Draw the title of frame widget. */
        virtual void draw_frame_title(Graphics& gfx, const StyleArgs& args, const Padding& title_padding,
                                      const std::string& title) const = 0;

        /** Draw the background of frame widget. */
        virtual void draw_frame_bg(Graphics& gfx, const StyleArgs& args, const Rect& title_rect,
                                   bool dragged) const = 0;

        /** Return the padding of a tab widget. */
        virtual Padding get_tab_padding() const = 0;
        /** Draw a tab widget. */
        virtual void draw_tab(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                              const std::string& caption) const = 0;

        /** Draw the background of a tab bar widget. */
        virtual void draw_tab_bar_bg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                     const Rect* sel_tab, bool scroll_buttons_shown, bool tab_through_left) const = 0;
        /** Draw the foreground of a tab bar widget. */
        virtual void draw_tab_bar_fg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                     const Rect* sel_tab, bool scroll_buttons_shown, bool tab_through_left) const = 0;
        /** Draw the background of a tab widget. */
        virtual void draw_tab_contents_bg(Graphics& gfx, const StyleArgs& args) const = 0;

        /** Return the minimum size of a helper button. */
        virtual Size get_helper_button_min_size(HelperButtonType type) const = 0;
        /** Draw a helper button. */
        virtual void draw_helper_button(Graphics& gfx, HelperButtonType type, const Rect& rect,
                                        const WidgetState& state, float opacity) const = 0;

        struct SliderMetrics {
            int handle_primary_dim, handle_secondary_dim, body_width;
        };
        /** Return the metrics for a slider. */
        virtual SliderMetrics get_slider_metrics() const = 0;

        /** Draw the body of a slider widget. */
        virtual void draw_slider_body(Graphics& gfx, const StyleArgs& args, const Rect& handle_rect,
                                      bool horizontal) const = 0;
        /** Draw the handle of a slider widget. */
        virtual void draw_slider_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const = 0;

};

}

#endif // LGUI_ABSTRACTSTYLE_H
