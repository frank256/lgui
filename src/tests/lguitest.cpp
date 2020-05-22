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

// This test program expects the following files to be accessible from its working directory:
// data/forgotteb.ttf; data/test_9.tga

#include "lgui/platform/error.h"
#include "lgui/platform/graphics.h"
#include "lgui/platform/font.h"
#include "lgui/platform/stringfmt.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "lgui/gui.h"

#include "lgui/style/defaultstyle.h"
#include "lgui/style/defaultstyle2ndborder.h"

#include "all_tests.h"
#include <cmath>

#ifdef __ANDROID__
#define USE_TOUCH_INPUT
#include <allegro5/allegro_android.h>
#endif

static ALLEGRO_EVENT_QUEUE* init_event_queue();
void setup_perspective_transform();

class AllTestsWidget : public lgui::Container {
    public:

        lgui::Signal<int> on_style_change_requested;

        AllTestsWidget(lgui::GUI& gui, const lgui::Font& small_font)
                : mgui(gui), mddtest(small_font), mmsgbox("", "Ok") {
            set_name("AllTests");
            mddtest.reset();
            mscroll_test.reset();

            setup_callbacks();

            mmenu_layout.set_max_on_row(1);
            mmenu_layout.set_vertical_spacing(4);

            mcontainer.set_name("MainContainer");

            mcontainer.add_child(mrounded_rect_test);
            mcontainer.add_child(mwwtest);
            mcontainer.add_child(mtextbox_test);

            mcontainer.add_child(mddtest);
            mcontainer.add_child(mscroll_test);
            mcontainer.add_child(mlistbox_test);
            mcontainer.add_child(mdrop_down_test);
            mcontainer.add_child(mtab_test);
            mcontainer.add_child(mtextfieldbuttontest);
            mcontainer.add_child(mslider_test);
            mcontainer.add_child(mpopup_test);
            mcontainer.add_child(mbox_layout_test);
            mcontainer.add_child(mtable_layout_test);
            mcontainer.add_child(mwindow_frame_test);
            mcontainer.add_child(mnp_test);
            mcontainer.add_child(mrelative_test);


            create_menu();

            mtop_layout.add_item(mmenu);
            mtop_layout.add_item(mcontainer, 1);
            set_layout(&mtop_layout);

            mcontainer.set_active_widget(&mrounded_rect_test);
            mmenu_buttons[0]->set_checked(true);
        }

        void setup_callbacks() {
            std::function<void(lgui::AbstractButton* src)> show_button_message =
                    [this](lgui::AbstractButton* src) {
                        auto bbutton = static_cast <BlockButton*> (src);
                        if (bbutton) {
                            mmsgbox.set_msg(bbutton->str());
                            mmsgbox.set_min_size();
                            mmsgbox.set_pos(512 - mmsgbox.width() / 2, 350 - mmsgbox.height() / 2);
                            mgui.push_top_widget(mmsgbox, true, lgui::grey_premult(0.2, 0.5));
                        }
                    };
            mddtest.show_message.connect(show_button_message);

            std::function<void(int, const std::string&)> show_list_box_message =
                    [this](int idx, const std::string& entry) {
                        mmsgbox.set_msg(lgui::StringFmt("Activated entry #%1: %2").arg(idx).arg(entry.c_str()));
                        mmsgbox.set_min_size();
                        mmsgbox.set_pos(512 - mmsgbox.width() / 2, 350 - mmsgbox.height() / 2);
                        mgui.push_top_widget(mmsgbox, true, lgui::grey_premult(0.2, 0.5));
                    };
            mlistbox_test.on_entry_activated.connect(show_list_box_message);
            mpopup_test.on_open_popup.connect([this]() { open_another_popup(); });
        }

        void open_another_popup() {
            auto* p = new Popup();
            p->set_style(&style());
            p->on_open_another.connect([this]() { open_another_popup(); });
            mpopups.push_back(std::unique_ptr<Popup>(p));

            p->set_min_size();

            float x = float(rand()) / RAND_MAX;
            float y = float(rand()) / RAND_MAX;
            x *= width() - mmenu.width() - p->width() - 40;
            y *= height() - p->height() - 40;
            x += mmenu.width() + 20;
            y += 20;
            p->set_pos(x, y);

            mgui.push_top_widget(*p, true, lgui::grey_premult(0.3, 0.8));
        }

        void add_button(const char* caption, Widget* page) {
            lgui::RadioButton* bt = new lgui::RadioButton(caption);
            mmenu_group.add_button(bt);
            bt->on_activated.connect([this, page]() { mcontainer.set_active_widget(page); });
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

            mmenu.set_padding(lgui::Padding(8, 4, 8, 4));
            mmenu.set_layout(&mmenu_layout);
        }

