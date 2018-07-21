/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_FONT_H
#define LGUI_FONT_H

#include "a5/a5font.h"

namespace lgui {
using FontImplementation = A5Font;
}

namespace lgui {

/** Class representing a font resource. */
class Font : public FontImplementation {
    public:
        /** Load a font resource from disk. */
        Font(const std::string& filename, int size)
            : FontImplementation(filename, size) {}

        /** Move constructor. */
        Font(Font &&other) : FontImplementation(std::forward<Font>(other)) {}

        /** Breaks a UTF8-string into lines at word boundaries. Words that do not fit
         *  on a single line are split at any location.
         *  @param text the text to process
         *  @param max_width the maximum width that is allowed; it is not
         *         necessarily used completely (if the lines end up shorter).
         *         If there are single characters that won't fit on a line
         *         with max_width, max_width is ignored.
         *         For max_width <= 0, no processing is done.
         *  @param out_lines a reference to a vector holding the output.
         *         It is *not* cleared before adding the lines.
         *  @return the actual width of the longest line in `out_lines`.
         */
        int do_wordwrap(const std::string& text, int max_width, std::vector <std::string>& out_lines) const;

        /** Return offset (first) and codepoint index (second) of the character
         *  hit by a point with x-coordinate px.
         *  There is no failure, the return value will always be between
         *  { 0, 0 } and { str.size(), utf8::length_cps(str) }, i.e. the return value is "clipped".
         */
        std::pair<size_t, size_t> hit_char(const std::string& str, int px) const;

};
}
#endif // LGUI_FONT_H
