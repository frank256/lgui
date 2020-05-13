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

#include "../bitmap.h"
#include "../error.h"
#include <allegro5/allegro_opengl.h>

namespace lgui {

A5Bitmap::A5Bitmap(const char* filename, bool filter)
    : mname(filename), mfilter(filter)
{
    load(filename);
}

A5Bitmap::A5Bitmap(A5Bitmap&& bmp) noexcept
    : mbmp(bmp.mbmp), mname(std::move(bmp.mname)), mfilter(bmp.mfilter)
{
    bmp.mbmp = nullptr;
    bmp.mname = "";
}

A5Bitmap::A5Bitmap(int w, int h)
 : mfilter(false)
{
    ASSERT(w > 0 && h > 0);
    mbmp = al_create_bitmap(w, h);
    if(mbmp == nullptr) {
        error("Error creating bitmap!", "Error creating bitmap!");
    }
}

void A5Bitmap::load(const char* filename)
{
    if(mfilter)
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_MIPMAP);
    else
        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    mbmp = al_load_bitmap(filename);
    if(mbmp == nullptr) {
        error("Error loading file", "Couldn't load file \"%s\"!", filename);
    }
}


int A5Bitmap::w() const
{
    ASSERT(mbmp);
    return al_get_bitmap_width(mbmp);
}

int A5Bitmap::h() const
{
    ASSERT(mbmp);
    return al_get_bitmap_height(mbmp);
}

void A5Bitmap::lock_for_reading()
{
    ASSERT(mbmp);
    if(!al_is_bitmap_locked(mbmp))
       al_lock_bitmap(mbmp, al_get_bitmap_format(mbmp), ALLEGRO_LOCK_READONLY);
}

void A5Bitmap::unlock()
{
    ASSERT(mbmp);
    if(al_is_bitmap_locked(mbmp))
       al_unlock_bitmap(mbmp);
}

void A5Bitmap::clear_to_transparent()
{
    ASSERT(mbmp);
    ALLEGRO_LOCKED_REGION* region = al_lock_bitmap(mbmp, ALLEGRO_PIXEL_FORMAT_ABGR_8888,
                                                   ALLEGRO_LOCK_WRITEONLY);
    ASSERT(region->pixel_size == 4);
    // FIXME: correct?
    for(int y = 0; y < h(); y++) {
        for(int x = 0; x < w(); x++) {
            ((uint32_t*)region->data)[y*region->pitch+x] = 0;
        }
    }
    al_unlock_bitmap(mbmp);
}


lgui::Color A5Bitmap::getpixel(int x, int y) const
{
    ASSERT(mbmp);
    return al_get_pixel(mbmp, x, y);
}


void A5Bitmap::reload()
{
    ASSERT(!mname.empty());
    ASSERT(mbmp == nullptr);
    load(mname.c_str());
}

void A5Bitmap::unload()
{
    if(mbmp)
        al_destroy_bitmap(mbmp);
    mbmp = nullptr;
}

A5Bitmap::~A5Bitmap()
{
    if(mbmp)
        al_destroy_bitmap(mbmp);
}

}
