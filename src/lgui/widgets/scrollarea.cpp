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

#include "scrollarea.h"
#include "scrollbar.h"
#include "platform/graphics.h"

namespace lgui {

    ScrollArea::ScrollArea()
        : mpadding(0),
          mhorz_scrollbar(Horizontal),
          mvert_scrollbar(Vertical),
          mcontent(nullptr),
          mself_size_behavior(Expand),
          mcontent_size_behavior(ContentNoLimits),
          mlisten_to_size_changed(true)
    {
        init_bars();
    }

    ScrollArea::ScrollArea(const Padding& padding)
        : ScrollArea()
    {
        mpadding = padding;
    }

    ScrollArea::~ScrollArea()
    {
        set_content(nullptr);
    }

    // Need to reimplement this so we can get rid of content pointer
    void ScrollArea::child_about_to_die(Widget& child)
    {
        if(&child == mcontent)
            set_content(nullptr);
        else // content is removed now
            WidgetPC::child_about_to_die(child);
    }

    void ScrollArea::draw(const DrawEvent& de) const
    {
        draw_private_children(de, true, true);
    }

    Rect ScrollArea::children_area() const
    {
        int w = width()-mpadding.horz(),
            h = height()-mpadding.vert();
        if(mvert_scrollbar.is_visible()) {
            w -= mvert_scrollbar.width();
        }
        if(mhorz_scrollbar.is_visible()) {
            h -= mhorz_scrollbar.height();
        }
        return Rect(mpadding.left(), mpadding.top(), w, h);
    }

    void ScrollArea::init_bars()
    {
        mhorz_scrollbar.on_scrolled.connect(&ScrollArea::_x_scrolled, *this);
        mvert_scrollbar.on_scrolled.connect(&ScrollArea::_y_scrolled, *this);
        mhorz_scrollbar.set_outside_children_area(true);
        mvert_scrollbar.set_outside_children_area(true);
        add_private_child(mhorz_scrollbar);
        add_private_child(mvert_scrollbar);
    }

    void ScrollArea::set_content(Widget* content)
    {
        if(mcontent) {
            mcontent->remove_widget_listener(this);
            remove_private_child(*mcontent);
            mcontent = nullptr;
        }
        mcontent = content;
        if(mcontent) {
            ASSERT(mcontent->parent() == nullptr);
            add_private_child(*mcontent);
            _send_child_to_back(*mcontent);
            mcontent->add_widget_listener(this);
            mcontent->set_pos(0, 0);
            mcontent->request_layout();
        }
    }

    void ScrollArea::set_content_size_behavior(ScrollArea::ContentSizeBehavior cbhv) {
        mcontent_size_behavior = cbhv;
        // Want to see it applied to content:
        if(mcontent)
            mcontent->set_need_relayout(true);
        request_layout();
    }

    void ScrollArea::make_visible(const Rect& rect)
    {
        // assume children_area is fixed on (0, 0) here (?)
        // nothing to do if no content and/or no scrollbars
        if(!mcontent || (!mhorz_scrollbar.is_visible() &&
                         !mvert_scrollbar.is_visible()))
            return;
        // clip with content rect
        lgui::Rect r = rect;
        r.clip_to(mcontent->size_rect());

        // horizontally
        if(mhorz_scrollbar.is_visible()) {
            int nx = mhorz_scrollbar.scroll_pos();
            int left = r.x1()+mcontent->pos_x(),
                right = r.x2()+mcontent->pos_x();

            int leftd = -left, rightd = right-children_area().w();
            if(leftd >= 0 && rightd >= 0) {
                // rect to be shown is too big and already visible
                // move into direction we have to move less
                // (through == 0: don't move repeatedly)
                if(leftd <= rightd) {
                    nx = r.x1();
                }
                else {
                    nx = r.x2()-children_area().w();
                }
            }
            else if(leftd > 0) {
                nx = r.x1();
            }
            else if(rightd > 0) {
                nx = r.x2()-children_area().w();
            }
            mhorz_scrollbar.scroll_to(nx);
        }
        // vertically
        if(mvert_scrollbar.is_visible()) {
            int ny = mvert_scrollbar.scroll_pos();
            int top = r.y1()+mcontent->pos_y(),
                bottom = r.y2()+mcontent->pos_y();

            int topd = -top, bottomd = bottom-children_area().h();
            //debug("\ntop: %d, bottom: %d, topd: %d, bottomd: %d\n", top, bottom, topd, bottomd);
            if(topd >= 0 && bottomd >= 0) {
                // rect to be shown is too big and already visible
                // move into direction we have to move less
                // (through == 0: don't move repeatedly)
                if(topd <= bottomd) {
                    ny = r.y1();
                }
                else {
                    ny = r.y2()-children_area().h();
                }
            }
            else if(topd > 0) {
                ny = r.y1();
            }
            else if(bottomd > 0) {
                ny = r.y2()-children_area().h();
            }
            mvert_scrollbar.scroll_to(ny);
        }
    }


