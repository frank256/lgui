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

#include "defaultstylecolorscheme.h"
#include "platform/error.h"

namespace lgui {

    DefaultStyleColorScheme::DefaultStyleColorScheme()
    {
        reset();
    }


    Color DefaultStyleColorScheme::color(DefaultStyleElement dse, int state_flags, float opacity) const
    {
        int offs = state_flags;
        if(offs & WidgetState::Inactive)
            offs = 8;
        else
            offs &= 7;

        const Color& col = mcolors[9*int(dse)+offs];
        return rgba_premult(col.r, col.g, col.b, opacity);
    }

    Color DefaultStyleColorScheme::get(DefaultStyleColorScheme::DefaultStyleElement dse, int state_flags) const
    {
        int offs = state_flags;
        if(offs & WidgetState::Inactive)
            offs = 8;
        else
            offs &= 7;

        return mcolors[9*int(dse)+offs];
    }

    void DefaultStyleColorScheme::reset()
    {
        // we suppose you don't want magic pink with no alpha, do you? :)
        mcolors.resize(9*int(DefaultStyleElement::NoElements), rgba(1.0, 0.0, 1.0, 0.0));
    }

    void DefaultStyleColorScheme::set(DefaultStyleElement dse, int state_flags, Color col)
    {
        int offs = state_flags;
        if(offs & WidgetState::Inactive)
            offs = 8;
        else
            offs &= 7;
        mcolors[9*int(dse)+offs] = col;
    }

    void DefaultStyleColorScheme::copy(DefaultStyleColorScheme::DefaultStyleElement dest,
                                       DefaultStyleColorScheme::DefaultStyleElement src)
    {
        for(int i = 0; i < 9; i++)
            mcolors[9*int(dest)+i] = mcolors[9*int(src)+i];
    }


    static bool is_not_set(const Color& col) {
        unsigned char r, g, b, a;
        col_get_rgba_i(col, r, g, b, a);
        return r == 255 && g == 0 && b == 255 && a == 0;
    }

    void DefaultStyleColorScheme::all_set()
    {
        // set the rest here, so it's just a matter of looking up afterwards
        for(int i = int(DefaultStyleElement::Background);
            i < int(DefaultStyleElement::NoElements); i++) {
            // cascade down here
            DefaultStyleElement dfe = DefaultStyleElement(i);

            // take hovered from focussed hovered or std
            if(is_not_set(get(dfe, WidgetState::Hovered))) {
                if(!is_not_set(get(dfe, WidgetState::Focused | WidgetState::Hovered)))
                    set(dfe, WidgetState::Hovered,
                        get(dfe, WidgetState::Focused | WidgetState::Hovered));
                else
                    set(dfe, WidgetState::Hovered, get(dfe, 0));
            }

            // take down from focussed down or std (fixme: hover?)
            if(is_not_set(get(dfe, WidgetState::Down))) {
                if(!is_not_set(get(dfe, WidgetState::Focused | WidgetState::Down)))
                    set(dfe, WidgetState::Down,
                        get(dfe, WidgetState::Focused | WidgetState::Down));
                else
                    set(dfe, WidgetState::Down, get(dfe, 0));
            }

            // take focused-down from focused if available or from down
            if(is_not_set(get(dfe, WidgetState::Focused | WidgetState::Down))) {
                if(!is_not_set(get(dfe, WidgetState::Focused)))
                    set(dfe, WidgetState::Focused | WidgetState::Down,
                        get(dfe, WidgetState::Focused));
                else
                    set(dfe, WidgetState::Focused | WidgetState::Down,
                        get(dfe, WidgetState::Down));
            }

            // take focused-hovered from focused if available or from hovered
            if(is_not_set(get(dfe, WidgetState::Focused | WidgetState::Hovered))) {
                if(!is_not_set(get(dfe, WidgetState::Focused)))
                    set(dfe, WidgetState::Focused | WidgetState::Hovered,
                        get(dfe, WidgetState::Focused));
                else
                    set(dfe, WidgetState::Focused | WidgetState::Hovered,
                        get(dfe, WidgetState::Hovered));
            }

            // take focused from std
            if(is_not_set(get(dfe, WidgetState::Focused)))
                set(dfe, WidgetState::Focused, get(dfe,0));


            // take hovered-down from down
            if(is_not_set(get(dfe, WidgetState::Hovered | WidgetState::Down)))
                set(dfe, WidgetState::Hovered | WidgetState::Down,
                    get(dfe, WidgetState::Down));

            // take focused-hovered-down from focused-down
            if(is_not_set(get(dfe, WidgetState::Focused | WidgetState::Hovered | WidgetState::Down)))
                set(dfe, WidgetState::Focused | WidgetState::Hovered | WidgetState::Down,
                    get(dfe, WidgetState::Focused | WidgetState::Down));

            // take inactive from std
            if(is_not_set(get(dfe, WidgetState::Inactive)))
                set(dfe, WidgetState::Inactive, get(dfe, 0));
        }
    }


