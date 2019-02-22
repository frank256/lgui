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

#include "imagebutton.h"
#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "platform/bitmap.h"

namespace lgui {

    ImageButton::ImageButton()
        : AbstractButton(Padding(0)),
          mnormal(nullptr), mhighlighted(nullptr), mclicked(nullptr)
    {}

    ImageButton::ImageButton(Bitmap& normal, Bitmap& highlighted, Bitmap& clicked)
        : AbstractButton(Padding(0))
    {
        set_images(normal, highlighted, clicked);
    }

    void ImageButton::set_images(Bitmap& normal, Bitmap& highlighted, Bitmap& clicked)
    {
        mnormal = &normal;
        mhighlighted = &highlighted;
        mclicked = &clicked;
        int maxw = std::max(mnormal->w(), mhighlighted->w());
        maxw = std::max(maxw, mclicked->w());
        int maxh = std::max(mnormal->h(), mhighlighted->h());
        maxh = std::max(maxh, mclicked->h());
        msize = Size(maxw, maxh);
        request_layout();
    }

    void ImageButton::draw(const DrawEvent& de) const
    {
        if (is_down() && mclicked)
            de.gfx().draw_tinted_bmp(*mclicked, 0, 0, grey_premult(1.0, de.opacity()));
        else if (is_hovered() && mhighlighted)
            de.gfx().draw_tinted_bmp(*mhighlighted, 0, 0, grey_premult(1.0, de.opacity()));
        else if (mnormal)
            de.gfx().draw_tinted_bmp(*mnormal, 0, 0, grey_premult(1.0, de.opacity()));
    }

    MeasureResults ImageButton::measure(SizeConstraint wc, SizeConstraint hc)
    {
        return force_size_constraints(msize, wc, hc);
    }

    Size ImageButton::min_size_hint()
    {
        return msize;
    }


}
