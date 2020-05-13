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

#include "defaultstyle.h"

#include "lgui/platform/graphics.h"
#include "lgui/platform/font.h"
#include <cmath> // for figuring out the thicker CheckBox coords...

namespace lgui {

// helper macro for debugging
#define SHOW_RECT gfx.rect(args.rect, rgb_col_f(0.0, 1.0, 0.0), 1)

static const float LH_DIV = 23.0;
#define LH_PERC(x) int(std::round(LH*float(x) / LH_DIV))

using DSE = DefaultStyleColorScheme::DefaultStyleElement;


DefaultStyle::DefaultStyle(const Font& def_font, const DefaultStyleColorScheme& scheme, int def_lh)
 : LH(def_lh > 0 ? def_lh : def_font.line_height()),
   CHECKBOX_WH(LH_PERC(16)),
   RADIO_WH(LH_PERC(15)),
   CHECKBOX_SPACING(LH_PERC(6)),
   RADIO_SPACING(LH_PERC(6)),
   SCROLL_BAR_WIDTH(LH_PERC(10)),
   CORNER_ROUNDNESS(LH_PERC(3)),
   DEFAULT_PADDING(LH_PERC(2)),
   PUSH_BUTTON_PADDING_X(LH_PERC(7)),
   PUSH_BUTTON_PADDING_Y(LH_PERC(3)),
   SELECTED_TAB_UPLIFT(LH_PERC(2)),
   HELPER_BUTTON_HZ(LH_PERC(2)),
   HELPER_BUTTON_VT(LH_PERC(5)),
   MULTILINE_SPACING(LH_PERC(4)),
   TAB_PADDING(LH_PERC(9), LH_PERC(6), LH_PERC(9), LH_PERC(1)),
   SLIDER_METRICS{LH_PERC(20), LH_PERC(20), LH_PERC(6)},

   mdef_font(def_font), mcscheme(scheme)
{
}

const Font& DefaultStyle::default_font() const
{
    return mdef_font;
}

Color DefaultStyle::label_text_color(bool inactive, float opacity) const
{
    (void) inactive;
    return col(DSE::DispText, 0, opacity);
}

Color DefaultStyle::button_text_color(const WidgetState& state, float opacity) const
{
    return col(DSE::ButtonText, state, opacity);
}

Color DefaultStyle::text_field_text_color(const WidgetState& state, float opacity) const {
    return col(DSE::EditText, state, opacity);
}

Color DefaultStyle::text_field_selection_color(const WidgetState& state, float opacity) const {
    return col(DSE::EditTextSelection, state, opacity);
}

Color DefaultStyle::text_field_cursor_color(const WidgetState& state, float opacity) const {
    return col(DSE::EditTextCursor, state, opacity);
}

Style::TextFieldPadding DefaultStyle::text_field_padding() const
{
    return { Padding(DEFAULT_PADDING/2, DEFAULT_PADDING,
                     DEFAULT_PADDING/2, DEFAULT_PADDING), Padding(2*DEFAULT_PADDING, 0,
                                                                  2*DEFAULT_PADDING, 0) };
}

int DefaultStyle::text_field_cursor_width() const
{
    return DEFAULT_PADDING;
}

void DefaultStyle::draw_text_field_bg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.filled_rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                            col(DSE::WidgetFillBg, args.state, args.opacity));
}

void DefaultStyle::draw_text_field_fg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                     col(DSE::Border1, args.state, args.opacity), 1);
}

Style::TextBoxPadding DefaultStyle::text_box_padding() const
{
    return { Padding(DEFAULT_PADDING), Padding(0), Padding(2*DEFAULT_PADDING) };
}

void DefaultStyle::draw_text_box_bg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.filled_rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                            col(DSE::WidgetFillBg, args.state, args.opacity));
}

