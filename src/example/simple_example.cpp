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

#include "lgui/platform/graphics.h"
#include "lgui/platform/stringfmt.h"
#include "lgui/style/style.h"

#include "lgui/widgets/pushbutton.h"

#include "shared.h"


class SimpleLguiExample : public LguiExample {

    public:
        void setup(lgui::Size display_size) override {
            LguiExample::setup(display_size);

            button.set_text("Test");
            button.set_pos(100, 100);
            button.set_min_size();
            button.on_activated.connect([this]() { click_counter++; });

            gui.push_top_widget(button);
        }

        void draw(lgui::Graphics& gfx) override {
            // Draw something independent of lgui.
            // We could use normal Allegro drawing routines for this; using lgui's Graphics wrapper is just a convenience
            // here.

            gfx.draw_textc(lgui::Widget::get_default_style()->default_font(), gfx.display_width() / 2,
                           gfx.display_height() / 2, lgui::grey(1.0),
                           lgui::StringFmt("Test button clicked %1 times").arg(click_counter));

            // Then draw the GUI with the widget on top.
            LguiExample::draw(gfx);
        }

        const char* get_name() const override {
            return "lgui simple example";
        }

    private:
        lgui::PushButton button;
        int click_counter = 0;
};

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    run_example<SimpleLguiExample>();

    return 0;
}

