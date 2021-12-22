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

#include "all_tests.h"

#include "lgui/platform/stringfmt.h"

#include "lgui/animation/valueanimation.h"
#include "lgui/animation/animationbuilder.h"
#include "lgui/gui.h"

AllTestsWidget::AllTestsWidget(lgui::GUI& gui, const lgui::Font& small_font)
        : mgui(gui), mddtest(small_font), mmsgbox("", "Ok"), mcurrent_animation(nullptr) {
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
    mcontainer.add_child(mtransformation_test);
    mcontainer.add_child(mlayout_animation_test);

    create_menu();

    mtop_layout.add_item(mmenu);
    mtop_layout.add_item(mcontainer, 1);
    set_layout(&mtop_layout);

    mcontainer.set_active_widget(&mrounded_rect_test);
    mmenu_buttons[0]->set_checked(true);

}
void AllTestsWidget::setup_callbacks() {
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

void AllTestsWidget::create_menu() {
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
    add_button("Transformation test", &mtransformation_test);
    add_button("Layout animation test", &mlayout_animation_test);

    mstyle_choser.model().add_item("Dark style");
    mstyle_choser.model().add_item("Bright style");
    mstyle_choser.set_selected(0);
    mmenu_layout.add_item(mstyle_choser);
    mstyle_choser.on_selected_changed.connect(on_style_change_requested);

    mmenu.set_padding(lgui::Padding(8, 4, 8, 4));
    mmenu.set_layout(&mmenu_layout);
}

void AllTestsWidget::add_button(const char* caption, lgui::Widget* page) {
    lgui::RadioButton* bt = new lgui::RadioButton(caption);
    mmenu_group.add_button(bt);
    bt->on_activated.connect([this, page]() {
        if (mcurrent_animation) {
            mcurrent_animation->end();
            mcurrent_animation = nullptr;
        }
        auto& seq = animate().sequence(
                lgui::animate_transform(mcontainer.transformation())
                        .rotate_y(0, 90)
                        .rotate_x(0, 90)
                        .with_duration(0.3)
                        .then_call([this, page]() { mcontainer.set_active_widget(page); }),
                lgui::animate_transform(mcontainer.transformation())
                        .rotate_y(-90, 0)
                        .rotate_x(-90, 0)
                        .with_duration(0.3)
                        .then_call([this]() { mcurrent_animation = nullptr; })
        ).build();
        mcurrent_animation = &seq;
        mcurrent_animation->start();
    });
    mmenu_buttons.push_back(std::unique_ptr<lgui::RadioButton>(bt));
    mmenu_layout.add_item(*bt);
}

void AllTestsWidget::open_another_popup() {
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
