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

#include "windowframetest.h"


WindowFrame::WindowFrame(const std::string& caption)
    : Frame(caption),
      mtextfield("I'm some text someone entered because he couldn't think of something more interesting at the time."),
      mtextfield2("Change me"),
      mtextfield3("The test buttons below me are switched when you click them..."),
      mbutton1("Button1", 100),
      mbutton2("Button2", 100)
{
    mlayout.set_default_alignment(lgui::Align::HMatchParent);
    int maxw = std::max(mbutton1.width(), mbutton2.width());
    mbutton1.set_size(maxw, maxw);
    mbutton2.set_size(maxw, maxw);

    mbutton_container.add_child(mbutton1);
    mbutton_container.add_child(mbutton2);
    mbutton_container.set_size(maxw, maxw);

    mbutton1.set_invisible();

    mbutton1.on_activated.connect([this]() {
        mbutton1.set_invisible();
        mbutton2.set_visible();
        invalidate_under_mouse();
    });
    mbutton2.on_activated.connect([this]() {
        mbutton2.set_invisible();
        mbutton1.set_visible();
        invalidate_under_mouse();
    });

    mlayout.add_spacing(8);
    mlayout.add_item(mtextfield);
    mlayout.add_spacing(16);
    mlayout.add_item(mtextfield2);
    mlayout.add_spacing(16);
    mlayout.add_item(mtextfield3);
    mlayout.add_spacing(16);
    mlayout.add_item({mbutton_container, lgui::Align::HCenter});
    set_layout(&mlayout);
}

static void setup_frame_size(lgui::Frame& frame, int min_width) {
    lgui::Size size_hint = frame.min_size_hint();
    min_width = std::max(min_width, frame.font().text_width(frame.title()) + 2*frame.font().char_width_hint());

    size_hint.set_w(std::max(size_hint.w(), min_width));
    frame.set_size(size_hint);
}

WindowFrameTest::WindowFrameTest()
    : mframe1("Test 1"),
      mframe2("Test 2: inactive"),
      mframe3("Test 3")
{
    int min_w = 15*font().char_width_hint();

    setup_frame_size(mframe1, min_w);
    setup_frame_size(mframe2, min_w);
    setup_frame_size(mframe3, min_w);

    add_child(mframe2);
    add_child(mframe1);
    add_child(mframe3);
    mframe2.set_pos(200, 150);
    mframe3.set_pos(300, 250);
    mframe2.set_active(false);
}

lgui::MeasureResults WindowFrameTest::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc)
{
    return lgui::force_size_constraints(lgui::Size(wc.value(), hc.value()), wc, hc);
}

