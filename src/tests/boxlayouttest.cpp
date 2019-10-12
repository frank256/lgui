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

#include "boxlayouttest.h"

#include "platform/graphics.h"
#include "lgui/drawevent.h"

void BoxLayoutTestWidget::draw(const lgui::DrawEvent& de) const {
    de.gfx().filled_rect(size_rect(), lgui::rgba_premult(0.0, 0.0, 1.0, de.opacity()));
    de.gfx().draw_textc(font(), width() / 2, (height() - font().line_height()) / 2,
                        lgui::grey_premult(1.0, de.opacity()), mstr);
}

BoxLayoutTest::BoxLayoutTest() {
    for (unsigned int j = 0; j < 7; j++) {
        mlayouts.emplace_back(std::make_unique<lgui::VBoxLayout>());
        auto& layout = *mlayouts.back();

        for (unsigned int i = 0; i < 4; i++) {
            mwidgets.emplace_back(std::make_unique<BoxLayoutTestWidget>(std::to_string(i + 1)));
            layout.add_item({*mwidgets.back(), lgui::Align::HCenter}, i < 3 ? 0 : 10);
            layout.add_spacing(10);
        }

        mww_labels.emplace_back(std::make_unique<lgui::WordWrapTextLabel>(mstatic_text));

        layout.add_item({*mww_labels.back()}, 60);

        mcontainers.emplace_back(std::make_unique<BasicContainer>());
        auto& container = *mcontainers.back();
        container.set_layout(&layout);

        mlayout.add_item(container, j + 1);
        mlayout.add_spacing(10);
    }
    set_layout(&mlayout);
}

void BoxLayoutTest::draw_background(const lgui::DrawEvent& de) const {
    de.gfx().rect(size_rect(), lgui::rgb(1.0, 1.0, 1.0), 1);
}
