/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUI_PRIMHELPER_H
#define LGUI_PRIMHELPER_H

#include "color.h"

namespace lgui {

enum class Corners {
        CornerTopRight = 1, CornerTopLeft = 2, CornersTop = 3,
        CornerBottomLeft = 4, CornersLeft = 6, CornerBottomRight = 8,
        CornersRight = 9, CornersBottom = 12, CornersAll = 15
};
enum class OpenEdge {
        OpenLeft, OpenRight, OpenTop, OpenBottom
};

enum class GradientDirection {
        LeftToRight, TopToBottom,
        RightToLeft, BottomToTop
};

struct PrimVertex {
    float x, y;
    lgui::Color color;
};
}

#include "a5/a5primhelper.h"

namespace lgui {
using PrimHelperImplementation = A5PrimHelper;

/** Helper for drawing primitives using a platform specific backend.
 *  Do not use this directly, but rather use the Graphics object to draw everything.
 */
class PrimHelper : public PrimHelperImplementation {
};

}

#endif // LGUI_PRIMHELPER_H
