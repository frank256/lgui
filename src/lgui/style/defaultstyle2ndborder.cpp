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

#include "defaultstyle2ndborder.h"
#include "lgui/platform/graphics.h"

namespace lgui {


    using DSE = DefaultStyleColorScheme::DefaultStyleElement;

    DefaultStyle2ndBorder::DefaultStyle2ndBorder(const Font& def_font, const DefaultStyleColorScheme& scheme,
                                                 int def_scaling_lh)
        : DefaultStyle(def_font, scheme, def_scaling_lh)
    {

    }

    Style::TextFieldPadding DefaultStyle2ndBorder::text_field_padding() const
    {
        return { Padding(DEFAULT_PADDING, DEFAULT_PADDING+1, DEFAULT_PADDING, DEFAULT_PADDING+1),
                 Padding(2*DEFAULT_PADDING, 0, 2*DEFAULT_PADDING, 0) };
    }

    void DefaultStyle2ndBorder::draw_text_field_fg(Graphics& gfx, const StyleArgs& args) const
    {
        DefaultStyle::draw_text_field_fg(gfx, args);
        gfx.rounded_rect(args.rect.shrunk(1), CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                      col(DSE::Border2, args.state, args.opacity), 1);
    }

    void DefaultStyle2ndBorder::draw_button_bg(Graphics& gfx, const Rect& rect, const WidgetState& state, float opacity, bool vertical) const
    {
        DefaultStyle::draw_button_bg(gfx, rect, state, opacity, vertical);
        gfx.rounded_rect(rect.shrunk(1), CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                         col(DSE::ButtonBorder2, state, opacity), 1);
    }

    Padding DefaultStyle2ndBorder::get_frame_title_padding() const
    {
        return Padding(4*DEFAULT_PADDING, 2*DEFAULT_PADDING, 4*DEFAULT_PADDING, 2*DEFAULT_PADDING);
    }

    Padding DefaultStyle2ndBorder::get_frame_padding() const
    {
        return Padding(4*DEFAULT_PADDING);
    }

    void DefaultStyle2ndBorder::draw_frame_title(Graphics& gfx, const StyleArgs& args, const Padding& title_padding, const std::string& title) const
    {
        Color col1 = col(DSE::FrameTitleBgA, args.state, args.opacity),
              col2 = col(DSE::FrameTitleBgB, args.state, args.opacity);

        const int border_width = 2;

        Rect r = args.rect.shrunk(border_width);
        r.set_height(r.h()+border_width); // subtract from three sides only
        gfx.filled_rounded_rect_bracket_gradient(r, CORNER_ROUNDNESS, CORNER_ROUNDNESS, col1, col2,
                                                 OpenEdge::OpenBottom, GradientDirection::TopToBottom);

        gfx.draw_text(args.font, args.rect.x1()+title_padding.left(), args.rect.y()+title_padding.top(),
                       col(DSE::FrameTitleText, args.state, args.opacity), title);
    }

    void DefaultStyle2ndBorder::draw_frame_bg(Graphics& gfx, const StyleArgs& args, const Rect& title_rect, bool dragged) const
    {
        (void) dragged;
        gfx.rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                         col(DSE::FrameBorder1, args.state, args.opacity), 1);

        int border_width = DEFAULT_PADDING;

        lgui::Rect r = args.rect;
        r.set_height(r.h()-title_rect.h()-border_width);
        r.set_pos_y(title_rect.h());
        r.set_width(r.w()-2*border_width);
        r.set_pos_x(r.x()+border_width);
        gfx.filled_rounded_rect_bracket(r, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                                        col(DSE::FrameBg, args.state, args.opacity), OpenEdge::OpenTop);

