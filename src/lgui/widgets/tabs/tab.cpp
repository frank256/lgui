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

#include "tab.h"
#include "lgui/platform/graphics.h"
#include "lgui/platform/keycodes.h"
#include "lgui/mouseevent.h"
#include "lgui/keyevent.h"
#include "tabbar.h"
#include "lgui/style/style.h"

namespace lgui {

    Tab::Tab(const std::string& caption, TabBar* tab_bar)
        : mcaption(caption), mpadding(style().get_tab_padding()), mtab_bar(tab_bar),
          mis_selected(false)
    {
        set_focusable(true);
        set_may_tab_into(true);
        set_may_tab_out_of(true);
    }

    void Tab::draw(const DrawEvent& de) const
    {
        StyleArgs args(*this, de, false, false, mis_selected);

        style().draw_tab(de.gfx(), args, mpadding, mcaption);
    }

    void Tab::set_caption(const std::string& caption)
    {
        mcaption = caption;
        request_layout();
    }

    void Tab::set_tab_bar(TabBar* tab_bar)
    {
        mtab_bar = tab_bar;
    }

    void Tab::set_selected(bool sel) {
        mis_selected = sel;
    }

    MeasureResults Tab::measure(SizeConstraint wc, SizeConstraint hc)
    {
        return force_size_constraints(min_size_hint(), wc, hc);
    }

    Size Tab::min_size_hint()
    {
        return mpadding.add(Size(font().text_width(mcaption), font().line_height()));
    }

    void Tab::mouse_clicked(MouseEvent& event)
    {
        if(mtab_bar) {
            mtab_bar->set_selected_tab(*this);
            //focus();
            event.consume();
        }
    }

    void Tab::key_pressed(KeyEvent& event)
    {

        if(event.key_code() == Keycodes::KEY_LEFT) {
            if(mtab_bar) {
                mtab_bar->select_previous_tab(true);
                event.consume();
            }
        }
        else if(event.key_code() == Keycodes::KEY_RIGHT) {
            if(mtab_bar) {
                mtab_bar->select_next_tab(true);
                event.consume();
            }
        }
        else if(event.key_code() == Keycodes::KEY_ENTER ||
                event.key_code() == Keycodes::KEY_SPACE) {
            if(mtab_bar && mtab_bar->selected_tab() != this) {
                mtab_bar->set_selected_tab(*this);
                event.consume();
            }
        }
    }

    void Tab::style_changed()
    {
        mpadding = style().get_tab_padding();
        request_layout();
    }

}
