/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#ifndef LGUI_DEFAULTSTYLE2NDBORDER_H
#define LGUI_DEFAULTSTYLE2NDBORDER_H

#include "defaultstyle.h"

namespace lgui {

    /** A variant of the default style drawing two borders around some widgets.
     *  Suitable for the dark color scheme DefaultStyleDarkColorScheme.
     */
    class DefaultStyle2ndBorder : public DefaultStyle
    {
        public:
            DefaultStyle2ndBorder(const Font& def_font, const DefaultStyleColorScheme& scheme,
                                  int def_scaling_lh = -1);

            TextFieldPadding text_field_padding() const override;
            void draw_text_field_fg(Graphics& gfx, const StyleArgs& args) const override;
            void draw_scroll_bar_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const override;

            Size get_push_button_min_size(const Font& font, const std::string& text) const override;

            void draw_push_button(Graphics& gfx, const StyleArgs& args, const std::string& text) const override;

            Padding get_tab_padding() const override;
            void draw_tab(Graphics& gfx, const StyleArgs& args, const Padding& padding, const std::string& caption) const override;
            void draw_tab_bar_fg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                               const Rect* sel_tab, bool scroll_buttons_shown,
                                               bool tab_through_left) const override;
            void draw_tab_contents_bg(Graphics& gfx, const StyleArgs& args) const override;

            Padding get_frame_title_padding() const override;
            Padding get_frame_padding() const override;
            void draw_frame_title(Graphics& gfx, const StyleArgs& args, const Padding& title_padding,
                                                const std::string& title) const override;
            void draw_frame_bg(Graphics& gfx, const StyleArgs& args, const Rect& title_rect, bool dragged) const override;
            void draw_button_bg(Graphics& gfx, const Rect& rect, const WidgetState& state,
                                              float opacity, bool vertical=false) const override;
    };


}

#endif // LGUI_DEFAULTSTYLE2NDBORDER_H