        int menu_min_height() {
            return mmenu.min_size_hint().h();
        }

        void enable_container_layout_active_only() {
            mcontainer.set_layout_consider_active_only(true);
        }

        void style_changed() override {
            Container::style_changed();
            mmsgbox.set_style(&style());
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
        std::vector<std::unique_ptr<lgui::RadioButton>> mmenu_buttons;
        std::vector<std::unique_ptr<Popup>> mpopups;

};


class LguiTest {
    public:
        LguiTest(lgui::Graphics& gfx, const lgui::Font& small_font, ALLEGRO_EVENT_QUEUE* event_queue,
                 lgui::DefaultStyle& bright_default_style, lgui::DefaultStyle& dark_default_style)
                : mevent_queue(event_queue), mgfx(gfx),
                  mbright_default_style(bright_default_style),
                  mdark_default_style(dark_default_style),
                  mall_tests_widget(mgui, small_font) {
            setup_timer();
            setup_callbacks();
            setup_window_size();
        }

        ~LguiTest() {
            al_destroy_timer(mtimer);
        }

        LguiTest(const LguiTest& other) = delete;
        LguiTest(LguiTest&& other) = delete;
        LguiTest& operator=(const LguiTest& other) = delete;
        LguiTest& operator=(LguiTest&& other) = delete;

        void main_loop() {
            lgui::Color clear_color = lgui::rgb(0.0, 0.0, 0.0);

            mgui.push_top_widget(mall_tests_widget);
            al_start_timer(mtimer);

            lgui::ExternalEvent event;
            ALLEGRO_EVENT al_ev, peek;

            bool quit = false, redraw = false;

            while (!quit) {
                al_wait_for_event(mevent_queue, &al_ev);
                if (al_peek_next_event(mevent_queue, &peek) &&
                    (peek.type == ALLEGRO_EVENT_MOUSE_AXES && peek.mouse.dz == 0))
                    continue;

                switch (al_ev.type) {
                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        quit = true;
                        break;
                    case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                        // Stop the timer so we don't run at all while our display isn't
                        // active.
                        al_stop_timer(mtimer);
                        redraw = false;
                        al_acknowledge_drawing_halt(al_get_current_display());
                        break;

                    case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                        al_acknowledge_drawing_resume(al_get_current_display());

                        al_start_timer(mtimer);
                        break;
                    case ALLEGRO_EVENT_DISPLAY_RESIZE:
                        mall_tests_widget.set_size(al_ev.display.width, al_ev.display.height);
                        al_acknowledge_resize(al_get_current_display());
                        setup_perspective_transform();
                        redraw = true;
                        mgui.handle_deferred();
                        break;

                    case ALLEGRO_EVENT_KEY_CHAR: {
                        switch (event.key.code) {
                            case ALLEGRO_KEY_ESCAPE:
                            case ALLEGRO_KEY_BACK:
                                quit = true;
                                break;
                            default:
                                break;
                        }
                    }
                    default:
                        if (!lgui::convert_a5_event(al_ev, event))
                            continue;
                        mgui.push_external_event(event);
                        if (al_ev.type == ALLEGRO_EVENT_TIMER) {
                            redraw = true;
                        }
                        break;
                }

                if (redraw && al_is_event_queue_empty(mevent_queue)) {
                    mgfx.clear(clear_color);
                    mgui.draw_widgets(mgfx);
                    mgfx.flip();
                    redraw = false;
                }
            }
            mgui.pop_top_widget();
        }


    private:
        void setup_callbacks() {
            mall_tests_widget.on_style_change_requested.connect([this](int s) {
                lgui::Style* style = &mdark_default_style;
                if (s == 1)
                    style = &mbright_default_style;
                mall_tests_widget.set_style(style);
            });
        }

        void setup_window_size() {
            mall_tests_widget.enable_container_layout_active_only();
            mall_tests_widget.set_size(al_get_display_width(al_get_current_display()),
                                       al_get_display_height(al_get_current_display()));
            lgui::Size min_size = mall_tests_widget.min_size_hint();
            al_set_window_constraints(al_get_current_display(), min_size.w(),
                                      mall_tests_widget.menu_min_height(), 0, 0);
            al_apply_window_constraints(al_get_current_display(), true);
        }

        void setup_timer() {
            mtimer = al_create_timer(1.0 / 60.0);
            if (!mtimer)
                lgui::error("Error initialising events", "Couldn't install timer!");
            al_register_event_source(mevent_queue, al_get_timer_event_source(mtimer));
        }

