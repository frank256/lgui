/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_FRAME_H
#define LGUI_FRAME_H

#include "lgui/basiccontainer.h"
#include <string>

namespace lgui {

    class DrawEvent;

    /** A draggable window frame with a title bar.
        @note You normally don't want to add this to a layout.
        @note You can currently drag the frame outside of its parent so that you will not be able to drag it
              anywhere afterwards. */
    class Frame : public lgui::BasicContainer {
        public:
            Frame();
            explicit Frame(const std::string& title);

            const Padding& padding() const { return mpadding; }
            void set_padding(const Padding& padding);
            void set_title(const std::string& title);
            const std::string& title() const { return mtitle; }

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            void mouse_released(lgui::MouseEvent& event) override;
            void mouse_pressed(lgui::MouseEvent& event) override;
            void mouse_dragged(lgui::MouseEvent& event) override;
            void focus_gained(lgui::FocusEvent& event) override;

            void resized(const Size& old_size) override;
            void style_changed() override;
            void draw_background(const DrawEvent& de) const override;

        private:
            void update_children_area();
            Rect title_rect() const;

            std::string mtitle;
            int mtitle_height;

            lgui::Position mdrag_pos;
            lgui::Padding mpadding, mtitle_padding;
            bool mdragged;
    };

}

#endif // LGUI_FRAME_H
