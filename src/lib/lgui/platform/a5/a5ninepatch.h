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

#ifndef LGUI_A5_NINEPATCH_H
#define LGUI_A5_NINEPATCH_H

#include "../bitmap.h"
#include "../ninepatchbase.h"


namespace lgui {
class Graphics;

/** Platform specific 9-patch image drawing code. Plugged into the class itself for ease of access.
 *  Do not instantiate directly, but use NinePatch, use the Graphics class to draw everything.
 */
class A5Ninepatch : public NinepatchBase {
        friend class Graphics;

    public:
        /** C'tor that will read a single bitmap. */
        explicit A5Ninepatch(Bitmap& src);

        /** C'tor that will read part of a bitmap. */
        A5Ninepatch(Bitmap& src, int offsx, int offsy, int w, int h);

        /** Move c'tor. */
        explicit A5Ninepatch(A5Ninepatch&& other) noexcept;


    protected:
        // only accessible from Graphics class
        /** Draw a tinted 9-patch with a certain content size. */
        void draw_tinted(const lgui::Color& col, float dx, float dy,
                         const lgui::Size& content_size) const;
};

}

#endif // LGUI_A5_NINEPATCH_H
