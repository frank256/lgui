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

#ifndef LGUI_STACKEDCONTAINER_H
#define LGUI_STACKEDCONTAINER_H

#include "container.h"
#include "lgui/layout/layout.h"


#include "platform/error.h"

namespace lgui {
    /** A special container that only displays one of its children at a time. */
    class StackedContainer : public PaddedContainer, public IWidgetListener
    {
        public:
            StackedContainer();
            ~StackedContainer() override;

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

            void set_active_widget(Widget* widget);
            Widget* active_widget() { return mactive_widget; }
            const Widget* active_widget() const { return mactive_widget; }


            /** Set whether the widget's role in layout should only be determined by the active widget.
             *   If `false` is passed (default), the maximum of all childrens' sizes is taken when measuring.
             */
            void set_layout_consider_active_only(bool lcao) {
                mlayout_consider_active_only = lcao;
            }

            bool layout_consider_active_only() const {
                return mlayout_consider_active_only;
            }

            Widget* get_child_at(int x, int y) override;
            void draw_children(const DrawEvent& de) const override;

            // implementation of IWidgetListener, listen to self
            void child_added_wl(Widget& w, Widget& child) override;
            void child_removed_wl(Widget& w, Widget& child) override;

        private:
            // class uses its own layout, disallow setting other layouts
            using BasicContainer::set_layout;

            class StackedLayout : public Layout
            {
                public:
                    StackedLayout() = default;
                protected:

                    void do_layout(const Rect& r) override;
                    bool _remove_widget_fnlh(Widget& w) override;
                    void remove_all() override;
            } mlayout;
            Widget* mactive_widget;
            bool mlayout_consider_active_only;
    };
}

#endif // LGUI_STACKEDCONTAINER_H
