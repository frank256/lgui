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

#ifndef LGUI_LAYOUT_H
#define LGUI_LAYOUT_H

#include <list>
#include "lgui/iwidgetlistener.h"
#include "lgui/widget.h"

namespace lgui {

    class BasicContainer;

    /** The abstract layout base class.
     *
     *  Layouts are responsible for arranging the children of a BasicContainer. They need to be
     *  attached via BasicContainer::set_layout(). Layouts can be nested (i.e. you can add other
     *  layouts to layouts, containing a group of children), that's why they share a
     *  common base with Widget (ILayoutElement). All layouts within a nested hierarchy share the
     *  same parent /widget/, however (called the target widget).
     *
     *  Layouts can automatically react to changes of the childrens' size or position. You can control
     *  whether they should do so. Usually, every operation that could affect the layout should be
     *  followed by a call to Widget::request_layout(), so this additional monitoring should not be
     *  necessary.
     *
     *  @see \ref layout for more information*/
    class Layout : public IWidgetListener, public ILayoutElement
    {
        protected:
            /** The actual layout method, derived layouts need to reimplement this. */
            virtual void do_layout(const Rect& r) = 0;

        public:
            Layout();
            ~Layout() override;

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override { (void) wc; (void) hc;  return Size(); }
            Size min_size_hint() override { return Size(0, 0); }

            void layout(const Rect& r) override;

            /** Remove all items from the layout. Reimplement to drop information you keep. */
            virtual void remove_all() = 0;

            LayoutElementType layout_element_type() const final { return LayoutElementLayout; }

            bool update_on_child_add_remove() const { return mupdate_on_child_add_remove; }
            void set_update_on_child_add_remove(bool u) { mupdate_on_child_add_remove = u; }
            bool update_on_child_resize() const { return mupdate_on_child_resize; }
            void set_update_on_child_resize(bool u) { mupdate_on_child_resize = u; }
            bool update_on_child_relocation() const { return mupdate_on_child_relocation; }
            void set_update_on_child_relocation(bool u) { mupdate_on_child_relocation = u; }

            void set_update_on_child_enable_all() {
                set_update_on_child_add_remove(true);
                set_update_on_child_resize(true);
                set_update_on_child_relocation(true);
            }

            void set_update_on_child_disable_all() {
                set_update_on_child_add_remove(false);
                set_update_on_child_resize(false);
                set_update_on_child_relocation(false);
            }

            /** Broadcast the request to remove a widget from a nested layout hierarchy.
             *  Return true if widget has been found and removed.
             */
            virtual bool _remove_widget_fnlh(Widget& w) = 0;

            /** Called whenever a child is added to target.
             *  Default implementation will just register the layout as a widget listener
             *  of the widget. */
            virtual void _child_added_to_target(Widget& widget);

            /** Called whenever a child is removed from target.
             *  Default implementation will just deregister the layout as a
                widget listener from the widget.*/
            virtual void _child_removed_from_target(Widget& widget);

            /**  Internal. You don't normally want to use this, but BasicContainer::set_layout(). */
            void _set_target(BasicContainer* target);

            // implementation of WidgetListener interface:
            void size_changed_wl(Widget& w) override;
            void pos_changed_wl(Widget& w) override;
            void visibility_changed_wl(Widget& w, bool gone_changed) override;
        protected:
            /** The widget whose children a laid out by the layout. */
            BasicContainer* mtarget;

            /** Called whenever a new target has been set for the layout. Reimplement this to add
             *  widgets to the target from Layout's internal data structures.
             *  Default implementation will just call _child_added_to_target for all the children. */
            virtual void _new_target();

            /** Called whenever a target is about to be changed.
             *  Default implementation will just deregister the layout as a listener from all
             *  the children of the target. */
            virtual void _cleanup_old_target();

        private:
            bool mis_laying_out;
            bool mupdate_on_child_add_remove,
                 mupdate_on_child_resize,
                 mupdate_on_child_relocation;
    };

    /** Invisible class to pass margins together with an ILayoutElement when adding widgets
     *  (or nested layouts - ILayoutElements) to layouts.
     *  The idea is to use it implicitly only, i.e. via {} initializers or by implicit conversion. All layouts
     *  should have methods to add ILayoutElements (i.e. widgets or nested layouts) which take one of these as
     *  argument rather than an ILayoutElement itself. LayoutItem will extract the margin and honor it when
     *  measuring.
     *
     *  Example: Use { widget, Margin(left, top, right, bottom) } to implicitly construct one of
     *           these objects when required as an argument to pass a widget with margin. Simply
     *           use widget to pass widgets with no margin through implicit conversion.
     */
    class LayoutItemProxy {
        public:
            /** Carry an ILayoutElement without margin. */
            LayoutItemProxy(ILayoutElement& elem)
                : melem(&elem) {}
            /** Carry an ILayoutElement with margin specifications. */
            LayoutItemProxy(ILayoutElement& elem, const Margin& margin)
                : melem(&elem), mmargin(margin) {}
            /** Carry an empty ILayoutElement with a margin. Use this if pure spacing elements should be needed.*/
            LayoutItemProxy(const Margin& margin)
                : melem(nullptr), mmargin(margin) {}

