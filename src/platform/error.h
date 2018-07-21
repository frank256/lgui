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

#ifndef ERROR_H
#define ERROR_H

#include "a5/a5error.h"

namespace lgui {

/** Maximum bytes to reserve for error messages (processed format strings). */
const unsigned int ERROR_MSG_BUFFER_SIZE = 1024;

/** Printf style function to display a fatal error message and terminate the program. */
void error(const char *heading, const char *format, ...);
/** Printf style function to display (log) a warning message. */
void warning(const char *format, ...);
/** Printf style function to display (log) a debug message. */
void debug(const char *format, ...);
/** Printf style function to display (log) an informational message. */
void info(const char *format, ...);

void _handle_assert(const char *expression, const char *_file_, int _line_);
void _handle_assert(const char *expression, const char *_file_, int _line_, const char* msg);

}

#ifdef __clang_analyzer__
// make Clang-analyzer understand our ASSERT
#include <cassert>
#define ASSERT(e) assert(e)
#define ASSERT_MSG(e,msg) assert(e)
#else
#define ASSERT(e) ((e) ? (void)0 : lgui::_handle_assert(#e, __FILE__, __LINE__))
#define ASSERT_MSG(e,msg) ((e) ? (void)0 : lgui::_handle_assert(#e, __FILE__, __LINE__, msg))
#endif

#endif // ERROR_H