    void DefaultStyleColorScheme::invert()
    {
        for(int i = int(DefaultStyleElement::Background);
            i < int(DefaultStyleElement::NoElements); i++) {
            for(int j = 0; j < 9; j++) {
                mcolors[9*i+j] = invert_color(mcolors[9*i+j]);
            }

        }
    }

    using DSE = DefaultStyleColorScheme::DefaultStyleElement;
    using WS = WidgetState;


    static void prepare_default_dark_scheme(DefaultStyleColorScheme& s)
    {
        s.reset();
        // text colors
        s.set(DSE::DispText, 0, grey(0.92));
        s.set(DSE::DispText, WS::Inactive, grey(0.75));
        s.set(DSE::ButtonText, 0, grey(0.92));
        s.set(DSE::ButtonText, WS::Inactive, grey(0.72));
        s.set(DSE::EditText, 0, grey(1.0));
        s.set(DSE::EditText, WS::Inactive, grey(.75));
        s.set(DSE::EditTextSelection, 0, grey(.4));
        s.set(DSE::EditTextSelection, WS::Focused, grey(.55));
        s.set(DSE::EditTextCursor, 0, grey_premult(0.9, 0.8));


        s.set(DSE::Background, 0, grey(0.18));
        s.set(DSE::Background, WS::Inactive, grey(0.27));

        s.set(DSE::WidgetFillBg, 0, grey(0.21));
        s.set(DSE::WidgetFillBg, WS::Inactive, grey(0.27));

        s.set(DSE::Border1, 0, grey(0.15));
        s.set(DSE::Border1, WS::Inactive, grey(0.23));
        s.set(DSE::Border1, WS::Focused, grey(0.45));
        s.set(DSE::Border2, 0, grey(0.3));
        s.set(DSE::Border2, WS::Inactive, grey(0.3));

        s.set(DSE::Check, 0, rgb(1.0, 207/255.0, 41.0/255.0));
        s.set(DSE::CheckBoxBg, 0, grey(0.20));
        s.set(DSE::CheckBoxBg, WS::Inactive, grey(0.27));
        s.set(DSE::CheckBoxBg, WS::Down, grey(0.3));
        s.set(DSE::CheckBoxBorder, 0, grey(0.4));
        s.set(DSE::CheckBoxBorder, WS::Focused, grey(0.6));
        s.set(DSE::CheckBoxBorder, WS::Inactive, grey(0.27));
        s.set(DSE::CheckBoxBorder, WS::Down, grey(0.3));
        s.set(DSE::CheckBoxBorder, WS::Focused | WS::Down, grey(0.3));

        s.copy(DSE::RadioFill, DSE::Check);
        s.copy(DSE::RadioBorder, DSE::CheckBoxBorder);
        s.copy(DSE::RadioBg, DSE::CheckBoxBg);

        s.set(DSE::CheckRadioTextEmphasisBgA, 0, grey(0.26));
        s.set(DSE::CheckRadioTextEmphasisBgB, 0, grey(0.26 * 0.75));
        s.set(DSE::CheckRadioTextEmphasisBorder, 0, grey(0.5));

        s.set(DSE::ScrollBarHandleBgA, 0, grey(0.18));
        s.set(DSE::ScrollBarHandleBgB, 0, grey(0.18 * 0.75));
        s.set(DSE::ScrollBarHandleBgA, WS::Inactive, grey(0.27));
        s.set(DSE::ScrollBarHandleBgB, WS::Inactive, grey(0.27 * 0.75));
        s.set(DSE::ScrollBarHandleBgA, WS::Down, grey(0.16));
        s.set(DSE::ScrollBarHandleBgB, WS::Down, grey(0.16 * 0.9));
        s.set(DSE::ScrollBarHandleBgA, WS::Hovered, grey(0.2));
        s.set(DSE::ScrollBarHandleBgB, WS::Hovered, grey(0.2 * 0.9));

        s.set(DSE::ScrollBarHandleBorder, 0, grey(0.4));
        s.set(DSE::ScrollBarHandleBorder, WS::Inactive, grey(0.27));
        s.set(DSE::ScrollBarHandleBorder, WS::Down, grey(0.26));
        s.set(DSE::ScrollBarHandleBorder, WS::Hovered, grey(0.5));

        s.set(DSE::ScrollBarBodyBg, 0, grey(0.16));
        s.set(DSE::ScrollBarBodyBorder, 0, grey(0.15));

        s.set(DSE::TabNotSelBg, 0, grey(0.12));
        s.set(DSE::TabSelBg, 0, grey(0.18));
        s.set(DSE::TabNotSelBg, WS::Inactive, grey(0.27));
        s.set(DSE::TabSelBg, WS::Inactive, grey(0.27));
        s.set(DSE::TabNotSelBg, WS::Focused, grey(0.3));
        s.set(DSE::TabSelBg, WS::Focused, grey(0.18));

        s.set(DSE::TabContentBg, 0, grey(0.18));
        s.set(DSE::TabSelFocusedBorder, 0, grey(0.5));

        s.set(DSE::TabBorder1, 0, grey(0.15));
        s.set(DSE::TabBorder1, WS::Inactive, grey(0.23));
        s.copy(DSE::TabBorder2, DSE::Border2);


        s.set(DSE::ButtonBgA, 0, grey(0.26));
        s.set(DSE::ButtonBgB, 0, grey(0.26 * 0.55));
        s.set(DSE::ButtonBgA, WS::Hovered, grey(0.28));
        s.set(DSE::ButtonBgB, WS::Hovered, grey(0.28 * 0.75));
        s.set(DSE::ButtonBgA, WS::Inactive, grey(0.27));
        s.set(DSE::ButtonBgB, WS::Inactive, grey(0.27 * 0.8));
        s.set(DSE::ButtonBgA, WS::Focused, grey(0.26));
        s.set(DSE::ButtonBgB, WS::Focused, grey(0.26 * 0.55));
        s.set(DSE::ButtonBgA, WS::Focused | WS::Down, grey(0.26));
        s.set(DSE::ButtonBgB, WS::Focused | WS::Down, grey(0.26 * 0.87));
        s.set(DSE::ButtonBgA, WS::Focused | WS::Hovered, grey(0.28));
        s.set(DSE::ButtonBgB, WS::Focused | WS::Hovered, grey(0.28 * 0.75));

        s.set(DSE::ButtonBorder1, 0, grey(0.15));
        s.set(DSE::ButtonBorder2, 0, grey(0.3));
        s.set(DSE::ButtonBorder1, WS::Inactive, grey(0.23));
        s.set(DSE::ButtonBorder2, WS::Inactive, grey(0.3));
        s.set(DSE::ButtonBorder1, WS::Focused, grey(0.45));
        s.set(DSE::ButtonBorder2, WS::Focused, grey(0.3));
        s.set(DSE::ButtonBorder1, WS::Focused | WS::Down, grey(0.3));
        s.set(DSE::ButtonBorder2, WS::Focused | WS::Down, grey(0.21));

        s.set(DSE::SliderBodyBg, 0, grey(0.13));
        s.set(DSE::SliderBodyBg, WS::Inactive, grey(0.27));

        s.set(DSE::SliderBodyBorder, 0, grey(0.11));
        s.set(DSE::SliderBodyBorder, WS::Inactive, grey(0.23));
        s.set(DSE::SliderBodyBorder, WS::Focused, grey(0.45));

        s.set(DSE::SliderBodyFillBg, 0, grey(0.45));
        s.set(DSE::SliderBodyFillBorder, 0, grey(0.15));

        s.set(DSE::FrameBorder1, 0, grey(0.3));
        s.set(DSE::FrameBorder2, 0, grey(0.45));

        s.set(DSE::FrameTitleBgA, 0, grey(0.3));
        s.set(DSE::FrameTitleBgB, 0, grey(0.3*0.75));
        s.set(DSE::FrameTitleBgA, WS::Focused | WS::Down, grey(0.29));
        s.set(DSE::FrameTitleBgB, WS::Focused | WS::Down, grey(0.29*0.775));
        s.set(DSE::FrameTitleBgA, WS::Focused, grey(0.35));
        s.set(DSE::FrameTitleBgB, WS::Focused, grey(0.35*0.775));

        s.copy(DSE::FrameBg, DSE::Background);

        s.copy(DSE::FrameTitleText, DSE::DispText);

    }

