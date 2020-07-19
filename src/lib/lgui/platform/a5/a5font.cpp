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

#include <cstring>

#include "../font.h"
#include "../utf8.h"
#include "../error.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

namespace lgui {

A5Font::A5Font(const std::string& filename, int size) {
    mfnt = al_load_ttf_font(filename.c_str(), size, 0);
    if (!mfnt) {
        error("Couldn't load font", "Couldn't load font \"%s\".", filename.c_str());
    }
}

A5Font::~A5Font() {
    if (mfnt) {
        al_destroy_font(mfnt);
        mfnt = nullptr;
    }
}

int A5Font::ascent() const {
    return al_get_font_ascent(mfnt);
}

int A5Font::descent() const {
    return al_get_font_descent(mfnt);
}

int A5Font::line_height() const {
    return al_get_font_line_height(mfnt);
}

int A5Font::char_width_hint() const {
    return text_width("M");
}

int A5Font::text_width(const std::string& str) const {
    return al_get_text_width(mfnt, str.c_str());
}

int A5Font::text_width(const char* str) const {
    return al_get_text_width(mfnt, str);
}

int A5Font::text_width(const std::string& str, size_t offs, size_t n) const {
    if (offs >= str.size())
        return 0;
    if (n > str.size() - offs) // will catch npos
        n = str.size() - offs;
    ALLEGRO_USTR_INFO info;
    const ALLEGRO_USTR* ref = al_ref_buffer(&info, str.data() + offs, n);
    return al_get_ustr_width(mfnt, ref);
}

lgui::Rect A5Font::text_dims(const std::string& str) const {
    int bbx, bby, bbw, bbh;
    al_get_text_dimensions(mfnt, str.c_str(), &bbx, &bby, &bbw, &bbh);
    return lgui::Rect(bbx, bby, bbw, bbh);
}

}
