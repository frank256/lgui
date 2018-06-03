/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
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

// This test program expects the following files to be accessible from its working directory:
// data/forgotteb.ttf; data/test_9.tga

#include "platform/error.h"
#include "platform/graphics.h"
#include "platform/font.h"
#include "platform/stringfmt.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "lgui/gui.h"

#include "lgui/style/defaultstyle.h"
#include "lgui/style/defaultstyle2ndborder.h"

#include "all_tests.h"


#ifdef __ANDROID__
#define USE_TOUCH_INPUT
#include <allegro5/allegro_android.h>
#endif


class AllTests : public lgui::Container {
    public:

        lgui::Signal <int> on_style_change_requested;

        AllTests(lgui::GUI& gui, const lgui::Font& small_font)
            : mgui(gui), mddtest(small_font), mmsgbox("", "Ok")
        {
            set_name("AllTests");
            mddtest.reset();
            mscroll_test.reset();

            std::function <void (lgui::AbstractButton *src)> show_button_message =
                    [this](lgui::AbstractButton *src)  {
                auto bbutton = static_cast <BlockButton *> (src);
                if(bbutton) {
                    mmsgbox.set_msg(bbutton->str());
                    mmsgbox.set_min_size();
                    mmsgbox.set_pos(512-mmsgbox.width()/2, 350-mmsgbox.height()/2);
                    mgui.push_top_widget(mmsgbox, true, lgui::grey_premult(0.2, 0.5));
                }
            };
            mddtest.show_message.connect(show_button_message);

            std::function <void (int, const std::string& )> show_list_box_message =
                    [this](int idx, const std::string& entry) {
                mmsgbox.set_msg(lgui::StringFmt("Activated entry #%1: %2").arg(idx).arg(entry.c_str()));
                mmsgbox.set_min_size();
                mmsgbox.set_pos(512-mmsgbox.width()/2, 350-mmsgbox.height()/2);
                mgui.push_top_widget(mmsgbox, true, lgui::grey_premult(0.2, 0.5));
            };
            mlistbox_test.on_entry_activated.connect(show_list_box_message);

            mmenu_layout.set_max_on_row(1);
            mmenu_layout.set_vertical_spacing(4);

            mcontainer.set_name("MainContainer");

            mcontainer.add(mrounded_rect_test);
            mcontainer.add(mwwtest);
            mcontainer.add(mtextbox_test);

            mcontainer.add(mddtest);
            mcontainer.add(mscroll_test);
            mcontainer.add(mlistbox_test);
            mcontainer.add(mdrop_down_test);
            mcontainer.add(mtab_test);
            mcontainer.add(mtextfieldbuttontest);
            mcontainer.add(mslider_test);
            mcontainer.add(mpopup_test);
            mcontainer.add(mbox_layout_test);
            mcontainer.add(mtable_layout_test);
            mcontainer.add(mwindow_frame_test);
            mcontainer.add(mnp_test);
            mcontainer.add(mrelative_test);

            mpopup_test.on_open_popup.connect([this]() { open_another_popup(); });

            create_menu();
            mmenu.set_padding(lgui::Padding(8, 4, 8, 4));
            mmenu.set_layout(&mmenu_layout);
            mtop_layout.add(mmenu);
            mtop_layout.add(mcontainer, 1);
            set_layout(&mtop_layout);

            mcontainer.set_active_widget(&mrounded_rect_test);
            mmenu_buttons[0]->set_checked(true);
    }

    void open_another_popup() {
        Popup* p = new Popup();
        p->set_style(&style());
        p->on_open_another.connect([this]() {open_another_popup(); });
        mpopups.push_back(std::unique_ptr<Popup>(p));

        p->set_min_size();


        float x = float(rand()) / RAND_MAX;
        float y = float(rand()) / RAND_MAX;
        x *= width() - mmenu.width() - p->width()-40;
        y *= height() - p->height()-40;
        x += mmenu.width()+20;
        y += 20;
        p->set_pos(x, y);

        mgui.push_top_widget(*p, true, lgui::grey_premult(0.3, 0.8));
    }