        ALLEGRO_EVENT_QUEUE* mevent_queue;
        lgui::Graphics& mgfx;
        lgui::DefaultStyle& mbright_default_style;
        lgui::DefaultStyle& mdark_default_style;
        ALLEGRO_TIMER* mtimer;
        lgui::GUI mgui;
        AllTestsWidget mall_tests_widget;
};

void run_test(lgui::Graphics& gfx, const lgui::Font& def_font, const lgui::Font& small_font) {
    ALLEGRO_EVENT_QUEUE* event_queue = init_event_queue();

    lgui::DefaultStyleDarkColorScheme dark_scheme;
    lgui::DefaultStyleBrightColorScheme bright_scheme;
    lgui::DefaultStyle bright_default_style(def_font, bright_scheme);
    lgui::DefaultStyle2ndBorder dark_default_style(def_font, dark_scheme);

    lgui::Widget::set_default_style(&dark_default_style);

    LguiTest lgui_test(gfx, small_font, event_queue, bright_default_style, dark_default_style);

    lgui_test.main_loop();

    al_destroy_event_queue(event_queue);

    al_uninstall_keyboard();
    al_uninstall_mouse();
}

static ALLEGRO_EVENT_QUEUE* init_event_queue() {
    ALLEGRO_EVENT_QUEUE* event_queue;

    if (!al_install_keyboard())
        lgui::error("Error initialising input", "Couldn't install keyboard!");
#ifdef USE_TOUCH_INPUT
    if(!al_install_touch_input())
        lgui::error("Error initialising input", "Couldn't install touch!");
#else
    if (!al_install_mouse())
        lgui::error("Error initialising input", "Couldn't install mouse!");
#endif

    event_queue = al_create_event_queue();
    if (event_queue == nullptr)
        lgui::error("Error initialising input", "Couldn't create event queue!");


    al_register_event_source(event_queue, al_get_keyboard_event_source());
#ifdef USE_TOUCH_INPUT
    al_register_event_source(event_queue, al_get_touch_input_event_source());
#else
    al_register_event_source(event_queue, al_get_mouse_event_source());
#endif

    ALLEGRO_EVENT_SOURCE* display_source = al_get_display_event_source(al_get_current_display());
    al_register_event_source(event_queue, display_source);

    return event_queue;
}

void setup_perspective_transform() {

    // The goal here is to setup a projection transform that behaves as if normal 2D pixels were drawn for z=0
    // (i.e. as with the normal orthographic transform set up by Allegro), but allows some perspective effects for
    // rotating widgets around the X and Y axes.

    float w = al_get_display_width(al_get_current_display()), h = al_get_display_height(al_get_current_display());
    float fov_angle = 90.0;
    float fov = tan(fov_angle * ALLEGRO_PI / 180.0 / 2.0);

    // That is the z near plane where we need to draw everything before the perspective transform so that it ends up
    // as 1:1 at pixel-coordingates.
    float z_near = w / 2 * fov;

    // If we really drew at depth of z_near, everything closer to the camera would be z-clipped.
    // This would be a problem for rotations around the x and y axis.
    // Therefore, to avoid z-clipping, we need to move everything further away from the camera before the perspective
    // transform is applied.
    // This is achieved by an additional view matrix which is composed with the perspective transform so that the view
    // transformation happens first.
    const float Z_DIST_FACTOR = 8.0;

    ALLEGRO_TRANSFORM perspective;
    al_identity_transform(&perspective);
    // FIXME: Does factor 2 for "far" make sense?
    al_perspective_transform(&perspective, -w / 2, -h / 2, z_near, w / 2, h / 2, z_near * Z_DIST_FACTOR * 2);

    ALLEGRO_TRANSFORM view;
    al_identity_transform(&view);
    // We make up for the perspective correction due to z-translation by scaling everything.
    al_scale_transform(&view, Z_DIST_FACTOR, Z_DIST_FACTOR);
    // Move away from the camera (and center).
    al_translate_transform_3d(&view, -w / 2 * Z_DIST_FACTOR, -h / 2 * Z_DIST_FACTOR, -z_near * Z_DIST_FACTOR);

    al_compose_transform(&view, &perspective);
    al_use_projection_transform(&view);
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    srand(time(nullptr));

#ifdef __ANDROID__
    al_android_set_apk_file_interface();
#endif
//      // Debug: log all events!
//    lgui::DebugEventFilter debug_event_filter;
//    lgui::Widget::set_default_event_filter(&debug_event_filter);

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(1024, 576);
    if (!display) {
        lgui::error("Couldn't create display!", "");
    }

    al_set_window_title(display, "lguitest");

    setup_perspective_transform();

    lgui::Graphics graphics;

    lgui::Font def_font("data/forgotteb.ttf", 20);
    lgui::Font small_font("data/forgotteb.ttf", 16);

    run_test(graphics, def_font, small_font);

    al_uninstall_keyboard();
    al_uninstall_mouse();


    return 0;
}