        gfx.rounded_rect(args.rect.shrunk(1), CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                         col(DSE::FrameBorder2, args.state, args.opacity), 1);
    }

    void DefaultStyle2ndBorder::draw_scroll_bar_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const
    {
        Color c1 = col(DSE::ScrollBarHandleBgA, args.state, args.opacity);
        Color c2 = col(DSE::ScrollBarHandleBgB, args.state, args.opacity);

        gfx.filled_rect_gradient(args.rect.shrunk(1),
                                 c1, c2, horizontal ? GradientDirection::TopToBottom : GradientDirection::LeftToRight);

        gfx.rect(args.rect.shrunk(1), col(DSE::ScrollBarHandleBorder, args.state, args.opacity), 1);
    }

    Size lgui::DefaultStyle2ndBorder::get_push_button_min_size(const Font& font, const std::string& text) const
    {
        return Size(2*PUSH_BUTTON_PADDING_X+font.text_width(text),
                    2*(PUSH_BUTTON_PADDING_Y+1)+font.line_height());
    }


    void DefaultStyle2ndBorder::draw_push_button(Graphics& gfx, const StyleArgs& args, const std::string& text) const
    {
        draw_button_bg(gfx, args.rect, args.state, args.opacity);
        gfx.draw_text(args.font, PUSH_BUTTON_PADDING_X, PUSH_BUTTON_PADDING_Y+1,
                      col(DSE::ButtonText, args.state, args.opacity), text);
    }



    Padding DefaultStyle2ndBorder::get_tab_padding() const
    {
        Padding p(TAB_PADDING);
        p.set_bottom(p.bottom()+1);
        return p;
    }

    void DefaultStyle2ndBorder::draw_tab(Graphics& gfx, const StyleArgs& args, const Padding& padding, const std::string& caption) const
    {
        Color bg_col = col(args.state.is_selected() ? DSE::TabSelBg : DSE::TabNotSelBg,
                           args.state, args.opacity);

        Color tcol = col(DSE::TabBorder1, args.state, args.opacity);
        Color tcol2 = col(DSE::TabBorder2, args.state, args.opacity);

        int ytop = args.rect.y1();

        if(!args.state.is_selected())
            ytop+=SELECTED_TAB_UPLIFT;

        gfx.filled_rounded_rect_bracket(args.rect.x1()+0.5, ytop+0.5, args.rect.x2()+0.5, args.rect.y2()+ytop+1,
                                        CORNER_ROUNDNESS, CORNER_ROUNDNESS, bg_col, OpenEdge::OpenBottom);

        if(args.state.is_focused() && args.state.is_selected()) {
            Rect r = padding.sub(args.rect);
            // FIXME: make customizable
            r = Padding(2, 0, 2, 0).add(r);
            gfx.rect(r, col(DSE::TabSelFocusedBorder, args.state, args.opacity), 1);
        }

        gfx.draw_text(args.font, padding.left(), padding.top(), col(DSE::DispText, args.state, args.opacity), caption);
        gfx.rounded_rect_bracket(args.rect.x1()+0.5, ytop+0.5, args.rect.x2()+0.5, args.rect.y2()+1,
                                 CORNER_ROUNDNESS, CORNER_ROUNDNESS, tcol, 0, OpenEdge::OpenBottom);
        gfx.rounded_rect_bracket(args.rect.x1()+0.5+1, ytop+0.5+1, args.rect.x2()+0.5-1, args.rect.y2()+1,
                                 CORNER_ROUNDNESS, CORNER_ROUNDNESS, tcol2, 0, OpenEdge::OpenBottom);
   }

    void DefaultStyle2ndBorder::draw_tab_bar_fg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                       const Rect* sel_tab, bool scroll_buttons_shown, bool tab_through_left) const
    {
        (void) scroll_buttons_shown;
        Color tcol1 = col(DSE::TabBorder1, args.state, args.opacity);
        Color tcol2 = col(DSE::TabBorder2, args.state, args.opacity);

        float line_y = args.rect.y2()+0.5-padding.bottom();

        if(sel_tab) {
            int x1 = args.rect.x1(),
                x2 = std::min(sel_tab->x(), args.rect.x2());
            if(sel_tab->x1() > args.rect.x1()) {
                gfx.line(x1+1, line_y,
                         x2+1, line_y, tcol2, 1);
                gfx.line(x1+2, line_y-1,
                         x2, line_y-1, tcol1, 1);
            }
            if(sel_tab->x2() < args.rect.x2()) {
                x1 = std::max(sel_tab->x2(), x1+1);
                x2 = args.rect.x2();
                gfx.line(x1, line_y,
                         x2, line_y, tcol2, 1);
                gfx.line(x1+1, line_y-1,
                         x2-1, line_y-1, tcol1, 1);
            }
        }
        else {
            gfx.line(args.rect.x1()+1, line_y,
                 args.rect.x2(), line_y, tcol2, 1);
            gfx.line(args.rect.x1()+2, line_y-1,
                     args.rect.x2()-1, line_y-1, tcol1, 1);
        }
        if(tab_through_left) {
            int plus = SELECTED_TAB_UPLIFT;
            if(sel_tab && args.rect.x1() >= sel_tab->x1() && args.rect.x1() <= sel_tab->x2())
                plus = 0;
            gfx.line(args.rect.x1()+0.5+1, args.rect.y1()+padding.top()+plus+1,
                     args.rect.x1()+0.5+1, args.rect.y2()+1-padding.bottom(), tcol2, 1);
            gfx.line(args.rect.x1()+0.5, args.rect.y1()+padding.top()+plus,
                     args.rect.x1()+0.5, args.rect.y2()+1-padding.bottom(), tcol1, 1);
        }
    }

    void DefaultStyle2ndBorder::draw_tab_contents_bg(Graphics& gfx, const StyleArgs& args) const
    {
        gfx.filled_rect(args.rect, col(DSE::TabContentBg, args.state, args.opacity));

        Color tcol1 = col(DSE::TabBorder1, args.state, args.opacity);
        Color tcol2 = col(DSE::TabBorder2, args.state, args.opacity);

        gfx.line(args.rect.x1()+0.5, args.rect.y1(), args.rect.x1()+0.5, args.rect.y2()+1, tcol1, 1);
        gfx.line(args.rect.x2()+0.5, args.rect.y1(), args.rect.x2()+0.5, args.rect.y2()+1, tcol1, 1);
        gfx.line(args.rect.x1(), args.rect.y2()+0.5, args.rect.x2(), args.rect.y2()+0.5, tcol1, 1);

        gfx.line(args.rect.x1()+0.5+1, args.rect.y1(), args.rect.x1()+0.5+1, args.rect.y2(), tcol2, 1);
        gfx.line(args.rect.x2()+0.5-1, args.rect.y1(), args.rect.x2()+0.5-1, args.rect.y2(), tcol2, 1);
        gfx.line(args.rect.x1()+1, args.rect.y2()+0.5-1, args.rect.x2()-1, args.rect.y2()+0.5-1, tcol2, 1);
    }

}