    void add_button(const char* caption, Widget* page) {
        lgui::RadioButton* bt = new lgui::RadioButton(caption);
        mmenu_group.add_button(bt);
        bt->on_activated.connect([this, page](){ mcontainer.set_active_widget(page); });
        mmenu_buttons.push_back(std::unique_ptr<lgui::RadioButton>(bt));
        mmenu_layout.add_item(*bt);
    }

    void create_menu() {
        mmenu_tl.set_text("Pick a test:");
        mmenu.set_name("Menu");
        mmenu_layout.add_item(mmenu_tl);
        add_button("Rounded rect test", &mrounded_rect_test);
        add_button("Word wrap test", &mwwtest);
        add_button("Drap Drop test", &mddtest);
        add_button("TextBox test", &mtextbox_test);
        add_button("Scroll test", &mscroll_test);
        add_button("Window frame test", &mwindow_frame_test);
        add_button("Drop Down test", &mdrop_down_test);
        add_button("Tab test", &mtab_test);
        add_button("Text field/button test", &mtextfieldbuttontest);
        add_button("Listbox test", &mlistbox_test);

        add_button("Slider test", &mslider_test);
        add_button("Popup test", &mpopup_test);
        add_button("BoxLayout test", &mbox_layout_test);
        add_button("TableLayout test", &mtable_layout_test);
        add_button("NinePatch test", &mnp_test);
        add_button("Relative layout test", &mrelative_test);


        mstyle_choser.model().add_item("Dark style");
        mstyle_choser.model().add_item("Bright style");
        mstyle_choser.set_selected(0);
        mmenu_layout.add_item(mstyle_choser);
        mstyle_choser.on_selected_changed.connect(on_style_change_requested);
    }

    int menu_min_height()  {
        return mmenu.min_size_hint().h();
    }

    void enable_container_layout_active_only() {
        mcontainer.set_layout_consider_active_only(true);
    }


    private:
        lgui::HBoxLayout mtop_layout;
        lgui::ButtonGroup mmenu_group;
        lgui::TextLabel mmenu_tl;
        lgui::FlowLayout mmenu_layout;
        lgui::StackedContainer mcontainer;
        lgui::Container mmenu;
        lgui::GUI& mgui;
        lgui::DropDown mstyle_choser;

        RoundedRectTest mrounded_rect_test;
        TextBoxTest mtextbox_test;
        WordWrapTest mwwtest;
        DragDropTest mddtest;


        ScrollTest mscroll_test;
        WindowFrameTest mwindow_frame_test;
        DropDownTest mdrop_down_test;

        TabTest mtab_test;
        ListBoxTest mlistbox_test;
        SliderTest mslider_test;
        PopupTest mpopup_test;
        BoxLayoutTest mbox_layout_test;
        SimpleTableLayoutTest mtable_layout_test;

        TextFieldButtonTest mtextfieldbuttontest;
        NinePatchTest mnp_test;
        RelativeLayoutTest mrelative_test;

        Message mmsgbox;
        std::vector <std::unique_ptr<lgui::RadioButton>> mmenu_buttons;
        std::vector <std::unique_ptr<Popup>> mpopups;

};


ALLEGRO_EVENT_QUEUE *init_event_queue(ALLEGRO_TIMER* timer) {
    ALLEGRO_EVENT_QUEUE *event_queue;

    if(al_install_keyboard() == false)
       lgui::error("Error initialising input", "Couldn't install keyboard!");
#ifdef USE_TOUCH_INPUT
    if(al_install_touch_input() == false)
        lgui::error("Error initialising input", "Couldn't install touch!");
#else
    if(al_install_mouse() == false)
       lgui::error("Error initialising input", "Couldn't install mouse!");
#endif

    event_queue = al_create_event_queue();
    if(event_queue == nullptr)
       lgui::error("Error initialising input", "Couldn't create event queue!");


    al_register_event_source(event_queue, al_get_keyboard_event_source());
#ifdef USE_TOUCH_INPUT
    al_register_event_source(event_queue, al_get_touch_input_event_source());
#else
    al_register_event_source(event_queue, al_get_mouse_event_source());
#endif
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    ALLEGRO_EVENT_SOURCE *display_source = al_get_display_event_source(al_get_current_display());
    al_register_event_source(event_queue, display_source);

    return event_queue;
}