void DefaultStyle::draw_text_box_fg(Graphics& gfx, const StyleArgs& args) const
{
    // do not draw text-box differently with focus
    Color color =  col(DSE::Border1, args.state.state &~WidgetState::Focused, args.opacity);
    gfx.rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS, color, 1);
}


Size DefaultStyle::get_check_box_min_size(const Font& font, const std::string& text) const
{
    // for Android, as the linker can't accept std::max with a static const member var
    // TODO: introduce proper right-padding (here: CHECKBOX_SPACING/2)
    const int CBXWH = CHECKBOX_WH;
    return Size(CBXWH+ CHECKBOX_SPACING + CHECKBOX_SPACING / 2+ font.text_width(text),
                std::max(font.line_height(), CBXWH));
}

static float prim_coord_x(const lgui::Rect& r, float px) {
    return r.x() + px *r.w() + 0.5;
}

static float prim_coord_y(const lgui::Rect& r, float py) {
    return r.y() + py *r.h() + 0.5;
}

void DefaultStyle::draw_check_box(Graphics& gfx, const StyleArgs& args,
                                  const std::string& text) const
{
    const lgui::Rect r(args.rect.x(), args.rect.y()+(args.rect.h()-CHECKBOX_WH)/2,
                      CHECKBOX_WH, CHECKBOX_WH);

    gfx.filled_rect(r, col(DSE::CheckBoxBg, args.state, args.opacity));
    gfx.rect(r, col(DSE::CheckBoxBorder, args.state, args.opacity), 1);

    gfx.line(r.x1()+1, r.y1()+1, r.x2(), r.y1()+1, col(DSE::Border1, 0, args.opacity), 1);

    if(args.state.is_checked()) {
        Rect inner = r;
        Color inner_col = col(DSE::Check, args.state, args.opacity);

        const float v1x = 0.75, v1y = 0.2,
                    v2x = 0.45, v2y = 0.7,
                    v3x = 0.2, v3y = 0.5;

        float jx = prim_coord_x(inner, v2x),
              jy = prim_coord_y(inner, v2y);

        // FIXME: line width, precalc?

        float dx = jx-prim_coord_x(inner, v1x);
        float dy = jy-prim_coord_y(inner, v1y);
        float d = sqrtf(dx*dx+dy*dy);
        dx/= d;
        dy/= d;

        gfx.line(prim_coord_x(inner, v1x),
                 prim_coord_y(inner, v1y),
                 jx+dx,
                 jy+dy, inner_col, 2);

        gfx.line(jx,
                 jy,
                 prim_coord_x(inner, v3x),
                 prim_coord_y(inner, v3y), inner_col, 2.5);
    }
    if(args.state.is_focused()) {
        Rect r(CHECKBOX_WH + CHECKBOX_SPACING/2, (args.rect.h() - args.font.line_height()) / 2,
               args.rect.w()-CHECKBOX_WH - CHECKBOX_SPACING/2, args.font.line_height());
        Color gradient_col1 = col(DSE::CheckRadioTextEmphasisBgA, args.state, args.opacity);
        Color gradient_col2 = col(DSE::CheckRadioTextEmphasisBgB, args.state, args.opacity);
        gfx.filled_rect_gradient(r, gradient_col1, gradient_col2, GradientDirection::TopToBottom);
        gfx.rect(r, col(DSE::CheckRadioTextEmphasisBorder, args.state, args.opacity), 1);
    }
    gfx.draw_text(args.font, args.rect.x()+CHECKBOX_WH + CHECKBOX_SPACING,
                  (args.rect.h() - args.font.line_height()) / 2,
                  col(DSE::DispText, args.state, args.opacity), text);
}

Size DefaultStyle::get_radio_button_min_size(const Font& font, const std::string& text) const
{
    // for Android, as the linker can't accept std::max with a static const member var
    // TODO: introduce proper right-padding (here: RADIO_SPACING/2)
    const int RWH = RADIO_WH;
    return Size(RWH + RADIO_SPACING + RADIO_SPACING/2 + font.text_width(text),
                std::max(font.line_height(), RWH));
}

