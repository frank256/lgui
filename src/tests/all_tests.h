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

#ifndef LGUI_ALL_TESTS_H
#define LGUI_ALL_TESTS_H

#include "boxlayouttest.h"
#include "dragdroptest.h"
#include "dropdowntest.h"
#include "listboxtest.h"
#include "ninepatchtest.h"
#include "popuptest.h"
#include "radiotest.h"
#include "relativelayouttest.h"
#include "roundedrecttest.h"
#include "scrolltest.h"
#include "simpletablelayouttest.h"
#include "slidertest.h"
#include "tabtest.h"
#include "textboxtest.h"
#include "textfieldbuttontest.h"
#include "windowframetest.h"
#include "wordwraptest.h"
#include "transformationtest.h"
#include "layoutanimationtest.h"


class AllTestsWidget : public lgui::Container {
    public:

        lgui::Signal<int> on_style_change_requested;

        AllTestsWidget(lgui::GUI& gui, const lgui::Font& small_font);

        void post_layout() override {
            mcontainer.transformation().set_pivot(lgui::PointF(mcontainer.size_rect().center()));
        }

        void setup_callbacks();

        void open_another_popup();

        void add_button(const char* caption, Widget* page);

        void create_menu();

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
        Container mmenu;
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
        TransformationTest mtransformation_test;
        LayoutAnimationTest mlayout_animation_test;

        Message mmsgbox;
        std::vector<std::unique_ptr<lgui::RadioButton>> mmenu_buttons;
        std::vector<std::unique_ptr<Popup>> mpopups;

        lgui::Animation* mcurrent_animation;
};


#endif // LGUI_ALL_TESTS_H
