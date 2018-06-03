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

#include "wordwraptextlabel.h"
#include "lgui/style/style.h"
#include "lgui/drawevent.h"

namespace lgui {

    WordWrapTextLabel::WordWrapTextLabel(const std::string& str)
        : mwwtext(Widget::font(), str, style().multiline_line_spacing()),
          mpadding(4), mcustom_color(false)
    {
        set_active(false);
    }

    WordWrapTextLabel::WordWrapTextLabel(const std::string& str, const Color& col, const Font* font)
        : mwwtext(font ? *font : Widget::font(), str, style().multiline_line_spacing()), mcol(col),
          mpadding(4), mcustom_color(true)
    {
        if(font)
            Widget::set_font(font);
        set_active(false);
    }

    void WordWrapTextLabel::set_size(Size s)
    {
        if(s.w() > 0) {
            mwwtext.set_allotted_width(s.w()-mpadding.horz());
            // FIXME: when measuring, don't change the size
            Widget::set_size(mpadding.add(mwwtext.size()));
        }
    }

    void WordWrapTextLabel::set_text(const std::string& text)
    {
        mwwtext.set_text(text);
        request_layout();
    }

    void WordWrapTextLabel::set_font(const Font* font)
    {
        Widget::set_font(font);
        mwwtext.set_font(Widget::font());
        request_layout();
    }

    void WordWrapTextLabel::set_max_width(int w, bool resize)
    {
        mwwtext.set_max_width(w-mpadding.horz(), resize);
        request_layout();
    }

    MeasureResults WordWrapTextLabel::measure(SizeConstraint wc, SizeConstraint hc)
    {
        return force_size_constraints(mpadding.add(mwwtext.size_hint(wc)), wc, hc);
    }


    void WordWrapTextLabel::draw(const DrawEvent& de) const
    {
        Color col;
        if(!mcustom_color)
            col = style().label_text_color(de.draw_inactive(), de.opacity());
        else
            col = col_mult_alpha(mcol, de.opacity());

        mwwtext.draw(de.gfx(), col, mpadding.left_top_offs());
    }


    void WordWrapTextLabel::style_changed()
    {
        mwwtext.set_line_spacing(style().multiline_line_spacing());
    }


}

