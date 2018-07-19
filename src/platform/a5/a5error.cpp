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

#include "../error.h"

#include <cstdio>
#ifdef __ANDROID__
#include <android/log.h>
#else
#include <allegro5/allegro_native_dialog.h>
#endif
#include "../graphics.h"

namespace lgui {

void _error(const char* heading, const char* msg)
{
    lgui::Graphics::_error_shutdown();
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_ERROR, heading, msg);
#else
    al_show_native_message_box(nullptr, "Error!", heading, msg, nullptr, ALLEGRO_MESSAGEBOX_WARN);
#endif
    exit(EXIT_FAILURE);
}

void _debug(const char* msg)
{
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_DEBUG, "lgui", msg);
#else
    fputs(msg, stderr);
#endif
}

void _info(const char* msg)
{
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_INFO, "lgui", msg);
#else
    fputs(msg, stderr);
#endif
}

void _warning(const char* msg)
{
#ifdef __ANDROID__
    __android_log_write(ANDROID_LOG_WARN, "lgui", msg);
#else
    fprintf(stderr, "Warning:\n %s", msg);
#endif
}


}
