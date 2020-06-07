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

#include "textlabel.h"
#include "lgui/style/style.h"
#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"

namespace lgui {

    TextLabel::TextLabel(const std::string& str)
        : mcol(rgb(0.0, 0.0, 0.0)), mtext(str), malign(Align::Left), mcustom_color(false)
    {
        set_active(false);
    }

    TextLabel::TextLabel(const std::string& str, const Color& col,
                         const Font* font)
        : mcol(col), malign(Align::Left), mcustom_color(true)
    {
        if(font)
            set_font(font);

        if(!str.empty())
            set_text(str);
        set_active(false);
    }

    void TextLabel::set_text(const std::string& str)
    {
        mtext = str;
        request_layout();
    }

    Size TextLabel::min_size_hint()
    {
        return Size(font().text_width(mtext)+2*MARGIN_X,
                font().line_height()+2*MARGIN_Y);
    }

    MeasureResults TextLabel::measure(SizeConstraint wc, SizeConstraint hc)
    {
        return force_size_constraints(min_size_hint(), wc, hc);
    }

    void TextLabel::draw(const DrawEvent& de) const
    {
        Color col;
        if(!mcustom_color)
            col = style().label_text_color(de.draw_disabled(), de.opacity());
        else
            col = col_mult_alpha(mcol, de.opacity());

        if(malign.horz() == Align::HCenter)
            de.gfx().draw_textc(font(), MARGIN_X+width()/2, MARGIN_Y, col, mtext);
        else if(malign.horz() == Align::Right)
            de.gfx().draw_textr(font(), width()-MARGIN_X, MARGIN_Y, col, mtext);
        else
            de.gfx().draw_text(font(), MARGIN_X, MARGIN_Y, col, mtext);
    }

}
