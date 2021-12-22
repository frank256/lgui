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

#include "ninepatchtest.h"

#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"

NinePatchTest::NinePatchTest()
        : mbmp("data/test_9.tga", true),
          mnp(mbmp),
          mnp_size(60, 60) {
    add_child(mwsl);
    add_child(mhsl);
    mwsl.on_value_changed.connect([this](int w) { mnp_size.set_w(w); });
    mhsl.on_value_changed.connect([this](int h) { mnp_size.set_h(h); });
}

void NinePatchTest::draw_background(const lgui::DrawEvent& de) const {
    de.gfx().draw_ninepatch_outer_size(mnp, padding().left(), padding().top(), mnp_size);
}

void NinePatchTest::resized(const lgui::Size& old_size) {
    TestContainer::resized(old_size);
    const int SLIDER_WH = std::max(mwsl.handle_secondary_dim(), mhsl.handle_secondary_dim());
    lgui::Size s = children_area().size();
    mwsl.set_size(s.w() - SLIDER_WH, SLIDER_WH);
    mhsl.set_size(SLIDER_WH, s.h() - SLIDER_WH);
    mwsl.set_pos(0, s.h() - SLIDER_WH);
    mhsl.set_pos(s.w() - SLIDER_WH, 0);
}
