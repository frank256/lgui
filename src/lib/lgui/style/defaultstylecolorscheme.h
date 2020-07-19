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


#ifndef LGUI_DEFAULTSTYLECOLORSCHEME_H
#define LGUI_DEFAULTSTYLECOLORSCHEME_H

#include "lgui/platform/color.h"
#include <vector>

#include "styleargs.h"

namespace lgui {

/** Color scheme to use with DefaultStyle. */
class DefaultStyleColorScheme {
    public:
        enum class DefaultStyleElement {
                Background,
                WidgetFillBg,
                Border1,   // border1 means outer (or single) border
                Border2,   // border2 means inner border
                DispText,
                ButtonText,
                EditText,
                EditTextSelection,
                EditTextCursor,
                TabSelBg,
                TabNotSelBg,
                TabContentBg,
                TabBorder1,
                TabBorder2,
                TabSelFocusedBorder,
                Check,
                CheckBoxBorder,
                CheckBoxBg,
                RadioFill,
                RadioBg,
                RadioBorder,
                CheckRadioTextEmphasisBgA,  // gradient top
                CheckRadioTextEmphasisBgB,  // gradient bottom
                CheckRadioTextEmphasisBorder,
                ScrollBarBodyBg,
                ScrollBarBodyBorder,
                ScrollBarHandleBorder,
                ScrollBarHandleBgA,  // gradient top/left
                ScrollBarHandleBgB, // gradient bottom/right
                SliderBodyBg,
                SliderBodyBorder,
                SliderBodyFillBg,
                SliderBodyFillBorder,
                SliderHandle,
                SliderHandleBorder,
                ButtonBgA,     // gradient top
                ButtonBgB,     // gradient bottom
                ButtonBorder1, // outer border
                ButtonBorder2, // inner border
                FrameBorder1,  // outer border
                FrameBorder2,  // inner border
                FrameTitleBgA, // gradient top
                FrameTitleBgB, // gradient bottom
                FrameTitleText,
                FrameBg,
                NoElements
        };

        DefaultStyleColorScheme();

        inline Color color(DefaultStyleElement c, WidgetState state,
                           float opacity) const {
            return color(c, state.state, opacity);
        }

        Color color(DefaultStyleElement c, int state_flags,
                    float opacity) const;
        // without multiplying alpha
        Color get(DefaultStyleElement dse, int state_flags) const;

        void reset();

        void set(DefaultStyleElement dse, int state_flags, Color c);
        void copy(DefaultStyleElement dest, DefaultStyleElement src);

        void all_set();
        void invert();

    protected:


    private:
        std::vector<Color> mcolors;

};

/** Dark color scheme for default style. This is intended to be used with
 *  DefaultStyle2ndBorder. */
class DefaultStyleDarkColorScheme : public DefaultStyleColorScheme {
    public:
        DefaultStyleDarkColorScheme();
};

/** Bright color scheme for default style. This is intended to be used with
 *  DefaultStyle. */
class DefaultStyleBrightColorScheme : public DefaultStyleColorScheme {
    public:
        DefaultStyleBrightColorScheme();
};
}

#endif // LGUI_DEFAULTSTYLECOLORSCHEME_H
