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

#ifndef LGUI_TABWIDGET_H
#define LGUI_TABWIDGET_H

#include "tabbar.h"
#include "../stackedcontainer.h"
#include "lgui/iwidgetlistener.h"
#include <vector>

namespace lgui {

    /** A container for several tabs and the accompanying tab bar.
     *  Add (tab, contents) or (name, contents) pairs.
     */
    class TabWidget : public lgui::BasicContainer, public IWidgetListener
    {
        public:
            TabWidget();

            void add_tab(const std::string& caption, Widget& contents);
            void add_tab(Tab& tab, Widget& contents);


            bool always_show_scroll_buttons() const {
                return mtab_bar.always_show_scroll_buttons();
            }

            void set_always_show_scroll_buttons(bool assb) {
                mtab_bar.set_always_show_scroll_buttons(assb);
            }

            void select_next_tab() { mtab_bar.select_next_tab(); }
            void select_previous_tab() { mtab_bar.select_previous_tab(); }
            void select_tab(const Tab& tab) { mtab_bar.set_selected_tab(tab); }
            void select_tab(int idx) { mtab_bar.set_selected_tab(idx); }
            int current_tab_idx() const { return mtab_bar.selected_tab_idx(); }
            const Tab* current_tab() const { return mtab_bar.selected_tab(); }
            const Widget* current_tab_contents() const { return mcontent.active_widget(); }
            Widget* current_tab_contents() { return mcontent.active_widget(); }

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            void resized(const Size& old_size) override;
            void size_changed_wl(Widget& w) override;
            void draw_background(const DrawEvent& de) const override;

            void tab_changed(Tab* tab);

        private:
            void adjust_content();

            TabBar mtab_bar;
            StackedContainer mcontent;
            Padding mcontent_padding;
            using TabPair = std::pair <Tab*, Widget*>;
            std::vector<TabPair> mtabs;
    };

}

#endif // LGUI_TABWIDGET_H
