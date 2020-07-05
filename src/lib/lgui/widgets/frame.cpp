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

#include "frame.h"
#include "lgui/drawevent.h"
#include "lgui/keyevent.h"
#include "lgui/mouseevent.h"
#include "lgui/style/style.h"
#include "lgui/platform/font.h"

namespace lgui {

    Frame::Frame()
        : mdragged(false)
    {
        mpadding = style().get_frame_padding();
        mtitle_padding = style().get_frame_title_padding();

        mtitle_height = font().line_height() + mtitle_padding.vert();
        update_children_area();
        set_focusable(true);
        set_clipped(true);
    }

    Frame::Frame(const std::string& title)
        : Frame()
    {
        mtitle = title;
    }

    void Frame::set_padding(const Padding& padding)
    {
        mpadding = padding;
        update_children_area();
    }

    void Frame::set_title(const std::string& title)
    {
        mtitle = title;
        request_layout();
    }

    MeasureResults Frame::measure(SizeConstraint wc, SizeConstraint hc)
    {
        SizeConstraint cwc = wc.sub(mpadding.horz());
        SizeConstraint chc = hc.sub(mpadding.vert() + mtitle_height);
        Size cm = measure_children(cwc, chc);
        return force_size_constraints(cm, cwc, chc);
    }

    Size Frame::min_size_hint()
    {
        Size csh = mpadding.add(min_size_hint_children());
        return Size(csh.w(), csh.h() + mtitle_height);
    }

    void Frame::draw_background(const DrawEvent& de) const
    {
        WidgetState state(is_focus_among_children(), false, de.draw_disabled(),
                    false, false, mdragged);
        StyleArgs args(*this, de);

        style().draw_frame_bg(de.gfx(), args, title_rect(), mdragged);

        StyleArgs args_title(state, title_rect(), font(), de.opacity());
        style().draw_frame_title(de.gfx(), args_title, mtitle_padding, mtitle);
    }

    void Frame::mouse_released(MouseEvent& event)
    {
        mdragged = false;
        BasicContainer::mouse_released(event);
    }

    void Frame::mouse_pressed(MouseEvent& event)
    {
        if(title_rect().contains(event.x(), event.y())) {
            mdrag_pos = event.pos();
            mdragged = true;
            event.consume();
            if(!is_focus_among_children())
                focus();
            bring_to_front();
        }
        else
            BasicContainer::mouse_pressed(event);
    }

    void Frame::mouse_dragged(MouseEvent& event)
    {
        if(mdragged) {
            set_pos(pos()+(event.pos()-mdrag_pos));
        }
        else
            BasicContainer::mouse_dragged(event);
    }

    // We take focus to change color of our window title
    // and try to forward it immediately
    void Frame::focus_gained(FocusEvent& event)
    {
        (void) event;
        if(focus_child())
            focus_child()->focus();
    }

    void Frame::resized(const Size& old_size)
    {
        (void) old_size;
        update_children_area();
    }

    void Frame::style_changed()
    {
        BasicContainer::style_changed();
        mtitle_padding = style().get_frame_title_padding();
        mpadding = style().get_frame_padding();
        mtitle_height = font().line_height() + mtitle_padding.vert();
        update_children_area();
        // TODO: resize?
    }

    void Frame::update_children_area()
    {
        set_children_area(Rect(mpadding.left_top_offs() + Point(0, mtitle_height),
                               get_children_area_size_for_size(size())));
    }

    Rect Frame::title_rect() const
    {
        return Rect(0, 0, width(), mtitle_height);
    }

    Size Frame::get_children_area_size_for_size(Size size)
    {
        int w = std::max(size.w() - mpadding.horz(), 0);
        int h = std::max(size.h() - mpadding.vert() - mtitle_height, 0);
        return Size(w, h);
    }

}
