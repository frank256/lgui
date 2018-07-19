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

#ifndef LGUI_NINEPATCH_H
#define LGUI_NINEPATCH_H

#include "a5/a5ninepatch.h"

namespace lgui {
using NinepatchImplementation = A5Ninepatch;
}


namespace lgui {
class Graphics;

/** A class for drawing 9-patch images. Will read it from a specifically formatted
 * image. 9-patch images have 9 regions with 4 of them being stretched in one
 * direction (left, right, top, bottom), 1 stretched in 2 directions (center)
 * and 4 not being stretched at all (corners). 9-patch images as used here have
 * another rectangle that specifies the fill area, i.e. where the content should go.
 *
 * Use the methods of the Graphics class to draw NinePatch images.
*/
class NinePatch : public NinepatchImplementation {
    public:
        NinePatch(Bitmap& src, int offsx, int offsy, int w, int h)
            : NinepatchImplementation(src, offsx, offsy, w, h) {}

        explicit NinePatch(Bitmap& src)
            : NinepatchImplementation(src) {}

        NinePatch(NinePatch&& other)
            : NinepatchImplementation(std::forward<NinePatch>(other)) {}

};

}

#endif // LGUI_NINEPATCH_H