void DefaultStyle::draw_radio_button(Graphics& gfx, const StyleArgs& args,
                                            const std::string& text) const
{
    int cy = args.rect.y() + args.rect.h() / 2;

    gfx.filled_circle(RADIO_WH/2, cy, RADIO_WH/2, col(DSE::RadioBg, args.state, args.opacity));
    gfx.circle(RADIO_WH/2, cy, RADIO_WH/2, col(DSE::RadioBorder, args.state, args.opacity), 1);

    if(args.state.is_checked())
        gfx.filled_circle(RADIO_WH/2, cy, RADIO_WH/2 - 3,
                          col(DSE::RadioFill, args.state, args.opacity));
    if(args.state.is_focused()) {
        Rect r(RADIO_WH + RADIO_SPACING/2, cy - args.font.line_height() / 2, args.rect.w()-RADIO_WH - RADIO_SPACING/2, args.font.line_height());
        Color gradient_col1 = col(DSE::CheckRadioTextEmphasisBgA, args.state, args.opacity);
        Color gradient_col2 = col(DSE::CheckRadioTextEmphasisBgB, args.state, args.opacity);
        gfx.filled_rect_gradient(r, gradient_col1, gradient_col2, GradientDirection::TopToBottom);
        gfx.rect(r, col(DSE::CheckRadioTextEmphasisBorder, args.state, args.opacity), 1);
    }
    gfx.draw_text(args.font, RADIO_WH + RADIO_SPACING,
                  cy - args.font.line_height() / 2,
                  col(DSE::DispText, args.state, args.opacity), text);
}

Size DefaultStyle::get_push_button_min_size(const Font& font, const std::string& text) const
{
    return Size(2*PUSH_BUTTON_PADDING_X+font.text_width(text), 2*PUSH_BUTTON_PADDING_Y+font.line_height());
}

void DefaultStyle::draw_push_button(Graphics& gfx, const StyleArgs& args, const std::string& text) const
{
    draw_button_bg(gfx, args.rect, args.state, args.opacity);
    gfx.draw_text(args.font, PUSH_BUTTON_PADDING_X, PUSH_BUTTON_PADDING_Y,
                  col(DSE::ButtonText, args.state, args.opacity), text);
}

int DefaultStyle::get_string_list_item_height(const Font& font) const
{
    return font.line_height() + 2*DEFAULT_PADDING;
}

int DefaultStyle::get_string_list_item_width(const Font& font, const std::string& str) const
{
    return font.text_width(str)+2*DEFAULT_PADDING;
}

void DefaultStyle::draw_string_list_item(Graphics& gfx, const StyleArgs& args,
                                  int indent, const std::string& str) const
{
    // indent is negative -> default
    if(indent < 0)
        indent = DEFAULT_PADDING;

    Color bg_col;
    if(args.state.is_selected())
        bg_col = col(DSE::EditTextSelection, args.state, args.opacity);
    else
        bg_col = col(DSE::WidgetFillBg, args.state, args.opacity);

    gfx.filled_rect(args.rect, bg_col);
    gfx.draw_text(args.font, args.rect.x()+indent, args.rect.y()+DEFAULT_PADDING,
                  col(DSE::EditText, args.state, args.opacity), str);
}

Padding DefaultStyle::get_list_padding() const
{
    return Padding(1);
}

Padding DefaultStyle::get_list_box_padding() const
{
    return Padding(0);
}

void DefaultStyle::draw_list_box_bg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.filled_rect(args.rect, col(DSE::WidgetFillBg, args.state, args.opacity));
}

void DefaultStyle::draw_list_box_fg(Graphics& gfx, const StyleArgs& args) const
{
    // only 0?
    gfx.rect(args.rect, col(DSE::Border1, args.state, args.opacity), 1);
}

