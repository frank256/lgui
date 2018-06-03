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

#ifndef LGUI_A5_FONT_H
#define LGUI_A5_FONT_H

#include <vector>
#include <string>
#include "lgui/lgui.h"

struct ALLEGRO_FONT;

namespace lgui {

/** Class representing an Allegro 5 font. Do not use this class directly, but rather use Font. */
class A5Font {
    friend class A5Graphics;
    public:
        /** Loads an Allegro 5 font with the specified size. */
        A5Font(const std::string& filename, int size);
        ~A5Font();

        /** Move constructor. */
        A5Font(A5Font &&other)
            : mfnt(other.mfnt) {
            other.mfnt = nullptr;
        }

        A5Font& operator=(const A5Font& other) = delete;
        A5Font(const A5Font& other) = delete;

        int ascent() const;
        int descent() const;
        /** Return the line height of the font. */
        int line_height() const;

        /** Returns the width of an 'M'-character. */
        int char_width_hint() const;

        /** Return the width the given text will occupy using this font. */
        int text_width(const std::string& str) const;
        /** Return the width the given text will occupy using this font. */
        int text_width(const char *str) const;

        /** Return the width a substring of the given text will occupy using this font.
         * @return the width of the substring starting at offs, n bytes(!) long.
         */
        int text_width(const std::string& str, size_t offs, size_t n) const;
        lgui::Rect text_dims(const std::string& str) const;

    private:
        ALLEGRO_FONT *mfnt;
};

}
#endif // LGUI_A5_FONT_H
