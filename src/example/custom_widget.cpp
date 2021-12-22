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

#include "lgui/platform/graphics.h"
#include "lgui/style/style.h"

#include "shared.h"
#include "lgui/basiccontainer.h"
#include "lgui/layout/flowlayout.h"
#include "lgui/layout/alignlayout.h"

// A simple widget that tracking mouse move and drag positions.
class CustomWidget : public lgui::Widget {

    public:

        CustomWidget() {}

        void draw(const lgui::DrawEvent& de) const override {
            // Fill background depending on the hover state.
            double v = is_mouse_down ? (is_hovered() ? 0.3 : 0.4) : (is_hovered() ? 1.0 : 0.8);
            de.gfx().fill_draw_area(lgui::grey(v));

            if (is_mouse_down) {
                // Draw a line to drag position clipped to widget dims.
                auto clipped = size_rect().clipped_point(mdrag);
                de.gfx().line_p05(mtrack.x(), mtrack.y(), clipped.x(), clipped.y(), lgui::rgb(0.7, 0.7, 0.0), 1);
            }

            // Draw a small indicator where the mouse is / last was (if initialized).
            if (mtrack != lgui::Point{-1, -1}) {
                de.gfx().draw_visible_pixel(mtrack.x(), mtrack.y(), lgui::rgb(0.8, 0.0, 0.0));
            }
        }

        lgui::Size min_size_hint() override {
            // Reserve at least 64x64 pixels for this widget when put into a layout.
            return lgui::Size(64, 64);
        }

        lgui::MeasureResults measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) override {
            // Reconcile min size hint with the constraints passed.
            // The return value of measure must never violate the constraints.
            return force_size_constraints(min_size_hint(), wc, hc);
        }

    protected:

        void mouse_moved(lgui::MouseEvent& event) override {
            // Track the mouse position.
            mtrack = event.pos();
            event.consume();
        }

        void mouse_dragged(lgui::MouseEvent& event) override {
            // Track the mouse position while button down.
            mdrag = event.pos();
            event.consume();
        }

        void mouse_pressed(lgui::MouseEvent& event) override {
            // Track whether mouse pressed and initialize drag tracking.
            is_mouse_down = true;
            mdrag = event.pos();
            event.consume();
        }

        void mouse_released(lgui::MouseEvent& event) override {
            // Track whether mouse released.
            is_mouse_down = false;
            event.consume();
        }

    private:
        lgui::Position mtrack{-1, -1};
        lgui::Position mdrag{-1, -1};
        bool is_mouse_down = false;
};

class CustomWidgetLguiExample : public LguiExample {

    public:
        void setup(lgui::Size display_size) override {
            LguiExample::setup(display_size);

            // Create some widgets and add them to layout.
            for (int i = 0; i < 16; ++i) {
                mcustom_widgets.push_back(std::make_unique<CustomWidget>());
                mlayout.add_item(*mcustom_widgets.back());
            }

            // Configure layout.
            mlayout.set_max_on_row(4);
            mlayout.set_horizontal_spacing(10);
            mlayout.set_vertical_spacing(10);

            // Add another parent layout wrapping the laid out widgets in order to center them.
            mouter_layout.add_item({mlayout, lgui::Alignment::HVCenter});

            // Set parent layout on the container and set it to full size.
            mcontainer.set_layout(&mouter_layout);
            mcontainer.set_size(display_size);

            // Set container as GUI top widget.
            gui.push_top_widget(mcontainer);
        }

        const char* get_name() const override {
            return "lgui custom widget example";
        }

    private:
        std::vector<std::unique_ptr<CustomWidget>> mcustom_widgets;
        lgui::FlowLayout mlayout;
        lgui::AlignLayout mouter_layout;
        lgui::BasicContainer mcontainer;
};

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    run_example<CustomWidgetLguiExample>();

    return 0;
}