Size DefaultStyle::get_drop_down_button_size(const Font& dd_font) const
{
    return lgui::Size(std::max(2*PUSH_BUTTON_PADDING_Y, dd_font.line_height()+2*DEFAULT_PADDING),
                     std::max(2*PUSH_BUTTON_PADDING_Y, dd_font.line_height()+2*DEFAULT_PADDING));
}

Padding DefaultStyle::get_drop_down_padding() const
{
    return Padding(2*DEFAULT_PADDING, DEFAULT_PADDING, DEFAULT_PADDING, DEFAULT_PADDING);
}

Size DefaultStyle::get_drop_down_folded_min_size(const Font& dd_font) const
{
    Padding padd = get_drop_down_padding();
    return padd.add(get_drop_down_button_size(dd_font));
}


void DefaultStyle::draw_drop_down(Graphics& gfx, const StyleArgs& args,
                                  const Padding& padding, const std::string& text,
                                  bool is_dropped_down, bool above) const
{
    Size button_size = get_drop_down_button_size(args.font);
    Rect button_rect(args.rect.w()-button_size.w(), 0,
                     Size(button_size.w(), get_drop_down_folded_min_size(args.font).h()));

    if(!is_dropped_down) {
        draw_button_bg(gfx, args.rect, args.state, args.opacity);
        draw_helper_button_icon(gfx, DropDownButton, button_rect, args.state, args.opacity);
    }
    else {
        WidgetState s = args.state;
        s.state |= WidgetState::Down;
        // Idea: add rounded corners radius to height.
        // Dropped-down listbox will be drawn on top,
        // this will avoid glitches in the corners.
        Rect br(args.rect);
        br.set_height(br.h()+PUSH_BUTTON_PADDING_Y);
        if (above)
            br.set_pos_y(br.y()-PUSH_BUTTON_PADDING_Y);
        draw_button_bg(gfx, br, s, args.opacity);
        const Rect& r = args.rect; // shorter
        gfx.rounded_rect_bracket(r.x()+0.5, r.y()+0.5, r.x2()+0.5, r.y2()+0.5,
                                 CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                                 col(DSE::Border1, args.state, args.opacity), 1,
                                 above ? OpenEdge::OpenTop : OpenEdge::OpenBottom);
    }
    // center text vertically around button
    gfx.draw_text(args.font, padding.left() + get_list_padding().left(),
                  padding.top() + (button_size.h() - args.font.line_height()) / 2,
                  col(DSE::DispText, args.state, args.opacity), text);
}

int DefaultStyle::get_scroll_bar_width() const
{
    return SCROLL_BAR_WIDTH;
}

void DefaultStyle::draw_scroll_bar_body(Graphics& gfx, const StyleArgs& args, bool horizontal) const
{
    (void) horizontal;
    gfx.filled_rect(args.rect, col(DSE::ScrollBarBodyBg, args.state, args.opacity));
    gfx.rect(args.rect, col(DSE::ScrollBarBodyBorder, args.state, args.opacity), 1);
}

void DefaultStyle::draw_scroll_bar_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const
{
    Color c1 = col(DSE::ScrollBarHandleBgA, args.state, args.opacity);
    Color c2 = col(DSE::ScrollBarHandleBgB, args.state, args.opacity);

    gfx.filled_rect_gradient(args.rect.shrunk(1),
                             c1, c2, horizontal ? GradientDirection::TopToBottom : GradientDirection::LeftToRight);

    if(!horizontal) {
        gfx.line(args.rect.x1(), args.rect.y1()+0.5, args.rect.x2(), args.rect.y1()+0.5,
                 col(DSE::ScrollBarHandleBorder, args.state, args.opacity), 1);
        gfx.line(args.rect.x1(), args.rect.y2()+0.5, args.rect.x2(), args.rect.y2()+0.5,
                 col(DSE::ScrollBarHandleBorder, args.state, args.opacity), 1);
    }
    else {
        gfx.line(args.rect.x1()+0.5, args.rect.y1(), args.rect.x1()+0.5, args.rect.y2(),
                 col(DSE::ScrollBarHandleBorder, args.state, args.opacity), 1);
        gfx.line(args.rect.x2()+0.5, args.rect.y1(), args.rect.x2()+0.5, args.rect.y2(),
                 col(DSE::ScrollBarHandleBorder, args.state, args.opacity), 1);
    }
}

