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

#ifndef LGUI_DRAGDROPTEST_H
#define LGUI_DRAGDROPTEST_H

#include "test_common.h"
#include "lgui/layout/flowlayout.h"

#include <memory>
#include <array>


class BlockContainer : public lgui::PaddedContainer, public lgui::IWidgetListener {
    public:
        BlockContainer(const std::string& pattern);

        virtual void draw(const lgui::DrawEvent& de) const override;

        void drag_entered(lgui::DragDropEvent& event) override;


        void drag_left(lgui::DragDropEvent& event) override;

        void dropped(lgui::DragDropEvent& event) override;

        virtual void child_added_wl(Widget& w, Widget& child) override;
        virtual void child_removed_wl(Widget& w, Widget& child) override;

        /*void dragEnded(lgui::DragDropEvent& event) override
        {

        }*/

        virtual lgui::MeasureResults measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) override;

        // Because the new layout system would give us our size based on the size of our children,
        // which we don't want here. We'd need Fixed/StaticLayout for that.
        void set_static_size(const lgui::Size& s);

        void insert(BlockButton& bt);

        void style_changed() override;

    private:
        lgui::Size mmy_size;
        std::string mpattern;
        lgui::FlowLayout mlayout;
        bool mdrag_active;
        lgui::Color mline_color;
};

class DragDropTest : public lgui::BasicContainer {
    public:

        lgui::Signal <lgui::AbstractButton*> show_message;

        DragDropTest(const lgui::Font& font);

        void reset();

    private:
        std::array <std::unique_ptr<BlockButton>, 6> mbb;
        BlockContainer mbc1, mbc2, mbc3;
        lgui::VBoxLayout mlayout;
        lgui::PushButton mreset_button;
        static const int NBUTTONS = 6;
        static const char* BUTTON_STRINGS[6];
};


#endif // LGUI_DRAGDROPTEST_H
