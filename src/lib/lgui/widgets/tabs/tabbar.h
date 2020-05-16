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

#ifndef LGUI_TABBAR_H
#define LGUI_TABBAR_H

#include "lgui/widgets/container.h"
#include "tab.h"
#include "lgui/widgets/buttons/helperbutton.h"
#include <vector>
#include <memory>
#include "lgui/signal.h"

namespace lgui {

    /** A container for tabs.
     *  Typically, you'd use TabWidget rather than this one directly.
     *
     *  You can either add tabs you yourself have created or implicitly create and add tabs by just supplying
     *  a string for the caption.
     *
     *  TabBar may show scroll buttons when there is not enough space to display all tabs.
     */
    class TabBar : public PaddedContainer {
        public:
            TabBar();

            /** Emitted when the tab-selection changes. */
            Signal<Tab*> on_tab_selected;

            bool always_show_scroll_buttons() const { return malways_show_scroll_buttons; }
            void set_always_show_scroll_buttons(bool assb) {
                malways_show_scroll_buttons = assb;
                update_scrolling();
            }

            void draw(const DrawEvent& de) const override;

            void add_tab(Tab& tab);

            /** Create a tab with the supplied caption.
             *  @return a reference to the tab created and added */
            Tab& add_tab(const std::string& caption);

            /** Sets the size. Currently, TabBar will not change its height. */
            void set_size(Size s) override;
            using Widget::set_size;

            /** Change the selected tab. */
            void set_selected_tab(const Tab& tab);
            /** Change the selected tab by providing an index.
             * Invalid values lead to no tab being selected. */
            void set_selected_tab(int idx);
            /** Select the next tab. */
            void select_next_tab(bool move_focus=false);
            /** Select the previous tab. */
            void select_previous_tab(bool move_focus=false);
            /** Return the selected tab (may be nullptr). */
            const Tab* selected_tab() const { return mselected_tab; }
            /** Return the index of the selected tab (may be -1 if no tab selected). */
            int selected_tab_idx() const { return mselected_tab_idx; }

            void visit_down(const std::function<void (Widget &)> &f) override;
            Widget* get_child_at(PointF p) override;
            Rect children_area() const override;

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            void resized(const Size& old_size) override;
            void child_about_to_die(Widget& child) override;
            void mouse_wheel_down(MouseEvent& event) override;
            void mouse_wheel_up(MouseEvent& event) override;
            void style_changed() override;
        private:
            void readjust_tabs();
            int find_index_for_tab(const Tab* tab) const;
            void update_scrolling();
            void update_scroll_buttons_active();
            void layout_scroll_buttons();
            void left_pressed();
            void right_pressed();
            void ensure_tab_visible(const Tab& tab);
            int visible_width() const;
            void set_show_scroll_buttons(bool enable);
            int appropriate_height() const;

            HelperButton mbt_left, mbt_right;
            std::vector <std::unique_ptr<Tab>> mmanaged_tabs;
            std::vector <Tab*> mtabs; // convenience
            Tab* mselected_tab;
            int mselected_tab_idx;
            int mscrollx, mtabs_width;
            bool mscroll_shown;
            bool mno_left_line, malways_show_scroll_buttons;
    };

}

#endif // LGUI_TABBAR_H
