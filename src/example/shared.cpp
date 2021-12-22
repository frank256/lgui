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

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "lgui/platform/graphics.h"
#include "lgui/platform/font.h"
#include "lgui/style/defaultstylecolorscheme.h"
#include "lgui/style/defaultstyle2ndborder.h"

#include "shared.h"

void init_allegro();
ALLEGRO_EVENT_QUEUE* init_allegro_event_queue();
void loop(LguiExample& example);
ALLEGRO_TIMER* create_timer();


void run_example(std::function<std::unique_ptr<LguiExample>()>&& creator) {
    init_allegro();

    // Load font and set global default style using it.
    // For simplicity, everything is kept on the stack here. These should not be destroyed prior to any widgets.
    lgui::Font def_font("data/forgotteb.ttf", 20);

    lgui::DefaultStyleDarkColorScheme dark_scheme;
    lgui::DefaultStyle2ndBorder dark_default_style(def_font, dark_scheme);
    lgui::Widget::set_default_style(&dark_default_style);

    std::unique_ptr<LguiExample> example = creator();

    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_set_window_title(display, example->get_name());

    example->setup({al_get_display_width(display), al_get_display_height(display)});
    loop(*example);
    example->tear_down();
}

void loop(LguiExample& example) {
    ALLEGRO_EVENT_QUEUE* queue = init_allegro_event_queue();

    ALLEGRO_TIMER* timer = create_timer();
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);


    // lgui graphics wrapper. This mainly provides a rectangle stack for internal use by the widgets but also offers
    // some further convenience methods.
    lgui::Graphics gfx;

    // Allegro 5 event loop follows.
    ALLEGRO_EVENT al_ev;
    bool quit = false, redraw = false;

    while (!quit) {
        al_wait_for_event(queue, &al_ev);

        switch (al_ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                quit = true;
                break;

            case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                redraw = false;
                al_stop_timer(timer);
                al_acknowledge_drawing_halt(al_get_current_display());
                break;

            case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                al_acknowledge_drawing_resume(al_get_current_display());
                al_start_timer(timer);
                break;

            case ALLEGRO_EVENT_KEY_CHAR: {
                switch (al_ev.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                    case ALLEGRO_KEY_BACK:
                        quit = true;
                        break;
                    default:
                        break;
                }
            }
                [[fallthrough]];

            default:
                if (!example.handle_a5_event(al_ev)) {
                    continue;
                }

                if (al_ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw = true;
                }
                break;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb_f(0.5, 0.5, 0.5));

            example.draw(gfx);

            al_flip_display();

            redraw = false;
        }
    }
    al_stop_timer(timer);
    al_destroy_event_queue(queue);
}

void init_allegro() {
    al_init();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    ALLEGRO_DISPLAY* display = al_create_display(1024, 576);
    if (!display) {
        lgui::error("Couldn't create display!", "");
    }

    if (!al_install_keyboard())
        lgui::error("Error initialising input", "Couldn't install keyboard!");
    if (!al_install_mouse())
        lgui::error("Error initialising input", "Couldn't install mouse!");
}

ALLEGRO_EVENT_QUEUE* init_allegro_event_queue() {
    ALLEGRO_EVENT_QUEUE* event_queue;

    event_queue = al_create_event_queue();
    if (!event_queue)
        lgui::error("Error initialising input", "Couldn't create event queue!");

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    ALLEGRO_EVENT_SOURCE* display_source = al_get_display_event_source(al_get_current_display());
    al_register_event_source(event_queue, display_source);

    return event_queue;
}

ALLEGRO_TIMER* create_timer() {
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    if (!timer)
        lgui::error("Error initialising events", "Couldn't install timer!");
    return timer;
}

