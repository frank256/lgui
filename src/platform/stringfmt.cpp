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

#include "stringfmt.h"
#include "utf8.h"
#include "error.h"

namespace lgui {

StringFmt::StringFmt(const std::string& str)
    : mstr(str)
{}

StringFmt::StringFmt(const char* str)
    : mstr(str)
{}

StringFmt& StringFmt::arg(const std::string& replace, int fieldwidth, int padchar)
{
    int num = find_next_arg_insertion_point();
    if(num > 0) {
        replace_arg_insertion_point(num, replace, fieldwidth, padchar);
    }
    else {
        warning("No escape sequence found for argument.");
    }
    return *this;
}

StringFmt& StringFmt::arg(const char *replace, int fieldwidth, int padchar)
{
    return arg(std::string(replace), fieldwidth, padchar);
}

StringFmt& StringFmt::arg(int replace, int fieldwidth, int padchar)
{
    arg(std::to_string(replace), fieldwidth, padchar);
    return *this;
}

StringFmt& StringFmt::arg(double replace, int precision, int fieldwidth, int padchar)
{
    char fmt[32];
    char temp[128];
    snprintf(fmt, 32, "%%.%df", precision);
    snprintf(temp, 128, fmt, replace);
    return arg(temp, fieldwidth, padchar);
}


// finds lowest number of %1 %2 %3 - %99 in string
int StringFmt::find_next_arg_insertion_point()
{
    size_t pos = 0;
    int min_number = 100;

    size_t size = mstr.size();

    while(true) {
       pos = mstr.find('%', pos);
       if(pos == std::string::npos)
           break;

       char c1=-1, c2=-1;

       if(pos+1 < size)
           c1 = mstr[pos+1];
       if(pos+2 < size)
           c2 = mstr[pos+2];
       pos++;

       // end of string reached
       if(c1 < 0) break;

       // something useful after '%'?
       int number;
       if(c1 >= '0' && c1 <= '9')
           number = c1 - '0';
       else
           continue;

       if(c2 >= '0' && c2 <= '9')
           number = 10 * number + c2 - '0';

       // got a number:
       min_number = std::min(min_number, number);
    }

    // nothing found:
    if(min_number == 100)
        return -1;
    return min_number;
}

void StringFmt::replace_arg_insertion_point(int number, const std::string& replace_with,
                                            int fieldwidth, int padchar)
{
    ASSERT(number >= 1 && number <= 99);

    char escapestr[4];

    escapestr[3] = '\0';
    escapestr[0] = '%';
    if(number >= 10) {
        escapestr[1] = '0' + number / 10;
        escapestr[2] = '0' + number % 10;
    }
    else {
        escapestr[1] = '0' + number;
        escapestr[2] = '\0';
    }

    if(fieldwidth > 0 && fieldwidth > signed(utf8::length_cps(replace_with))) {
        int npre = fieldwidth - utf8::length_cps(replace_with);
        std::string prepend;
        for(int i = 0; i < npre; i++)
            utf8::append_chr(prepend, padchar);
        prepend.append(replace_with);

        utf8::replace_all(mstr, escapestr, prepend);
    }
    else
        utf8::replace_all(mstr, escapestr, replace_with);
}

}
