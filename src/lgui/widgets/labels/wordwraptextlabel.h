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

#ifndef LGUI_WORDWRAPTEXTLABEL_H
#define LGUI_WORDWRAPTEXTLABEL_H

#include "lgui/widget.h"
#include <vector>
#include "lgui/wordwrappedtext.h"

namespace lgui {

    /** A simple widget displaying multiple lines of text.
        You can set a maximum width that will not be exceeded even if the widget is offered more space.
        Note that min_size_hint() can not be implemented properly, so you have to be a bit careful when
        adding this widget to certain layouts. - It is usually a good idea to assign a stretch factor for the
        width in the layout. */
    class WordWrapTextLabel : public Widget
    {
        public:
            WordWrapTextLabel(const std::string& str="");

            WordWrapTextLabel(const std::string& str,
                              const Color& col,
                              const Font* font=nullptr);

            virtual void draw(const DrawEvent& de) const override;


            void set_max_width(int w, bool resize=true);

            // how to implement min_size_hint?

            virtual MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;

            void set_text(const std::string& text);
            void set_color(const Color& col) { mcol = col; }
            virtual void set_font(const Font* font) override;

        protected:
            virtual void style_changed() override;
            virtual void resized(const Size& old_size) override;

        private:
            WordWrappedText mwwtext;
            Color mcol;
            Padding mpadding;
            bool mcustom_color;
    };
}

#endif // LGUI_WORDWRAPTEXTLABEL_H
