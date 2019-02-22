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

#ifndef LGUI_SCROLLAREA_H
#define LGUI_SCROLLAREA_H

#include "lgui/widgetpc.h"
#include "scrollbar.h"
#include "lgui/iwidgetlistener.h"

namespace lgui {

    /** A scroll area that allows to display another ("content") widget inside its children area with
     *  scrollbars displayed if necessary.  */
    class ScrollArea : public WidgetPC, public IWidgetListener
    {
        public:

            /** Specifies how the ScrollArea shall behave regarding its own size. */
            enum SelfSizeBehavior {
                /** Take whatever width/height the ScrollArea gets, independent of content size.
                    This is the default. */
                Expand,
                /** As long as the resulting size of the ScrollArea is smaller than the space available,
                 *  shrink with the content. This will remove gaps between content and scrollbars.
                 *
                 *  When using this option together with ContentSizeBehavior::ContentNoLimits, there might be
                 *  glitches, i.e. unneeded scrollbars appearing. This is due to the fact that certain layouts
                 *  are not compatible with width-for-height situations that can occur with %ScrollArea. For
                 *  example, RelativeLayout measures all its item horizontally first, granting full vertical
                 *  space. That way, %ScrollArea may decide it does not need a vertical scrollbar when in
                 *  fact it is granted less vertical space when the second measurement is done. The vertical
                 *  scrollbar will need extra space, but as the horizontal space is fixed from the first
                 *  measurement, an annoying horizontal scrollbar will appear, too.
                 */
                ShrinkWithContent,
            };

            /** Specifies how the ScrollArea shall behave regarding the size of its content.
            */
            enum ContentSizeBehavior {
                /** The content may freely choose its size in both dimensions. This is the default. */
                ContentNoLimits = 0,

                /** The width of the content is always resized to the width of the ScrollArea.
                    Space for a vertical scrollbar is deducted when necessary.  This effectively
                    disables horizontal scrolling. */
                ContentForceWidth = 1,

                /** The width of the content is capped to the width of the ScrollArea.
                    Space for a vertical scrollbar is deducted when necessary.  This effectively
                    disables horizontal scrolling. */
                ContentMaxWidth = 5,

                /** The height of the content is always resized to the height of the ScrollArea.
                    Space for a horizontal scrollbar is deducted when necessary.  This effectively
                    disables vertical scrolling. */
                ContentForceHeight = 2,

                /** The height of the content is capped to the height of the ScrollArea.
                    Space for a horizontal scrollbar is deducted when necessary.  This effectively
                    disables vertical scrolling. */
                ContentMaxHeight = 6,

            };


            ScrollArea();
            explicit ScrollArea(Padding &&padding);
            explicit ScrollArea(const Padding& padding);
            ~ScrollArea() override;

            void draw(const DrawEvent& de) const override;

            Rect children_area() const override;

            void set_content(Widget* content);
            Widget* content() { return mcontent; }

            /** Sets how the ScrollArea should behave regarding its own size. */
            void set_size_behavior(SelfSizeBehavior sbhv) {
                mself_size_behavior = sbhv;
                request_layout();
            }

            /** Sets how the ScrollArea should behave regarding the size of its content. */
            void set_content_size_behavior(ContentSizeBehavior cbhv);


            /** Make a portion of the `content` widget visible. Rect is understood to be a rectangle within
             *  `content`. */
            void make_visible(const lgui::Rect& rect);

            const Padding& padding() const { return mpadding; }
            void set_padding(const Padding& padding);

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            Padding mpadding;

            void recalibrate_scrollbars(bool preserve_pos=false);

            void child_about_to_die(Widget& child) override;
            void resized(const Size& old_size) override;
            void style_changed() override;

            void mouse_wheel_down(MouseEvent& event) override;
            void mouse_wheel_up(MouseEvent& event) override;

        private:
            void init_bars();
            // implementation of WidgetListener: listening to content size changes
            void size_changed_wl(Widget& w) override;

            // slots for scrollbars
            void _x_scrolled(int new_x_pos);
            void _y_scrolled(int new_y_pos);

            MeasureResults measure_content(SizeConstraint wc, SizeConstraint hc);


            ScrollBar mhorz_scrollbar, mvert_scrollbar;
            Widget* mcontent;
            SelfSizeBehavior mself_size_behavior;
            ContentSizeBehavior mcontent_size_behavior;
            bool mlisten_to_size_changed;
    };

}

#endif // LGUI_SCROLLAREA_H