Padding DefaultStyle::get_container_padding() const
{
    return Padding(2);
}

void DefaultStyle::draw_container_bg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.filled_rect(args.rect, col(DSE::Background, args.state, args.opacity));
}

Padding DefaultStyle::get_frame_title_padding() const
{
    return Padding(2*DEFAULT_PADDING);
}

Padding DefaultStyle::get_frame_padding() const
{
    return Padding(2*DEFAULT_PADDING);
}

void DefaultStyle::draw_frame_title(Graphics& gfx, const StyleArgs& args, const Padding& title_padding,
                                    const std::string& title) const
{
    Color col1 = col(DSE::FrameTitleBgA, args.state, args.opacity),
          col2 = col(DSE::FrameTitleBgB, args.state, args.opacity);

    const int border_width = 1;

    Rect r = args.rect.shrunk(border_width);
    r.set_height(r.h()+border_width); // subtract from three sides only
    gfx.filled_rounded_rect_bracket_gradient(r, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                                             col1, col2, OpenEdge::OpenBottom,
                                             GradientDirection::TopToBottom);

    gfx.draw_text(args.font, args.rect.x1()+title_padding.left(), args.rect.y()+title_padding.top(),
                   col(DSE::FrameTitleText, args.state, args.opacity), title);
}

void DefaultStyle::draw_frame_bg(Graphics& gfx, const StyleArgs& args, const Rect& title_rect, bool dragged) const
{
    (void) dragged;
    gfx.rounded_rect(args.rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                     col(DSE::FrameBorder1, args.state, args.opacity), 1);

    const int border_width = 1;

    lgui::Rect r = args.rect;
    r.set_height(r.h()-title_rect.h()-border_width);
    r.set_pos_y(title_rect.h());
    r.set_width(r.w()-2*border_width);
    r.set_pos_x(r.x()+border_width);
    gfx.filled_rounded_rect_bracket(r, CORNER_ROUNDNESS, CORNER_ROUNDNESS,
                                    col(DSE::FrameBg, args.state, args.opacity), OpenEdge::OpenTop);
}


Padding DefaultStyle::get_tab_padding() const
{
    return TAB_PADDING;
}

void DefaultStyle::draw_tab(Graphics& gfx, const StyleArgs& args, const Padding& padding, const std::string& caption) const
{
    Color bg_col = col(args.state.is_selected() ? DSE::TabSelBg : DSE::TabNotSelBg,
                       args.state, args.opacity);

    Color tcol = col(DSE::TabBorder1, args.state, args.opacity);

    int ytop = args.rect.y1();

    if(!args.state.is_selected())
        ytop+=SELECTED_TAB_UPLIFT;

    gfx.filled_rounded_rect_bracket(args.rect.x1()+0.5, ytop+0.5, args.rect.x2()+0.5, args.rect.y2()+ytop+1,
                                    CORNER_ROUNDNESS, CORNER_ROUNDNESS, bg_col, OpenEdge::OpenBottom);

    if(args.state.is_focused() && args.state.is_selected()) {
        Rect r = padding.sub(args.rect);
        // FIXME: make customizable
        r = Padding(DEFAULT_PADDING, 0, DEFAULT_PADDING, 0).add(r);
        gfx.rect(r, col(DSE::TabSelFocusedBorder, args.state, args.opacity), 1);
    }

    gfx.draw_text(args.font, padding.left(), padding.top(), col(DSE::DispText, args.state, args.opacity),
                  caption);
    gfx.rounded_rect_bracket(args.rect.x1()+0.5, ytop+0.5, args.rect.x2()+0.5, args.rect.y2()+1,
                             CORNER_ROUNDNESS, CORNER_ROUNDNESS, tcol, 0, OpenEdge::OpenBottom);
}

