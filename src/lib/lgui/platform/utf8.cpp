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

#include "utf8.h"

// Large parts of this file are more or less copied from the great Allegro5 project.

#define IS_SINGLE_BYTE(c)  (((unsigned)(c) & 0x80) == 0)
#define IS_LEAD_BYTE(c)    (((unsigned)(c) - 0xC0) < 0x3E)
#define IS_TRAIL_BYTE(c) (((unsigned)(c) & 0xC0) == 0x80)

namespace lgui {
namespace utf8 {

bool next_cp(const std::string& str, size_t& pos) {
    const unsigned char* data = (const unsigned char*) str.data();
    size_t size = str.size();

    if (pos >= size)
        return false;

    while (++pos < size) {
        int c = data[pos];
        if (IS_SINGLE_BYTE(c) || IS_LEAD_BYTE(c))
            break;
    }
    return true;
}

bool prev_cp(const std::string& str, size_t& pos) {
    const unsigned char* data = (const unsigned char*) str.data();
    size_t size = str.size();

    if (pos > size || pos == 0)
        return false;

    while (pos > 0) {
        pos--;
        int c = data[pos];
        if (IS_SINGLE_BYTE(c) || IS_LEAD_BYTE(c))
            break;
    }
    return true;
}

size_t length_cps(const std::string& str) {
    size_t pos = 0, cps = 0;

    while (next_cp(str, pos))
        cps++;

    return cps;
}

size_t length_cps_substr(const std::string& str, size_t start, size_t endoffs) {
    size_t pos = start, cps = 0;

    while (pos < endoffs && next_cp(str, pos))
        cps++;

    return cps;
}

size_t cp_width(int32_t c) {
    uint32_t uc = c;

    if (uc <= 0x7f)
        return 1;
    if (uc <= 0x7ff)
        return 2;
    if (uc <= 0xffff)
        return 3;
    if (uc <= 0x10ffff)
        return 4;
    // others not valid
    return 0;
}

// s should point to a buffer holding at least 4 chars
static size_t encode_chr(char s[], int32_t c) {
    uint32_t uc = c;

    if (uc <= 0x7f) {
        s[0] = uc;
        return 1;
    }

    if (uc <= 0x7ff) {
        s[0] = 0xC0 | ((uc >> 6) & 0x1F);
        s[1] = 0x80 | (uc & 0x3F);
        return 2;
    }

    if (uc <= 0xffff) {
        s[0] = 0xE0 | ((uc >> 12) & 0x0F);
        s[1] = 0x80 | ((uc >> 6) & 0x3F);
        s[2] = 0x80 | (uc & 0x3F);
        return 3;
    }

    if (uc <= 0x10ffff) {
        s[0] = 0xF0 | ((uc >> 18) & 0x07);
        s[1] = 0x80 | ((uc >> 12) & 0x3F);
        s[2] = 0x80 | ((uc >> 6) & 0x3F);
        s[3] = 0x80 | (uc & 0x3F);
        return 4;
    }

    // others not valid
    return 0;
}

std::string encode_chr(int32_t c) {
    char buf[5];
    size_t n = encode_chr(buf, c);
    buf[n] = '\0';
    return std::string(buf);
}

size_t find_chr(const std::string& str, int32_t c, size_t pos) {
    char buf[5];
    size_t n = encode_chr(buf, c);
    if (n <= 0)
        return std::string::npos;
    else if (n == 1)
        return str.find(buf[0], pos);
    else
        return str.find(buf, pos, n);
}

size_t rfind_chr(const std::string& str, int32_t c, size_t pos) {
    char buf[5];
    size_t n = encode_chr(buf, c);
    if (n <= 0)
        return std::string::npos;
    else if (n == 1)
        return str.rfind(buf[0], pos);
    else
        return str.rfind(buf, pos, n);
}

void append_chr(std::string& str, int32_t c) {
    char buf[5];
    size_t n = encode_chr(buf, c);
    if (n <= 0)
        return;
    else if (n == 1)
        str.push_back(buf[0]);
    else {
        str.append(buf, n);
    }
}

size_t insert_chr(std::string& str, size_t pos, int32_t c) {
    char buf[5];
    size_t n = encode_chr(buf, c);
    if (n >= 1)
        str.insert(pos, buf, n);
    return n;
}

int32_t get_at_offs(const std::string& str, size_t pos) {
    int32_t c;
    int remain;
    int32_t minc;

    size_t size = str.size();

    if (pos >= size)
        return -1;

    const unsigned char* data = (const unsigned char*) str.data();

    c = data[pos];

    if (c <= 0x7F) {
        // Plain ASCII
        return c;
    }

    if (c <= 0xC1) {
        // Trailing byte of multi-byte sequence or an overlong encoding for
        // code point <= 127
        return -2;
    }

    if (c <= 0xDF) {
        // 2-byte sequence
        c &= 0x1F;
        remain = 1;
        minc = 0x80;
    }
    else if (c <= 0xEF) {
        // 3-byte sequence
        c &= 0x0F;
        remain = 2;
        minc = 0x800;
    }
    else if (c <= 0xF4) {
        // 4-byte sequence
        c &= 0x07;
        remain = 3;
        minc = 0x10000;
    }
    else {
        // Otherwise invalid
        return -2;
    }

    if (pos + remain > size)
        return -2;


    while (remain--) {
        ++pos;
        if (pos >= size)
            return -2;
        int d = data[pos];

        if (!IS_TRAIL_BYTE(d))
            return -2;

        c = (c << 6) | (d & 0x3F);
    }

    /* Check for overlong forms, which could be used to bypass security
    * validations.  We could also check code points aren't above U+10FFFF or in
    * the surrogate ranges, but we don't.
    */
    if (c < minc) {
        return -2;
    }

    return c;
}

int32_t get_cp_next(const std::string& str, size_t& pos) {
    int32_t c = get_at_offs(str, pos);
    if (c >= 0) {
        pos += cp_width(c);
        return c;
    }
    if (c == -1) {
        return c;
    }
    // Skip invalid bytes.
    next_cp(str, pos);
    return c;
}

bool remove_chr(std::string& str, size_t pos) {
    int32_t c;
    size_t w;
    c = get_at_offs(str, pos);

    if (c < 0)
        return false;
    w = cp_width(c);
    str.erase(pos, w);
    return true;
}

void replace_all(std::string& str, const std::string& find, const std::string& replace,
                 size_t start_pos) {
    if (find.empty() || find.size() > str.size())
        return;
    if (start_pos > str.size() - find.size())
        return;

    std::string res;
    res.reserve(str.length());

    size_t pos;
    while ((pos = str.find(find, start_pos)) != std::string::npos) {
        res += str.substr(start_pos, pos - start_pos);
        res += replace;
        pos += find.length();
        start_pos = pos;
    }
    res += str.substr(start_pos);
    str.swap(res);
}

size_t skip_to_next_word_boundary(const std::string& str, size_t offs, bool backwards) {
    const char* const whitespace = " \n\r\t";
    size_t f = offs;
    if (backwards) {
        if (!utf8::prev_cp(str, f))
            return 0;
        f = str.find_last_of(whitespace, f);
        if (f == std::string::npos)
            f = 0;
        else {
            if (f == offs - 1) {
                // spaces => skip them
                f = str.find_last_not_of(whitespace, offs - 1);
                if (f == std::string::npos)
                    return 0;
            }
            // move to beginning of skipped "word"
            utf8::next_cp(str, f);
        }
    }
    else {
        f = str.find_first_of(whitespace, offs);
        if (f == std::string::npos)
            f = str.size();
        else {
            if (f == offs) {
                // spaces => skip them
                f = str.find_first_not_of(whitespace, offs);
                if (f == std::string::npos)
                    f = str.size();
            }
        }
    }
    return f;
}

}

}
