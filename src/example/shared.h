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

#ifndef LGUIEXAMPLES_SHARED_H
#define LGUIEXAMPLES_SHARED_H

#include "lgui/gui.h"

class LguiExample {
    public:
        virtual void setup(lgui::Size display_size) { (void) display_size; }

        virtual void tear_down() {
            gui.pop_top_widget();
        }

        virtual void draw(lgui::Graphics& gfx) {
            // Use the passed lgui graphics wrapper to draw the widgets of the GUI.
            gui.draw_widgets(gfx);
        }

        virtual bool handle_a5_event(const ALLEGRO_EVENT& allegro_event) {
            // Convert Allegro 5 event to lgui event.
            lgui::ExternalEvent event;
            if (!lgui::convert_a5_event(allegro_event, event))
                return false; // Not all A5 events have their lgui equivalent.

            // Push event into GUI, triggering delivery to widgets.
            gui.push_external_event(event);

            return true;
        }

        virtual const char* get_name() const {
            // Is used as a window title.
            return "lgui example";
        }

    protected:
        lgui::GUI gui;
};

void run_example(std::function<std::unique_ptr<LguiExample>()>&& creator);

template<class T>
void run_example() {
    run_example(std::make_unique<T>);
}

#endif //LGUIEXAMPLES_SHARED_H
