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



}
#endif // LGUI_LAYOUT_H
