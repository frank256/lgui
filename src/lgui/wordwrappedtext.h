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

#ifndef LGUI_WORDWRAPPEDTEXT_H
#define LGUI_WORDWRAPPEDTEXT_H

#include "platform/font.h"
#include "lgui.h"
#include <vector>
#include "platform/color.h"


namespace lgui {
    class Graphics;

    /** A helper class for dealing with word-wrapped text. You can set the text, font and width, line spacing,
     *  retrieve the size and let the text be drawn. Optionally, you may set a max width that will never be
     *  exceeded. A max_width < 0 means it is not set. */
    class WordWrappedText
    {
        public:
            explicit WordWrappedText(const Font& font, const std::string& text = "",
                            int line_spacing=0);

            void draw(Graphics& gfx, const Color& color, Position offset=Position(0, 0)) const;

            void set_text(const std::string& text);
            void set_font(const Font& font);
            void set_allotted_width(int width);
            void set_max_width(int max_width, bool resize);
            void set_line_spacing(int line_spacing);
            void set_shrink_to_used_width(bool shrink);

            Size size_hint(SizeConstraint wc);

            const Size& size() const { return msize; }
            const std::string& text() const { return mtext; }
            int allotted_width() const { return mallotted_width; }
            int max_width() const { return mmax_width; }
            bool is_shrink_to_used_width_enabled() const { return mshrink_to_used_width; }
            int line_spacing() const { return mline_spacing; }

        private:
            void do_wordwrap();
            int calc_height();

            const Font* mfont;
            lgui::Size msize;
            int mallotted_width, mmax_width;
            std::string mtext;
            std::vector <std::string> mlines;
            int mline_spacing;
            bool mshrink_to_used_width;
    };

}

#endif // LGUI_WORDWRAPPEDTEXT_H
