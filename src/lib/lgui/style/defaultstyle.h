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

#ifndef LGUI_DEFAULTSTYLE_H
#define LGUI_DEFAULTSTYLE_H

#include "style.h"
#include "defaultstylecolorscheme.h"

namespace lgui {

    class DefaultStyleColorScheme;

    /** A default style class. You need to set a Font and a DefaultStyleColorScheme on it in order to
     *  be able to use it. */
    class DefaultStyle : public Style
    {
        public:
            /** C'tor.
             * @param def_font The font to use as default font for the style. Its line height serves as a base
             *                 size for the style's widget, i.e. all sizes will be geared towards matching
             *                 its line height.
             * @param color_scheme The color scheme to use.
             * @param def_scaling_lh Can be used to override the line height of the default font regarding its
             *                       function as a base size for the sizes of the widgets.
             */
            DefaultStyle(const Font& def_font,
                         const DefaultStyleColorScheme& color_scheme, int def_scaling_lh = -1);

            const Font& default_font() const override;

            Color label_text_color(bool inactive, float opacity) const override;
            int multiline_line_spacing() const override { return MULTILINE_SPACING; }

            Color button_text_color(const WidgetState& state, float opacity) const override;
            Color text_field_text_color(const WidgetState& state,
                                                float opacity) const override;
            Color text_field_selection_color(const WidgetState& state,
                                                     float opacity) const override;
            Color text_field_cursor_color(const WidgetState& state,
                                                  float opacity) const override;

            TextFieldPadding text_field_padding() const override;
            int text_field_cursor_width() const override;
            void draw_text_field_bg(Graphics& gfx, const StyleArgs& args) const override;
            void draw_text_field_fg(Graphics& gfx, const StyleArgs& args) const override;

            TextBoxPadding text_box_padding() const override;
            void draw_text_box_bg(Graphics& gfx, const StyleArgs& args) const override;
            void draw_text_box_fg(Graphics& gfx, const StyleArgs& args) const override;

            Size get_check_box_min_size(const Font& font, const std::string& text) const override;
            void draw_check_box(Graphics& gfx, const StyleArgs& args,
                                        const std::string& text) const override;

            Size get_radio_button_min_size(const Font& font, const std::string& text) const override;
            void draw_radio_button(Graphics& gfx, const StyleArgs& args,
                                                  const std::string& text) const override;

            Size get_push_button_min_size(const Font& font, const std::string& text) const override;
            void draw_push_button(Graphics& gfx, const StyleArgs& args,
                                          const std::string& text) const override;

            int get_string_list_item_height(const Font& font) const override;
            int get_string_list_item_width(const Font& font, const std::string& str) const override;
            void draw_string_list_item(Graphics& gfx, const StyleArgs& args, int indent,
                                        const std::string& str) const override;
            Padding get_list_padding() const override;

            Padding get_list_box_padding() const override;

            void draw_list_box_bg(Graphics& gfx, const StyleArgs& args) const override;
            void draw_list_box_fg(Graphics& gfx, const StyleArgs& args) const override;

            Padding get_drop_down_padding() const override;
            Size get_drop_down_folded_min_size(const Font& dd_font) const override;
            void draw_drop_down(Graphics& gfx, const StyleArgs& args,
                                        const Padding& padding, const std::string& text,
                                        bool is_dropped_down, bool above) const override;

            int get_scroll_bar_width() const override;
            void draw_scroll_bar_body(Graphics& gfx, const StyleArgs& args, bool horizontal) const override;
            void draw_scroll_bar_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const override;

            Padding get_container_padding() const override;
            void draw_container_bg(Graphics& gfx, const StyleArgs& args) const override;

            Padding get_frame_title_padding() const override;
            Padding get_frame_padding() const override;
            void draw_frame_title(Graphics& gfx, const StyleArgs& args, const Padding& title_padding,
                                          const std::string& title) const override;
            void draw_frame_bg(Graphics& gfx, const StyleArgs& args, const Rect& title_rect,
                                       bool dragged) const override;

            Padding get_tab_padding() const override;
            void draw_tab(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                  const std::string& caption) const override;
            void draw_tab_bar_bg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                         const Rect* sel_tab, bool scroll_buttons_shown,
                                         bool tab_through_left) const override;
            void draw_tab_bar_fg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                         const Rect* sel_tab, bool scroll_buttons_shown,
                                         bool tab_through_left) const override;
            void draw_tab_contents_bg(Graphics& gfx, const StyleArgs& args) const override;

            Size get_helper_button_min_size(HelperButtonType type) const override;
            void draw_helper_button(Graphics& gfx, HelperButtonType type, const Rect& rect,
                                            const WidgetState& state, float opacity) const override;

            SliderMetrics get_slider_metrics() const override;
            void draw_slider_body(Graphics& gfx, const StyleArgs& args, const Rect& handle_rect,
                                          bool horizontal) const override;
            void draw_slider_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const override;

        protected:
            Color col(DefaultStyleColorScheme::DefaultStyleElement e, const WidgetState& state, float opacity) const;
            Color col(DefaultStyleColorScheme::DefaultStyleElement e, int state_flags, float opacity) const;

            virtual void draw_button_bg(Graphics& gfx, const Rect& rect, const WidgetState& state,
                                        float opacity, bool vertical=false) const;

            const int LH;
            const int CHECKBOX_WH, RADIO_WH, CHECKBOX_SPACING, RADIO_SPACING, SCROLL_BAR_WIDTH,
                      CORNER_ROUNDNESS, DEFAULT_PADDING, PUSH_BUTTON_PADDING_X, PUSH_BUTTON_PADDING_Y,
                      SELECTED_TAB_UPLIFT, HELPER_BUTTON_HZ, HELPER_BUTTON_VT, MULTILINE_SPACING;

            const Padding TAB_PADDING;
            const SliderMetrics SLIDER_METRICS;

        private:
            const Font& mdef_font;
            const DefaultStyleColorScheme& mcscheme;


            lgui::Size get_drop_down_button_size(const Font& dd_font) const;
            void draw_helper_button_icon(Graphics& gfx, Style::HelperButtonType type, const Rect& rect,
                                         const WidgetState& state, float opacity) const;

    };

}

#endif // LGUI_DEFAULTSTYLE_H