void run_test(lgui::Graphics& gfx, const lgui::Font& def_font, const lgui::Font& small_font)
{
    ALLEGRO_TIMER* timer;
    timer = al_create_timer(1.0);
    if(!timer)
        lgui::error("Error initialising events", "Couldn't install timer!");

    ALLEGRO_EVENT_QUEUE* event_queue = init_event_queue(timer);

    lgui::Color clear_color = lgui::rgb(0.0, 0.0, 0.0);


    lgui::DefaultStyleDarkColorScheme darksch;

    lgui::DefaultStyleBrightColorScheme brightsch;

    lgui::DefaultStyle brightdefstyle(def_font, brightsch);
    lgui::DefaultStyle2ndBorder darkdefstyle(def_font, darksch);
    lgui::Widget::set_default_style(&darkdefstyle);


    lgui::GUI gui; // has to be declared before the others

    AllTests atw(gui, small_font);

    atw.on_style_change_requested.connect([&brightdefstyle, &darkdefstyle/*, &sks*/, &atw](int s) {
        lgui::Style* style = &darkdefstyle;
        if(s == 1)
            style = &brightdefstyle;
        atw.set_style(style);
    });

    atw.set_size(al_get_display_width(al_get_current_display()),
                 al_get_display_height(al_get_current_display()));
    lgui::Size min_size = atw.min_size_hint();
    al_set_window_constraints(al_get_current_display(), min_size.w(), atw.menu_min_height(), 0, 0);
    al_apply_window_constraints(al_get_current_display(), 1);

    atw.enable_container_layout_active_only();

    gui.push_top_widget(atw);

    al_set_timer_speed(timer, 1.0/60.0);
    al_set_timer_count(timer, 0);
    al_start_timer(timer);


    lgui::ExternalEvent event;
    bool quit=false;
    bool redraw = false;


    ALLEGRO_EVENT al_ev, peek;
    while(!quit) {

        al_wait_for_event(event_queue, &al_ev);
        if(al_peek_next_event(event_queue, &peek) &&
                (peek.type == ALLEGRO_EVENT_MOUSE_AXES && peek.mouse.dz == 0))
                continue;

        switch(al_ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                quit = true;
                break;
            case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                // Stop the timer so we don't run at all while our display isn't
                // active.
                al_stop_timer(timer);
                //al_stop_timer(timer);
                //ALLEGRO_DEBUG("after set target");
                redraw = false;
                al_acknowledge_drawing_halt(al_get_current_display());
                break;

            case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                al_acknowledge_drawing_resume(al_get_current_display());

                al_start_timer(timer);
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                atw.set_size(al_ev.display.width, al_ev.display.height);
                al_acknowledge_resize(al_get_current_display());
                redraw = true;
                gui.handle_deferred();
                break;

            case ALLEGRO_EVENT_KEY_CHAR:
            {
                switch(event.key.code) {
                    case ALLEGRO_KEY_ESCAPE:
                    case ALLEGRO_KEY_BACK:
                        quit = true;
                        break;
                    default: break;
                }
            }
            default:
                lgui::convert_a5_event(al_ev, event);
                if (!lgui::convert_a5_event(al_ev, event))
                    continue;
                gui.push_external_event(event);
                if (al_ev.type == ALLEGRO_EVENT_TIMER)
                    redraw = true;
                break;
        }

        if(redraw && al_is_event_queue_empty(event_queue)) {
            gfx.clear(clear_color);
            gui.draw_widgets(gfx);
            al_flip_display();
            redraw = false;
        }
    }

    gui.pop_top_widget();

    al_destroy_timer(timer);

    al_destroy_event_queue(event_queue);

    al_uninstall_keyboard();
    al_uninstall_mouse();
}

int main(int argc, char **argv)
{
    (void) argc; (void) argv;
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    srand(time(0));

#ifdef __ANDROID__
    al_android_set_apk_file_interface();
#endif

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY *display = al_create_display(1024, 576);
    if(!display) {
        lgui::error("Couldn't create display!", "");
    }

    al_set_window_title(display, "lguitest");

    lgui::Graphics graphics;

    lgui::Font def_font("data/forgotteb.ttf", 20);
    lgui::Font small_font("data/forgotteb.ttf", 16);

    run_test(graphics, def_font, small_font);

    return 0;
}
