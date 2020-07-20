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

#ifndef LGUI_STYLEARGS_H
#define LGUI_STYLEARGS_H

#include "lgui/widget.h"
#include "lgui/drawevent.h"


namespace lgui {

class Widget;
class Font;

/** A helper struct to pass state to the various style functions in order to avoid many boolean parameters. */
struct WidgetState {

    enum Flags {
        Hovered = 1, Down = 2, Focused = 4, Checked = 8, Selected = 16, Inactive = 32
    };

    inline WidgetState(const Widget& from, bool inactive, bool checked = false, bool down = false,
                       bool selected = false)
            : state(0) {
        if (from.has_focus())
            state |= Focused;
        if (from.is_hovered())
            state |= Hovered;
        if (inactive)
            state |= Inactive;
        if (checked)
            state |= Checked;
        if (down)
            state |= Down;
        if (selected)
            state |= Selected;
    }

    inline WidgetState(bool focused, bool hovered, bool inactive, bool checked = false, bool down = false,
                       bool selected = false)
            : state(0) {
        if (focused)
            state |= Focused;
        if (hovered)
            state |= Hovered;
        if (inactive)
            state |= Inactive;
        if (checked)
            state |= Checked;
        if (down)
            state |= Down;
        if (selected)
            state |= Selected;
    }

    inline bool is(Flags f) const { return state & f; }
    inline bool is_hovered() const { return is(Hovered); }
    inline bool is_inactive() const { return is(Inactive); }
    inline bool is_focused() const { return is(Focused); }
    inline bool is_checked() const { return is(Checked); }
    inline bool is_down() const { return is(Down); }
    inline bool is_selected() const { return is(Selected); }

    int state;
};

/** A helper struct to pass arguments to the various style functions in order to
 *  avoid many similar parameters. */
struct StyleArgs {
    const Font& font;
    const lgui::Rect rect; // copy?
    const WidgetState state;
    const float opacity;

    StyleArgs(const Widget& w, const DrawEvent& de, bool checked = false, bool down = false,
              bool selected = false)
            : font(w.font()), rect(w.size_rect()),
              state(w, de.draw_disabled(), checked, down, selected),
              opacity(de.opacity()) {}

    StyleArgs(const Widget& w, const DrawEvent& de, const Rect& r, bool checked = false, bool down = false,
              bool selected = false)
            : font(w.font()), rect(r),
              state(w, de.draw_disabled(), checked, down, selected),
              opacity(de.opacity()) {}

    StyleArgs(const WidgetState& ws, const Rect& r, const Font& f, float _opacity)
            : font(f), rect(r), state(ws), opacity(_opacity) {}
};

}
#endif // LGUI_STYLEARGS_H
