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

#include "font.h"
#include "utf8.h"

namespace lgui {

std::pair<size_t, size_t> Font::hit_char(const std::string& str, int px) const
{
    if(px <= 0 || str.empty())
        return { 0, 0 };

    size_t start_offs = 0,
           end_offs = str.size(),
           start_cps = 0,
           end_cps = utf8::length_cps(str);

    int start_x = 0, end_x = text_width(str);

    if(px >= end_x)
        return { str.size(), end_cps };

    // Use binary search
    while(start_cps < end_cps-1) {
        int mid = (start_cps + end_cps) / 2;

        // Go to mid offset
        size_t mid_offs = start_offs;
        for(size_t i = 0; i < mid - start_cps; i++)
            utf8::next_cp(str, mid_offs);

        // FIXME: kerning?
        // Maybe rather use: int mx = text_width(str, 0, mid_offs); // ?
        int mx = start_x + text_width(str, start_offs, mid_offs - start_offs);

        if(px == mx) {
            // Direct hit: start of character
            return { mid_offs, mid };
        }
        else if(px > mx) {
            // Continue in right interval
            start_cps = mid;
            start_offs = mid_offs;
            start_x = mx;
        }
        else { // px < mx
            // Continue in left interval
            end_cps = mid;
            end_offs = mid_offs;
            end_x = mx;
        }
    }

    // Nearer to next character?
    if(end_x-px < px-start_x)
        return { end_offs, end_cps };
    else
        return { start_offs, start_cps };
}


// helper for WordWrap
struct LineAdder {
        LineAdder(const Font& font, std::vector <std::string>&out_lines)
            : mfont(font), mout_lines(out_lines),
              mwidest_line(0)
        {}

        void add(const std::string& text, size_t offs, size_t size)
        {
            std::string line = text.substr(offs, size);
            mout_lines.emplace_back(line);
            int w = mfont.text_width(mout_lines.back());
            mwidest_line = std::max(mwidest_line, w);
        }

        int widest_line() const { return mwidest_line; }

        const Font& mfont;
        std::vector <std::string>& mout_lines;
        int mwidest_line;
};

int Font::do_wordwrap(const std::string& text, int max_width, std::vector <std::string>& out_lines) const
{
    const char *whitespace = " \t\r\n";

    LineAdder la(*this, out_lines);

    if(text.empty() || max_width <= 0)
        return 0;

    const int space_width = text_width(" ");
    const size_t npos = std::string::npos;

    bool finished = false;
    size_t line_start = 0, last_word = 0,
           next_word = text.find_first_of(whitespace);
    int line_width = 0;

    while(!finished) {
        int word_width = text_width(text, last_word,
                                    (next_word != npos) ? next_word - last_word : npos);
        if(line_width + word_width > max_width) {
            // Line would be too long with this word.
            bool split_word = word_width > max_width;

            if(split_word) {
                size_t word_size;
                if(next_word != npos)
                    word_size = next_word - last_word;
                else
                    word_size = text.size() - last_word;
                unsigned int split_idx = 1;
                for(; split_idx < word_size; split_idx++) {
                    int part_word_w = text_width(text, last_word, split_idx);
                    if(part_word_w + line_width > max_width) {
                        break;
                    }
                }
                split_idx--;

                // In case even one character won't fit:
                // Force emitting at least one character to avoid endless loop
                if(split_idx == 0 && line_start == last_word)
                    split_idx++;

                // Avoid trailing spaces that destroy our max width when split_idx == 0
                // (this is different from TextBox::make_rows).
                la.add(text, line_start,
                       last_word + split_idx - line_start - ((split_idx == 0) ? 1 : 0));
                last_word = last_word + split_idx;
            }
            else {
                if(line_start < last_word) {
                    // Emit start of line until current word
                    la.add(text, line_start, last_word - line_start-1);
                }
            }
            line_start = last_word;
            line_width = 0;
        }
        else if(next_word != npos && text[next_word] == '\n') {
            // New line: make new line in any case.
            la.add(text, line_start, next_word - line_start);
            last_word = line_start = next_word + 1;
            line_width = 0;
            next_word = text.find_first_of(whitespace, next_word + 1);
        }
        else if(next_word == npos) {
            // Finished: add last line
            la.add(text, line_start, next_word); // Will clip npos
            finished = true;
        }
        else {
            // "Add" the word to the same line.
            line_width += word_width + space_width;
            last_word = next_word+1;
            next_word = text.find_first_of(whitespace, next_word + 1);
        }
    }
    return la.widest_line();
}
}