void DefaultStyle::draw_tab_bar_bg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                   const Rect* sel_tab, bool scroll_buttons_shown, bool tab_through_left) const
{
    (void) tab_through_left;
    (void) scroll_buttons_shown;
    (void) sel_tab;
    (void) padding;
    gfx.filled_rect(args.rect, col(DSE::Background, args.state, args.opacity));
}

void DefaultStyle::draw_tab_bar_fg(Graphics& gfx, const StyleArgs& args, const Padding& padding,
                                   const Rect* sel_tab, bool scroll_buttons_shown, bool tab_through_left) const
{
    (void) scroll_buttons_shown;
    Color tcol1 = col(DSE::TabBorder1, args.state, args.opacity);

    float line_y = args.rect.y2()+0.5-padding.bottom();

    if(sel_tab) {
        int x1 = args.rect.x1(),
            x2 = std::min(sel_tab->x(), args.rect.x2());
        if(sel_tab->x1() > args.rect.x1()) {
            gfx.line(x1+1, line_y,
                     x2, line_y, tcol1, 1);
        }
        if(sel_tab->x2() < args.rect.x2()) {
            x1 = std::max(sel_tab->x2(), x1);
            x2 = args.rect.x2();
            gfx.line(x1+1, line_y,
                     x2, line_y, tcol1, 1);
        }
    }
    else {
        gfx.line(args.rect.x1()+1, line_y,
                 args.rect.x2(), line_y, tcol1, 1);
    }
    if(tab_through_left) {
        int plus = SELECTED_TAB_UPLIFT; // see draw_tab: not selected tabs are 2px down
        if(sel_tab && args.rect.x1() >= sel_tab->x1() && args.rect.x1() <= sel_tab->x2())
            plus = 0;
        gfx.line(args.rect.x1()+0.5, args.rect.y1()+padding.top()+plus,
                 args.rect.x1()+0.5, args.rect.y2()+1-padding.bottom(), tcol1, 1);
    }
}

void DefaultStyle::draw_tab_contents_bg(Graphics& gfx, const StyleArgs& args) const
{
    gfx.filled_rect(args.rect, col(DSE::TabContentBg, args.state, args.opacity));

    Color tcol1 = col(DSE::TabBorder1, args.state, args.opacity);

    gfx.line(args.rect.x1()+0.5, args.rect.y1(), args.rect.x1()+0.5, args.rect.y2()+1, tcol1, 1);
    gfx.line(args.rect.x2()+0.5, args.rect.y1(), args.rect.x2()+0.5, args.rect.y2()+1, tcol1, 1);
    gfx.line(args.rect.x1(), args.rect.y2()+0.5, args.rect.x2(), args.rect.y2()+0.5, tcol1, 1);
}

Size DefaultStyle::get_helper_button_min_size(Style::HelperButtonType type) const
{
    if (type == IncreaseButton || type == DecreaseButton) {
        return Size(2*HELPER_BUTTON_VT + 2*DEFAULT_PADDING,
                    2*HELPER_BUTTON_HZ + 2*DEFAULT_PADDING);
    }
    return Size(CHECKBOX_WH - 2 * DEFAULT_PADDING, CHECKBOX_WH - 2 * DEFAULT_PADDING);
}

void DefaultStyle::draw_helper_button(Graphics& gfx, Style::HelperButtonType type, const Rect& rect,
                                      const WidgetState& state, float opacity) const
{
    draw_button_bg(gfx, rect, state, opacity);
    draw_helper_button_icon(gfx, type, rect, state, opacity);
}


