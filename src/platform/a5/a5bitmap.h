/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#ifndef LGUI_A5BITMAP_H
#define LGUI_A5BITMAP_H

#include "../color.h"
#include <string>

namespace lgui {

/** Class representing an Allegro 5 bitmap resource. Do not use this class directly, but rather use Bitmap. */
class A5Bitmap {
    friend class A5Ninepatch;
    friend class A5Graphics;

    public:
        /** Load a file from disk. If `filter` is set to true, filtering is applied when enlarging
         * or shrinking the bitmap. */
        explicit A5Bitmap(const char* filename, bool filter=false);
        /** Creates a new bitmap with the specified width and height. */
        A5Bitmap(int w, int h);
        /** Move constructor. */
        explicit A5Bitmap(A5Bitmap &&bmp) noexcept;

        ~A5Bitmap();

        A5Bitmap(const A5Bitmap& other) = delete;
        A5Bitmap operator=(const A5Bitmap& other) = delete;

        int w() const;
        int h() const;

        /** Locks the bitmap for reading. You should call this before reading large amounts of bitmap data. */
        void lock_for_reading();
        /** Return the color value of a pixel. Consider locking the bitmap via lock_for_reading() before
         *  reading large amounts of pixel data. Also do not forget to unlock the bitmap again. */
        lgui::Color getpixel(int x, int y) const;
        /** Unlocks a bitmap. Call this after you've finished reading pixel data from a bitmap. */
        void unlock();

        /** Unloads the bitmap. The object will remember how to reload it. */
        void unload();
        /** Reloads the bitmap from disk. */
        void reload();

        /** Clears the bitmap to transparent color. */
        void clear_to_transparent();

    protected:
        ALLEGRO_BITMAP* mbmp;

    private:
        void load(const char* filename);

        std::string mname;
        bool mfilter;
};

}


#endif // LGUI_A5BITMAP_H
