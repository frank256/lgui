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

#ifndef LGUI_TEXTLABEL_H
#define LGUI_TEXTLABEL_H

#include "lgui/widget.h"
#include <string>
#include "lgui/platform/font.h"
#include "lgui/platform/color.h"

namespace lgui {

/** A simple widget displaying one line of text. */
class TextLabel : public Widget {
    public:
        explicit TextLabel(const std::string& str = "");

        TextLabel(const std::string& str,
                  const Color& col,
                  const Font* font = nullptr);

        void draw(const DrawEvent& de) const override;

        void set_text(const std::string& str);
        void set_color(const Color& col) {
            mcol = col;
            mcustom_color = true;
        }
        void set_align(Align align) { malign = align; }

        const std::string& text() const {
            return mtext;
        }

        Size min_size_hint() override;
        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;

    private:
        static const int MARGIN_X = 0, MARGIN_Y = 0;

        Color mcol;
        std::string mtext;
        Align malign;
        bool mcustom_color;
};

}
#endif // LGUI_TEXTLABEL_H