void DefaultStyle::draw_helper_button_icon(Graphics& gfx, Style::HelperButtonType type, const Rect& rect,
                                           const WidgetState& state, float opacity) const
{

    Point center = rect.pos() + Point(rect.w()/2, rect.h()/2);
    const int V = HELPER_BUTTON_VT, H = HELPER_BUTTON_HZ;
    Point h1(center.x() - H, center.y());
    Point h2(center.x() + H, center.y());
    Point v1, v2, v3;
    if (type == TabBarLeft) {
        v1 = Point(h2.x(), h2.y() - V);
        v2 = h1;
        v3 = Point(h2.x(), h2.y() + V);
    }
    else if (type == TabBarRight) {
        v1 = Point(h1.x(), h2.y() - V);
        v2 = Point(h1.x(), h2.y() + V);;
        v3 = h2;
    }
    else if (type == DropDownButton || type == DecreaseButton){ // DropDown
        v1 = Point(center.x()-V, center.y()-H);
        v2 = Point(center.x()+V, center.y()-H);
        v3 = Point(center.x(), center.y()+H);
    }
    else if (type == IncreaseButton) {
        v1 = Point(center.x()+V, center.y()+H);
        v2 = Point(center.x()-V, center.y()+H);
        v3 = Point(center.x(), center.y()-H);
    }
    gfx.filled_triangle(v1, v2, v3, col(DSE::ButtonText, state, opacity));
}

Style::SliderMetrics DefaultStyle::get_slider_metrics() const
{
    return SLIDER_METRICS;
}

void DefaultStyle::draw_slider_body(Graphics& gfx, const StyleArgs& args, const Rect& handle_rect, bool horizontal) const
{
    Color bcol = col(DSE::SliderBodyBorder, args.state, args.opacity);
    Color bgcol = col(DSE::SliderBodyBg, args.state, args.opacity);

    gfx.filled_rounded_rect(args.rect, 1, 1, bgcol);
    gfx.rounded_rect(args.rect, 1, 1, bcol, 1);

    Color fill_col = col(DSE::SliderBodyFillBg, args.state, args.opacity);

    lgui::Rect r = args.rect;
    if(horizontal)
        r.set_width(handle_rect.x()+handle_rect.w()/2-r.x1());
    else {
        r.set_height(r.h()-handle_rect.y()-handle_rect.h()/2);
        r.set_pos_y(handle_rect.y2()+1);
    }

    // no fill when inactive
    if(!args.state.is_inactive() && r.w() > 0 && r.h() > 0) {
        gfx.filled_rounded_rect(r, 1, 1, fill_col);
        gfx.rounded_rect_bracket(r, 1, 1, col(DSE::SliderBodyFillBorder, args.state, args.opacity), 1,
                                 horizontal ? OpenEdge::OpenRight : OpenEdge::OpenTop);
    }

}

void DefaultStyle::draw_slider_handle(Graphics& gfx, const StyleArgs& args, bool horizontal) const
{
    draw_button_bg(gfx, args.rect, args.state, args.opacity, !horizontal);
}

Color DefaultStyle::col(DefaultStyleColorScheme::DefaultStyleElement e, const WidgetState& state, float opacity) const
{
    return mcscheme.color(e, state, opacity);
}

Color DefaultStyle::col(DefaultStyleColorScheme::DefaultStyleElement e, int state_flags, float opacity) const
{
    return mcscheme.color(e, state_flags, opacity);
}

void DefaultStyle::draw_button_bg(Graphics& gfx, const Rect& rect, const WidgetState& state,
                                  float opacity, bool vertical) const
{
    Color tcol = col(DSE::ButtonBgA, state, opacity),
          bcol = col(DSE::ButtonBgB, state, opacity),
          outl_col1 = col(DSE::ButtonBorder1, state, opacity);

    gfx.filled_rounded_rect_gradient(rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS, tcol, bcol,
                                     (!vertical) ? GradientDirection::TopToBottom : GradientDirection::LeftToRight);

    gfx.rounded_rect(rect, CORNER_ROUNDNESS, CORNER_ROUNDNESS, outl_col1, 1);
}


}

