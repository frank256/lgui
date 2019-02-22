/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#include "tabwidget.h"
#include "lgui/style/style.h"
#include "platform/graphics.h"

namespace lgui {

    TabWidget::TabWidget()
        : mcontent_padding(2)
    {
        add_child(mtab_bar);
        add_child(mcontent);
        mtab_bar.add_widget_listener(this);
        mtab_bar.on_tab_selected.connect([this](Tab* tab) {tab_changed(tab);});
    }

    void TabWidget::add_tab(const std::string& caption, Widget& contents)
    {
        Tab& tab = mtab_bar.add_tab(caption);
        mcontent.add_child(contents);
        mtabs.emplace_back(TabPair(&tab, &contents));
    }

    void TabWidget::add_tab(Tab& tab, Widget& contents)
    {
        mtab_bar.add_tab(tab);
        mcontent.add_child(contents);
        mtabs.emplace_back(TabPair(&tab, &contents));
    }

    MeasureResults TabWidget::measure(SizeConstraint wc, SizeConstraint hc)
    {
        SizeConstraint cwc = wc.adapted_for_child();
        SizeConstraint chc = hc.adapted_for_child();
        MeasureResults bar = mtab_bar.measure(cwc, chc);
        MeasureResults container = mcontent.measure(wc.adapted_for_child(mcontent_padding.horz()),
                                          hc.adapted_for_child(bar.h() + mcontent_padding.vert()));
        Size container_s = mcontent_padding.add(container);
        return force_size_constraints(Size(std::max(container_s.w(), bar.w()), bar.h() + container_s.h()), wc, hc,
                                      TooSmallAccumulator(bar, container));
    }

    Size TabWidget::min_size_hint()
    {
        Size tab_bar = mtab_bar.min_size_hint();
        Size content = mcontent_padding.add(mcontent.min_size_hint());
        return Size(std::max(tab_bar.w(), content.w()), tab_bar.h() + content.h());
    }

    void TabWidget::resized(const Size& old_size)
    {
        BasicContainer::resized(old_size);
        mtab_bar.set_size(width(), mtab_bar.height());
        mtab_bar.set_pos(0, 0);
        adjust_content();
    }

    void TabWidget::size_changed_wl(Widget& w)
    {
        // this is for catching a height increase when adding tabs
        if(&w == &mtab_bar) {
            if(mcontent.pos_y() != mtab_bar.height()+mcontent_padding.top()) {
                adjust_content();
            }
        }
    }

    void TabWidget::draw_background(const DrawEvent& de) const
    {
        Rect bottom_bg_rect(0, mtab_bar.height(), width(), height()-mtab_bar.height());
        StyleArgs args(*this, de, bottom_bg_rect);
        style().draw_tab_contents_bg(de.gfx(), args);
    }

    void TabWidget::tab_changed(Tab* tab)
    {
        // will do nothing if tab is nullptr
        for(auto t : mtabs) {
            if(t.first == tab) {
                mcontent.set_active_widget(t.second);
                return;
            }
        }
    }

    void TabWidget::adjust_content()
    {
        mcontent.set_size(width()-mcontent_padding.horz(),
                          height()-mtab_bar.height()-mcontent_padding.vert());
        mcontent.set_pos(mcontent_padding.left(),
                         mtab_bar.height()+mcontent_padding.top());
    }

}
