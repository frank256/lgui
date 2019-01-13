/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#include "wordwraptest.h"

#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "lgui/mouseevent.h"
#include "lgui/buttongroup.h"

WordWrapTestWidget::WordWrapTestWidget()
{
    set_text(mstatic_text);
    set_active(true);
}

void WordWrapTestWidget::draw(const lgui::DrawEvent& de) const {
    lgui::WordWrapTextLabel::draw(de);
    de.gfx().line(width()-2, 0, width()-2, height(), lgui::rgba_premult(0, 1.0, 0.0, de.opacity()), 2);
}

void WordWrapTestWidget::mouse_pressed(lgui::MouseEvent& event) {
    mouse_dragged(event);
}

void WordWrapTestWidget::mouse_dragged(lgui::MouseEvent& event) {
    if(event.x() > 0) {
        set_size(event.x(), height());
        event.consume();
    }
}

WordWrapTest::WordWrapTest()
: mlabel("Word wrap test")
{
    mwwtest.set_pos(0, 30);
    mwwtest.set_size(200, 500);
    add_child(mwwtest);
    add_child(mlabel);
}
