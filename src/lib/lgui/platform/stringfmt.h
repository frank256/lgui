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

#ifndef LGUI_STRINGFMT_H
#define LGUI_STRINGFMT_H

#include <string>

namespace lgui {
/** Helper class for Qt-style formatted strings. Placeholders like %1, %2 will be replaced with the text from
 *  the first, second, etc. calls to one of the arg methods. The arg-methods will return the object itself
 *  so you can chain them. */
class StringFmt {
    public:
        StringFmt(const std::string& str);
        StringFmt(const char *str);

        StringFmt& arg(const std::string& replace, int fieldwidth=-1, int padchar=' ');
        StringFmt& arg(const char *replace, int fieldwidth=-1, int padchar=' ');
        StringFmt& arg(int replace, int fieldwidth=-1, int padchar=' ');
        StringFmt& arg(double replace, int precision=2, int fieldwidth=-1, int padchar=' ');

        operator std::string() const { return mstr; }

        std::string to_string() const { return mstr; }
        const char *c_str() const { return mstr.c_str(); }

    private:
        int find_next_arg_insertion_point();
        void replace_arg_insertion_point(int number, const std::string& replace_with,
                                     int fieldwidth=-1, int padchar=' ');
        std::string mstr;
};

}
#endif // LGUI_STRINGFMT_H
