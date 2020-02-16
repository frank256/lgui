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

#include "scaledimagebutton.h"
#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "platform/bitmap.h"

namespace lgui {

    ScaledImageButton::ScaledImageButton(Bitmap& normal, Bitmap& highlighted,
                                         Bitmap& clicked)
        : ImageButton(normal, highlighted, clicked)
    {
    }

    void ScaledImageButton::draw(const DrawEvent& de) const
    {
        if(is_down())
            de.gfx().draw_tinted_scaled_bmp(*mclicked, 0, 0, width(), height(),
                                       grey_premult(1.0, de.opacity()));
        else if(is_hovered())
            de.gfx().draw_tinted_scaled_bmp(*mhighlighted, 0, 0, width(), height(),
                                       grey_premult(1.0, de.opacity()));
        else
            de.gfx().draw_tinted_scaled_bmp(*mnormal, 0, 0, width(), height(),
                                       grey_premult(1.0, de.opacity()));
    }

    void ScaledImageButton::resized(const Size& old_size)
    {
        // We take any size
        msize = size();
        ImageButton::resized(old_size);
    }

    Size ScaledImageButton::min_size_hint()
    {
        return Size(1, 1);
    }
}
