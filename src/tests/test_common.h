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

#ifndef LGUI_TEST_COMMON_H
#define LGUI_TEST_COMMON_H

#include "lgui/widgets/container.h"
#include "lgui/widgets/slider.h"
#include "lgui/dragrepresentation.h"
#include "lgui/widgets/abstractbutton.h"
#include "lgui/widgets/labels/textlabel.h"
#include "lgui/widgets/pushbutton.h"
#include "lgui/layout/vboxlayout.h"

extern const char* mstatic_text;

class TestContainer : public lgui::Container {
    public:
        TestContainer()
        {
            set_padding(lgui::Padding(15, 15, 15, 15));
        }
};


class WidthSlider : public lgui::HorizontalSlider {
    protected:
        void resized(const lgui::Size& old_size) override;
};


class HeightSlider : public lgui::VerticalSlider {
    protected:
        void resized(const lgui::Size& old_size) override;
};


class BlockButtonDragRepr : public lgui::DragRepresentation {
    public:
        BlockButtonDragRepr(lgui::Widget& src_widget, const lgui::Position& hotspot,
                            const std::string& content_descr, const lgui::Size& size);

        virtual void draw(lgui::Graphics& gfx) const;
};

class BlockContainer;

class BlockButton : public lgui::AbstractButton {
    public:
        BlockButton(const std::string& str, int dim);

        virtual lgui::MeasureResults measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc) override;

        virtual void draw(const lgui::DrawEvent& de) const override;

        virtual void mouse_pressed(lgui::MouseEvent& event) override;

        virtual void mouse_dragged(lgui::MouseEvent& event) override;

        virtual void drag_ended(lgui::DragDropEvent& event) override;

        virtual void resized(const lgui::Size& old_size) override;

        BlockContainer* home() { return mhome; }
        void set_home(BlockContainer* home) { mhome = home; }
        const std::string& str() const { return mstr; }

    private:
        int mwant_w;
        lgui::Position mpressed_pos;
        double mpressed_time;
        std::string  mstr;
        BlockContainer* mhome;
        lgui::Position moffs;
        bool mdisable;
};


class Message : public lgui::Container
{
    public:
        Message(const std::string& msg, const std::string& bttext);

        void set_msg(const std::string& msg);

        virtual void focus_gained(lgui::FocusEvent& event) override;

    private:
        lgui::TextLabel mlabel;
        lgui::PushButton mbutton;
        lgui::VBoxLayout mlayout;
};


#endif // LGUI_TEST_COMMON_H