    void ScrollArea::resized(const Size& old_size)
    {
        (void) old_size;
        if(mcontent && mcontent->needs_relayout()) {
            mlisten_to_size_changed = false;
            // measure_content will force necessary constraints
            MeasureResults mr = measure_content(SizeConstraint(width(), SizeConstraintMode::Maximum),
                                               SizeConstraint(height(), SizeConstraintMode::Maximum));

            mcontent->layout(Rect(Position(0, 0), mr));
            mlisten_to_size_changed = true;
        }
        recalibrate_scrollbars(true);
    }

    // listen to content's size changes
    void ScrollArea::size_changed_wl(Widget& w)
    {
        if(!mlisten_to_size_changed)
            return;
        ASSERT(&w == mcontent);

        if(!layout_in_progress() && mself_size_behavior == ShrinkWithContent)
            request_layout();
        else {
            // Guard content size
            // FIXME: Save last measurement
            if(mcontent_size_behavior != ContentNoLimits) {
                mlisten_to_size_changed = false;
                MeasureResults mr = measure_content(SizeConstraint(width(), SizeConstraintMode::Maximum),
                                                   SizeConstraint(height(), SizeConstraintMode::Maximum));
                mcontent->layout(Rect(Position(0, 0), mr));
                mlisten_to_size_changed = true;
            }
            recalibrate_scrollbars(true);
        }
    }

    void ScrollArea::_x_scrolled(int new_x_pos)
    {
        if(mcontent)
            mcontent->set_pos(-new_x_pos, mcontent->pos_y());
    }

    void ScrollArea::_y_scrolled(int new_y_pos)
    {
        if(mcontent) {
            mcontent->set_pos(mcontent->pos_x(), -new_y_pos);
        }
    }


    void ScrollArea::recalibrate_scrollbars(bool preserve_pos)
    {
        if(mcontent) {
            lgui::Size avail = mpadding.sub(size());

            bool need_x_bar, need_y_bar;
            need_x_bar = mcontent->width() > avail.w();
            need_y_bar = mcontent->height() > avail.h();

            // need scroll bar because of other scroll bar?
            if(need_x_bar)
                need_y_bar = mcontent->height() > (avail.h() - mhorz_scrollbar.min_size_hint().h());

            if(need_y_bar)
                need_x_bar = mcontent->width() > (avail.w() - mvert_scrollbar.min_size_hint().w());

            if(need_x_bar) {
                int xpos = preserve_pos ? mhorz_scrollbar.scroll_pos() : 0;
                int x_bar_width = avail.w();
                if(need_y_bar)
                    x_bar_width -= mvert_scrollbar.width();

                mhorz_scrollbar.setup(mcontent->width(), x_bar_width, xpos, 10);
                mhorz_scrollbar.set_pos(mpadding.left(),
                                        mpadding.top()+avail.h()-mhorz_scrollbar.height());
                mhorz_scrollbar.set_visible();
            }
            else {
                // fix content becoming hidden when shrinking so that scrollbar disappears
                if(mhorz_scrollbar.is_visible())
                    mhorz_scrollbar.scroll_to(0);
                mhorz_scrollbar.set_invisible();
            }

            if(need_y_bar) {
                int ypos = preserve_pos ? mvert_scrollbar.scroll_pos() : 0;
                int y_bar_height = avail.h();
                if(need_x_bar)
                    y_bar_height -= mhorz_scrollbar.height();
                mvert_scrollbar.setup(mcontent->height(), y_bar_height, ypos, 10);
                mvert_scrollbar.set_pos(mpadding.left() + avail.w() - mvert_scrollbar.width(),
                                        mpadding.top());
                mvert_scrollbar.set_visible();
            }
            else {
                // fix content becoming hidden when shrinking so that scrollbar disappears
                if(mvert_scrollbar.is_visible())
                    mvert_scrollbar.scroll_to(0);
                mvert_scrollbar.set_invisible();
            }
        }
        else {
            mvert_scrollbar.set_invisible();
            mhorz_scrollbar.set_invisible();
        }
    }

    void ScrollArea::mouse_wheel_down(MouseEvent& event)
    {
        if(mvert_scrollbar.is_visible())
            mvert_scrollbar.mouse_wheel_down(event);
    }

    void ScrollArea::mouse_wheel_up(MouseEvent& event)
    {
        if(mvert_scrollbar.is_visible())
            mvert_scrollbar.mouse_wheel_up(event);
    }


    void ScrollArea::set_padding(const Padding& padding)
    {
        mpadding = padding;
        recalibrate_scrollbars(true);
    }