            ILayoutElement* elem() const { return melem; }
            const Margin& margin() const { return mmargin; }
        private:
            ILayoutElement* melem;
            Margin mmargin;
    };



    /** Class representing an item in a layout. Typically, an item will wrap a widget or
     *  a nested layout together with layout parameters (added in derived classes).
     *  Items can be empty, too, to serve as spacing or stretching items.
     *  The LayoutItem provides a rectangle that can be used to store measurement
     *  information.
     *
     *  Each layout will either provide its own derived class or use this one directly.
     */
    class LayoutItem {
        public:
            LayoutItem(const LayoutItemProxy& proxy)
                : mle(proxy.elem()), mmargin(proxy.margin())
            {}

            LayoutItem(ILayoutElement* le)
                : mle(le)
            {}

            LayoutItem(ILayoutElement* le, const Margin& margin)
                : mle(le), mmargin(margin)
            {}

            virtual ~LayoutItem() = default;

            /** Query the size set for this item. */
            Size allotted_size() const { return mallotted_rect.size(); }

            /** Change the size set for this item. */
            void set_allotted_size(const Size& s) { mallotted_rect.set_size(s); }

            /** Change the position set for this item. */
            void set_allotted_pos(const Position& p) { mallotted_rect.set_pos(p); }

            /** Change the position set for this item. */
            void set_allotted_pos(int x, int y) { mallotted_rect.set_pos(x, y); }

            /** Query the position set for this item. */
            Position allotted_pos() const { return mallotted_rect.pos(); }


            /** Change the rectangle set for this item. */
            void set_allotted_rect(const Rect& r ) { mallotted_rect = r; }

            /** Query the rectangle set for this item. */
            const Rect& allotted_rect() const { return mallotted_rect; }

            /** Query the margin set for this item. */
            const Margin& margin() const { return mmargin; }

            /** Change the margin set for this item.
             *  A layout should call request_layout() after calling this
             *  (if the item is not to be skipped).
            */
            void set_margin(const Margin& margin) {
                mmargin = margin;
            }

            /** Return whether the item should be considered for layout. */
            virtual bool skip() const {
                const Widget* w = widget();
                if(w)
                    return w->is_gone();
                return false;
            }

            /** Return the ILayoutElement contained in the LayoutItem as a %Widget if it is one.
                @return the widget wrapped in the %LayoutItem or nullptr if there is no %ILayoutElement
                        or if it isn't a %Widget. */
            Widget* widget() {
                if(mle && mle->layout_element_type() == ILayoutElement::LayoutElementWidget)
                    return static_cast<Widget*> (mle);
                return nullptr;
            }
            const Widget* widget() const {
                if(mle && mle->layout_element_type() == ILayoutElement::LayoutElementWidget)
                    return static_cast<const Widget*> (mle);
                return nullptr;
            }

            /** Return the ILayoutElement contained in the LayoutItem as a %Layout if it is one.
                @return the layout wrapped in the %LayoutItem or nullptr if there is no %ILayoutElement
                        or if it isn't a %Layout. */
            Layout* child_layout() {
                if(mle && mle->layout_element_type() == ILayoutElement::LayoutElementLayout)
                    return static_cast<Layout*> (mle);
                return nullptr;
            }
            const Layout* child_layout() const {
                if(mle && mle->layout_element_type() == ILayoutElement::LayoutElementLayout)
                    return static_cast<const Layout*> (mle);
                return nullptr;
            }

            ILayoutElement* layout_element() { return mle; }
            const ILayoutElement* layout_element() const { return mle; }

            virtual MeasureResults measure(SizeConstraint wc, SizeConstraint hc) {
                if(mle) {
                    return force_size_constraints(mmargin.add(mle->measure(wc.sub(mmargin.horz()), hc.sub(mmargin.vert()))),
                                                  wc, hc);
                }
                else {
                    return force_size_constraints(Size() + mmargin, wc, hc);
                }
            }

            virtual Size min_size_hint() {
                if(mle)
                    return mle->min_size_hint() + margin();
                else
                    return Size() + margin();
            }

            virtual void layout(const Rect& r) {
                if(mle) {
                    mle->layout(Rect(r.pos() + mmargin.left_top_offs(),
                                     mmargin.sub(r.size())));
                }
            }

            // Debugging:
            const char* name() const {
                if (mle)
                    return mle->name().c_str();
                else
                    return "(null)";
            }

        protected:
            /** The layout element the %LayoutItem contains. May be nullptr for items representing
             *  spacing or stretch elements. */
            ILayoutElement* mle;
        private:
            Rect mallotted_rect;
            Margin mmargin;
    };

}
#endif // LGUI_LAYOUT_H
