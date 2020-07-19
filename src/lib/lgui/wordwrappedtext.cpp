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

#include "wordwrappedtext.h"
#include "lgui/platform/graphics.h"

namespace lgui {

WordWrappedText::WordWrappedText(const Font& font, const std::string& text, int line_spacing)
        : mfont(&font), mallotted_width(0), mmax_width(-1), mtext(text), mline_spacing(line_spacing),
          mshrink_to_used_width(true) {}

void WordWrappedText::draw(Graphics& gfx, const Color& color, lgui::Position offset) const {
    int lh = mfont->line_height() + mline_spacing;
    for (unsigned int i = 0; i < mlines.size(); i++) {
        gfx.draw_text(*mfont, offset.x(), offset.y() + i * lh, color, mlines[i]);
    }
}

void WordWrappedText::set_font(const Font& font) {
    if (mfont != &font) {
        mfont = &font;
        do_wordwrap();
    }
}

void WordWrappedText::set_allotted_width(int width) {
    if (mmax_width >= 0 && width > mmax_width)
        width = mmax_width;
    if (width != mallotted_width) {
        mallotted_width = width;
        do_wordwrap();
    }
}

void WordWrappedText::set_max_width(int max_width, bool resize) {
    mmax_width = max_width;
    if (resize && msize.w() > mmax_width)
        set_allotted_width(mmax_width);
}

void WordWrappedText::set_text(const std::string& text) {
    mtext = text;
    do_wordwrap();
}

void WordWrappedText::set_line_spacing(int line_spacing) {
    mline_spacing = line_spacing;
    msize.set_h(calc_height());
}

void WordWrappedText::set_shrink_to_used_width(bool shrink) {
    mshrink_to_used_width = shrink;
    do_wordwrap();
}

Size WordWrappedText::size_hint(SizeConstraint wc) {
    // FIXME: do something more reasonable here with the interface to word-wrap,
    //        maybe introduce a function for only checking the size

    int w = wc.value();
    if (wc.mode() == SizeConstraintMode::NoLimits)
        w = 65536; // FIXME: use something more reasonable / configurable
    if (w <= 0)
        return Size(0, 0);
    std::vector<std::string> temp_lines;
    temp_lines.reserve(mlines.size());

    int longest_line_w = mfont->do_wordwrap(mtext, w, temp_lines);

    return Size(longest_line_w,
                std::max(int(temp_lines.size()) * (mfont->line_height() + mline_spacing) - mline_spacing, 0));
}

void WordWrappedText::do_wordwrap() {
    mlines.clear();
    if (mallotted_width <= 0 || mtext.empty()) {
        msize.set(0, 0);
        return;
    }

    int longest_line_w = mfont->do_wordwrap(mtext, mallotted_width, mlines);
    if (mshrink_to_used_width)
        msize.set_w(longest_line_w);
    else
        msize.set_w(mallotted_width);
    msize.set_h(calc_height());
}

int WordWrappedText::calc_height() {
    int height = 0;
    if (!mlines.empty()) {
        height = mlines.size() * mfont->line_height();
        height += (mlines.size() - 1) * mline_spacing;
    }
    return height;
}

}