    MeasureResults ScrollArea::measure_content(SizeConstraint wc, SizeConstraint hc)
    {
        ASSERT(mcontent);
        SizeConstraint cwc = SizeConstraint::no_limits(),
                       chc = SizeConstraint::no_limits();

        if((mcontent_size_behavior & ContentForceWidth) &&
           wc.mode() != SizeConstraintMode::NoLimits) {
            mcontent->set_need_relayout(true);
            SizeConstraintMode mode = (mcontent_size_behavior == ContentMaxWidth) ? SizeConstraintMode::Maximum :
                                                                                    SizeConstraintMode::Exactly;
            cwc = SizeConstraint(wc.value() - mpadding.horz(), mode);
            // Do we need a scrollbar in the other direction?
            if(hc.mode() != SizeConstraintMode::NoLimits) {
                MeasureResults mr = mcontent->measure(cwc, chc);
                // Don't have to care about a horizontal scrollbar here, because it is
                // effectively disabled.
                if(mr.h() > hc.value() - mpadding.vert()) { // Yes: remeasure below
                    cwc = SizeConstraint(cwc.value() - mvert_scrollbar.min_size_hint().w(), mode);
                }
                else
                    return mr;
            }
        }

        if((mcontent_size_behavior & ContentForceHeight) &&
           hc.mode() != SizeConstraintMode::NoLimits) {
            mcontent->set_need_relayout(true);
            SizeConstraintMode mode = (mcontent_size_behavior == ContentMaxHeight) ? SizeConstraintMode::Maximum :
                                                                                     SizeConstraintMode::Exactly;
            chc = SizeConstraint(hc.value() - mpadding.vert(), mode);
            // Do we need a scrollbar in the other direction?
            if(wc.mode() != SizeConstraintMode::NoLimits) {
                MeasureResults mr = mcontent->measure(cwc, chc);
                // Don't have to care about a vertical scrollbar here, because it is
                // effectively disabled.
                if(mr.w() > wc.value() - mpadding.horz()) // Yes: remeasure below
                    chc = SizeConstraint(chc.value() - mhorz_scrollbar.min_size_hint().h(), mode);
                else
                    return mr;
            }
        }

        return mcontent->measure(cwc, chc);
    }

    MeasureResults ScrollArea::measure(SizeConstraint wc, SizeConstraint hc)
    {
//        debug("ScrollArea::measure called with wc=%d (%d), hc=%d (%d).\n", wc.value(), wc.mode(),
//                 hc.value(), hc.mode());
        if (wc.mode() == SizeConstraintMode::NoLimits && hc.mode() == SizeConstraintMode::NoLimits) {
            if(!mcontent)
                return min_size_hint();
            else
                return mpadding.add(mcontent->measure(wc, hc));
        }

        if (mself_size_behavior == Expand) {
            if (wc.mode() != SizeConstraintMode::NoLimits
               && hc.mode() != SizeConstraintMode::NoLimits) {
                // Take all the space.
                int max_w = wc.value(),
                    max_h = hc.value();
//                debug("\nScrollArea expands: %d, %d", max_w, max_h);
                return force_size_constraints(Size(max_w, max_h), wc, hc);
            }
            else { // one of wc or hc is == NoLimits (case of both having been dealt with above)
                MeasureResults mr = measure_content(wc, hc);

                if (hc.mode() == SizeConstraintMode::NoLimits) {
                    int add_h = mpadding.vert();
                    int comp_w = wc.value() - mpadding.horz();

                    if(mr.w() > comp_w)
                        add_h += mhorz_scrollbar.min_size_hint().h();

                    return Size(wc.value(), mr.h() + add_h);
                }
                else { // wc == NoLimits
                    int add_w = mpadding.horz();
                    int comp_h = hc.value() - mpadding.vert();

                    if(mr.h() > comp_h)
                        add_w += mvert_scrollbar.min_size_hint().w();

                    return Size(mr.w() + add_w, hc.value());
                }
            }
        }
        else { // msize_behavior_mode == ShrinkWithContent
            MeasureResults mr = measure_content(wc, hc);

//            debug("\ncontent wants to become: %d, %d\n", mr.w(), mr.h());

            int content_based_w = mr.w() + mpadding.horz();
            int content_based_h = mr.h() + mpadding.vert();

            bool need_vert_bar, need_horz_bar;

            need_vert_bar = hc.mode() != SizeConstraintMode::NoLimits &&
                                content_based_h > hc.value();

            need_horz_bar = wc.mode() != SizeConstraintMode::NoLimits &&
                                content_based_w > wc.value();

            if(need_horz_bar)
                content_based_h += mhorz_scrollbar.min_size_hint().h();

            if(need_vert_bar)
                content_based_w += mvert_scrollbar.min_size_hint().w();

            int min_w = wc.value(), min_h = hc.value();

            if (wc.mode() == SizeConstraintMode::NoLimits || content_based_w < min_w)
                min_w = content_based_w;
            if (hc.mode() == SizeConstraintMode::NoLimits || content_based_h < min_h)
                min_h = content_based_h;

            return force_size_constraints(Size(min_w, min_h), wc, hc);
        }
    }

    Size ScrollArea::min_size_hint()
    {
        if(mcontent && mself_size_behavior == ShrinkWithContent)
        {
            Size mr = mpadding.add(mcontent->min_size_hint());
            return mr;
        }
        else
            return mpadding.add(Size(mhorz_scrollbar.min_size_hint().w(),
                                     mvert_scrollbar.min_size_hint().h()));
    }



    void ScrollArea::style_changed()
    {

        WidgetPC::style_changed();

        //if(mhorz_scrollbar.size() != hs || mvert_scrollbar.size() != vs)
            //recalibrate_scrollbars(true);
    }


}
