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

#ifndef LGUI_BASICCONTAINER_H
#define LGUI_BASICCONTAINER_H

#include <list>
#include "widget.h"

namespace lgui {

class Layout;

/** A %BasicContainer is a widget with children. As such, it serves as bases class for a number of other
 *  widgets. You can set layouts on a %BasicContainer and its subclasses. When using a layout, you usually
 *  need to add children to the layouts and not the %BasicContainer (but the layout will add them as
 *  children of the %BasicContainer themselves).
 *  Children are publicly accessible e.g. through iterators. See WidgetPC for a widget with private
 *  children (that can not be managed by layouts, though).
 *
 *  Children will reside within the children area. With %BasicContainer, this area will always be as large
 *  as the %BasicContainer. See PaddedContainer for a widget that provides padding.
 *  %BasicContainer will serve as a transparent container by default, see Container for a widget that will
 *  draw a default background.
 *
 *  Note that BasicContainer provides draw_children and draw_background methods to be overriden by derived
 *  classes. Its own draw() method will simply call these.
 */
class BasicContainer : public Widget {
    public:
        BasicContainer();
        ~BasicContainer() override;

        void draw(const DrawEvent& de) const override;
        Widget* get_child_at(PointF p) override;
        Rect children_area() const override;

        void add_child(Widget& widget);
        void remove_child(Widget& widget);
        void remove_all_children();
        void set_layout(Layout* layout);

        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override {
            return measure_children(wc, hc);
        }

        Size min_size_hint() override {
            return min_size_hint_children();
        }

        void layout(const Rect& r) override;
        bool has_layout() const override { return mlayout != nullptr; }

        // Container typedefs and access funcs
        // Only provide const_iterator because they are to pointers
        // and no one should ever be allowed to change our internal
        // pointers! (i.e. const_iterator is not the same as const Widget*,
        //            but rather Widget* const)
        using container_t = std::list<Widget*>;
        using iterator = container_t::const_iterator;
        using reverse_iterator = container_t::const_reverse_iterator;
        iterator begin() const { return mchildren.begin(); }
        iterator end() const { return mchildren.end(); }
        reverse_iterator rbegin() const { return mchildren.rbegin(); }
        reverse_iterator rend() const { return mchildren.rend(); }
        size_t no_children() const { return mchildren.size(); }
        Widget& last() const { return *mchildren.back(); }
        Widget& first() const { return *mchildren.front(); }

        void visit_down(const std::function<void(Widget&)>& f) override;
        void _remove_child(Widget& widget);

    protected:
        void child_about_to_die(Widget& child) override;
        void resized(const Size& old_size) override;

        /** This reimplementation will propagate style changes to children. */
        void style_changed() override;

        virtual void draw_children(const DrawEvent& de) const;
        virtual void draw_background(const DrawEvent& de) const {
            (void) de;
        }

        virtual MeasureResults measure_children(SizeConstraint wc, SizeConstraint hc);
        Size min_size_hint_children() const;

        Rect children_bounding_box() const;

        /** Changes the children area. To be used by derived classes.
         *  Will not perform any checks, will update the layout. */
        void set_children_area(const Rect& children_area);

        void _bring_child_to_front(Widget& child) override;
        void _send_child_to_back(Widget& child) override;

        /** Used during the layout process to see how large the children area shall be given a potential size of the widget. */
        virtual Size get_children_area_size_for_size(Size size) { return size; }

    private:
        container_t mchildren;
        Layout* mlayout;
        Rect mchildren_area;
};

}

#endif // LGUI_BASICCONTAINER_H
