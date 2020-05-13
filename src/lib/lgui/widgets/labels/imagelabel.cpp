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

#include "imagelabel.h"
#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"

namespace lgui {

    ImageLabel::ImageLabel()
        : mbmp(nullptr)
    {
        set_active(false);
    }

    ImageLabel::ImageLabel(Bitmap& bmp)
        : mbmp(&bmp)
    {
        set_active(false);
    }

    void ImageLabel::set_image(Bitmap& bmp)
    {
        mbmp = &bmp;
        request_layout();
    }

    void ImageLabel::draw(const DrawEvent& de) const
    {
        if (mbmp)
            de.gfx().draw_tinted_bmp(*mbmp, 0, 0, grey_premult(1.0, de.opacity()));
    }

    MeasureResults ImageLabel::measure(SizeConstraint wc, SizeConstraint hc)
    {
        if (mbmp)
            return force_size_constraints(Size(mbmp->w(), mbmp->h()), wc, hc);
        else
            return force_size_constraints(Size(), wc, hc);
    }

    Size ImageLabel::min_size_hint()
    {
        if (mbmp)
            return Size(mbmp->w(), mbmp->h());
        else
            return Size();
    }
}