    static void prepare_default_bright_scheme(DefaultStyleColorScheme& s)
    {

        s.reset();
        // text colors
        s.set(DSE::DispText, 0, grey(0.08));
        s.set(DSE::DispText, WS::Inactive, grey(0.4));
        s.set(DSE::ButtonText, 0, grey(0.08));
        s.set(DSE::ButtonText, WS::Inactive, grey(0.25));
        s.set(DSE::EditText, 0, grey(0.08));
        s.set(DSE::EditText, WS::Inactive, grey(.4));
        s.set(DSE::EditTextSelection, 0, grey(.85));
        s.set(DSE::EditTextSelection, WS::Focused, rgb_i(158, 213, 255));
        s.set(DSE::EditTextCursor, 0, grey_premult(0.1, 0.8));

        Color bg = rgb(0.95, 0.96, 0.98);


        s.set(DSE::Background, 0, bg);
        s.set(DSE::Background, WS::Inactive, grey(0.83));

        s.set(DSE::WidgetFillBg, 0, grey(1.0));
        s.set(DSE::WidgetFillBg, WS::Inactive, grey(0.9));

        s.set(DSE::Border1, 0, grey(0.7));
        s.set(DSE::Border1, WS::Inactive, grey(0.6));
        s.set(DSE::Border1, WS::Focused, rgb(0.00, 0.46, 0.8));

        s.set(DSE::Border2, 0, grey(0.74));
        s.set(DSE::Border2, WS::Inactive, grey(0.7));

        s.set(DSE::Check, 0, rgb(0.00, 0.46, 0.8));

        s.set(DSE::CheckBoxBg, 0, grey(1.0));
        s.set(DSE::CheckBoxBg, WS::Down, grey(.94));
        s.set(DSE::CheckBoxBg, WS::Inactive, grey(.8));

        s.set(DSE::CheckBoxBorder, 0, grey(0.5));
        s.set(DSE::CheckBoxBorder, WS::Focused, grey(0.4));
        s.set(DSE::CheckBoxBorder, WS::Inactive, grey(0.73));
        s.set(DSE::CheckBoxBorder, WS::Down, grey(0.7));
        s.set(DSE::CheckBoxBorder, WS::Focused | WS::Down, grey(0.7));

        s.set(DSE::RadioFill, 0, rgb(0.00, 0.46, 0.8));
        s.copy(DSE::RadioBg, DSE::CheckBoxBg);
        s.copy(DSE::RadioBorder, DSE::CheckBoxBorder);

        s.set(DSE::CheckRadioTextEmphasisBgA, 0, grey(.97));
        s.set(DSE::CheckRadioTextEmphasisBgB, 0, grey(0.9));
        s.set(DSE::CheckRadioTextEmphasisBorder, 0, grey(0.8));


        s.set(DSE::ScrollBarHandleBgA, 0, grey(0.97));
        s.set(DSE::ScrollBarHandleBgB, 0, grey(0.97 * 0.95));
        s.set(DSE::ScrollBarHandleBgA, WS::Hovered, grey(0.98));
        s.set(DSE::ScrollBarHandleBgB, WS::Hovered, grey(0.98 * 0.96));
        s.set(DSE::ScrollBarHandleBgA, WS::Inactive, grey(0.85));
        s.set(DSE::ScrollBarHandleBgB, WS::Inactive, grey(0.85 * 0.98));
        s.set(DSE::ScrollBarHandleBgA, WS::Down, grey(0.78));
        s.set(DSE::ScrollBarHandleBgB, WS::Down, grey(0.78 * 0.96));

        s.copy(DSE::ScrollBarHandleBorder, DSE::Border1);

        s.set(DSE::ScrollBarBodyBg, 0, grey(0.85));
        s.set(DSE::ScrollBarBodyBg, WS::Inactive, grey(0.83));

        s.copy(DSE::ScrollBarBodyBorder, DSE::Border1);

        s.set(DSE::TabNotSelBg, 0, lgui::rgb_mult(bg, 0.925));
        s.set(DSE::TabSelBg, 0, grey(0.97));
        s.set(DSE::TabNotSelBg, WS::Inactive, grey(0.8));
        s.set(DSE::TabSelBg, WS::Inactive, grey(0.88));
        s.set(DSE::TabNotSelBg, WS::Focused, grey(0.83));
        s.set(DSE::TabSelBg, WS::Focused, grey(0.97));


        s.set(DSE::TabContentBg, 0, grey(0.97));
        s.set(DSE::TabContentBg, WS::Inactive, grey(0.88));

        s.set(DSE::TabBorder1, 0, grey(0.7));
        s.set(DSE::TabBorder1, WS::Inactive, grey(0.6));

        s.set(DSE::TabBorder2, 0, grey(0.74));
        s.set(DSE::TabBorder2, WS::Inactive, grey(0.7));

        s.set(DSE::TabSelFocusedBorder, 0, grey(0.8));


        s.set(DSE::ButtonBgA, 0, grey(0.94));
        s.set(DSE::ButtonBgB, 0, grey(0.94 * 0.9));
        s.set(DSE::ButtonBgA, WS::Hovered, grey(0.95));
        s.set(DSE::ButtonBgB, WS::Hovered, grey(0.95 * 0.95));
        s.set(DSE::ButtonBgA, WS::Inactive, grey(0.85));
        s.set(DSE::ButtonBgB, WS::Inactive, grey(0.85 * 0.95));
        s.set(DSE::ButtonBgA, WS::Focused, grey(0.94));
        s.set(DSE::ButtonBgB, WS::Focused, grey(0.94 * 0.9));
        s.set(DSE::ButtonBgA, WS::Focused | WS::Down, grey(0.86));
        s.set(DSE::ButtonBgB, WS::Focused | WS::Down, grey(0.86 * 0.97));
        s.set(DSE::ButtonBgA, WS::Focused | WS::Hovered, grey(0.95));
        s.set(DSE::ButtonBgB, WS::Focused | WS::Hovered, grey(0.95 * 0.95));

        s.set(DSE::ButtonBorder1, 0, grey(0.65));
        s.set(DSE::ButtonBorder1, WS::Inactive, grey(0.6));
        s.set(DSE::ButtonBorder1, WS::Focused, rgb(0.00, 0.46, 0.8));
        s.set(DSE::ButtonBorder1, WS::Focused | WS::Down, rgb(0.00, 0.46, 0.8));

        s.set(DSE::ButtonBorder2, 0, grey(0.7));
        s.set(DSE::ButtonBorder2, WS::Inactive, grey(0.7));
        s.set(DSE::ButtonBorder2, WS::Focused, grey(0.7));
        s.set(DSE::ButtonBorder2, WS::Focused | WS::Down, grey(0.79));

        s.set(DSE::SliderBodyBg, 0, grey(0.87));
        s.set(DSE::SliderBodyBg, WS::Inactive, grey(0.73));

        s.set(DSE::SliderBodyBorder, 0, grey(0.55));
        s.set(DSE::SliderBodyBorder, WS::Inactive, grey(0.65));
        s.set(DSE::SliderBodyBorder, WS::Focused, rgb(0.00, 0.46, 0.8));

        s.set(DSE::SliderBodyFillBg, 0, grey(0.65));
        s.set(DSE::SliderBodyFillBorder, 0, grey(0.55));
        s.set(DSE::SliderBodyFillBorder, WS::Focused, rgb(0.00, 0.46, 0.8));

        s.set(DSE::FrameBorder1, 0, grey(0.6));

        s.set(DSE::FrameBorder2, 0, grey(0.55));

        s.set(DSE::FrameTitleBgA, 0, rgb_i(158, 213, 255));
        s.set(DSE::FrameTitleBgB, 0, rgb_mult(rgb_i(158, 213, 255), 0.95));
        s.set(DSE::FrameTitleBgA, WS::Focused | WS::Down, grey(0.71));
        s.set(DSE::FrameTitleBgB, WS::Focused | WS::Down, grey(0.71*0.775));
        s.set(DSE::FrameTitleBgA, WS::Focused, rgb_i(182, 223, 255));
        s.set(DSE::FrameTitleBgB, WS::Focused, rgb_mult(rgb_i(182, 223, 255), 0.95));
        s.set(DSE::FrameTitleBgA, WS::Inactive, grey(0.9));
        s.set(DSE::FrameTitleBgB, WS::Inactive, grey(0.9*0.98));

        s.set(DSE::FrameTitleText, 0, grey(0.1));

        s.set(DSE::FrameBg, 0, rgb_mult(bg, 1.01));
        s.set(DSE::FrameBg, WS::Inactive, grey(0.83));
    }

    DefaultStyleDarkColorScheme::DefaultStyleDarkColorScheme()
    {
        prepare_default_dark_scheme(*this);
        all_set();
    }

    DefaultStyleBrightColorScheme::DefaultStyleBrightColorScheme()
    {
        prepare_default_bright_scheme(*this);
        all_set();
    }

}

