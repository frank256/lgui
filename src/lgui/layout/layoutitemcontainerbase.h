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

#ifndef LGUI_LAYOUTITEMCONTAINERBASE_H
#define LGUI_LAYOUTITEMCONTAINERBASE_H

#include <vector>

#include "layout.h"
#include "lgui/basiccontainer.h"
#include "platform/error.h"

namespace lgui {

/** Simple template serving as a base class for layouts that store info about
 *  layout items themselves in a simple container. Manages synchronization of
 *  these with the target.
 */
template<class ItemT, class ContainerT>
class LayoutItemContainerBase : public Layout {
    public:
        static_assert(std::is_same<typename ContainerT::value_type, ItemT>::value,
                      "Please pass a compatible container type for LayoutItemContainerBase.");

        /** "Private" this if you don't want it to be available in your subclass for some reason. */
        void set_elem_margin(const ILayoutElement& elem, const Margin& margin) {
            auto it = find_elem(elem);
            if (it != mitems.end()) {
                it->set_margin(margin);
                if (mtarget && update_on_child_resize())
                    mtarget->request_layout();
            }
        }

        /** Reimplement and call this implementation if you keep more information in your layout. */
        void remove_all() override {
            if (mtarget)
                _cleanup_old_target();
            mitems.clear();
        }

        /** Removes an item from the Layout. Possibly reimplement if you store extra information, but extra
            information should also be removed through _purge_removed_widget(). */
        virtual void remove(ILayoutElement& le) {
            auto it = find_elem(le);
            if (it != mitems.end()) {
                _purge_removed_widget(*it);
                mitems.erase(it);
                removed_elem(le);
            }
        }

    protected:

        void _child_added_to_target(Widget& widget) override {
            (void) widget;
            // Do nothing, since we do not intend to include widgets that are
            // added to the target (instead of the layout) in our layout process.
        }

        void _child_removed_from_target(Widget& widget) override {
            // Here we suspect (correctly?) that something important must have
            // happened, so we, too, drop the widget from the layout.
            // In the regular case, we'd expect that widgets are removed from the
            // layout, not its target.
            if (!mtarget_being_cleaned)
                _remove_widget_fnlh(widget);
        }

        void _new_target() override {
            // Add each widget that we control to the target.
            ASSERT(mtarget); // Should've been set.
//            if(mtarget->no_children() > 0) {
//                warning("LayoutItemContainerBase expects containers with no children (EXCEPTING NESTED LAYOUTS)...");
//            }
            // This should have been cleared by Layout::_set_target
            //ASSERT(!update_on_child_add_remove());
            for (auto& li : mitems) {
                ILayoutElement* le = li.layout_element();
                if (le) {
                    if (le->layout_element_type() == LayoutElementLayout) {
                        Layout* lyt = static_cast<Layout*>(le);
                        lyt->_set_target(mtarget);
                    }
                    else {
                        Widget* w = static_cast<Widget*>(le);
                        if (w->parent() != mtarget)
                            mtarget->add_child(*w); // Will call _child_added_to_target.
                        w->add_widget_listener(this);
                    }
                }
            }
        }

        void _cleanup_old_target() override {
            mtarget_being_cleaned = true;
            for (auto& li : mitems) {
                ILayoutElement* le = li.layout_element();
                if (le) {
                    if (le->layout_element_type() == LayoutElementLayout) {
                        Layout* lyt = static_cast<Layout*>(le);
                        lyt->_set_target(nullptr); // Will do recursive work.
                    }
                    else {
                        Widget* w = static_cast<Widget*>(le);
                        mtarget->remove_child(*w);
                        w->remove_widget_listener(this);
                    }
                }
            }
            mtarget_being_cleaned = false;
        }

        /** Call this after you've added the element to mitems. */
        virtual void added_elem(ILayoutElement& elem) {
            if (mtarget) {
                ASSERT(!mitems.empty() && mitems.back().layout_element() == &elem);
                if (elem.layout_element_type() == LayoutElementLayout) {
                    Layout* lyt = static_cast<Layout*>(&elem);
                    lyt->_set_target(mtarget);
                }
                else {
                    Widget& w = static_cast<Widget&>(elem);
                    if (w.parent() != mtarget)
                        mtarget->add_child(w);
                    w.add_widget_listener(this);
                }
                if (update_on_child_add_remove())
                    mtarget->request_layout();
            }
        }

        /** Call this *after* you've removed the widget from mitems. */
        virtual void removed_elem(ILayoutElement& elem) {
            if (mtarget) {
                if (elem.layout_element_type() == LayoutElementWidget) {
                    Widget* w = static_cast<Widget*>(&elem);
                    mtarget->remove_child(*w);
                    w->remove_widget_listener(this);
                }
                else { // Layout
                    Layout* lyt = static_cast<Layout*>(&elem);
                    lyt->_set_target(nullptr);
                }
                if (update_on_child_add_remove())
                    mtarget->request_layout();
            }
        }

        typename ContainerT::iterator find_elem(const ILayoutElement& elem) {
            for (auto it = mitems.begin(); it != mitems.end(); ++it) {
                if (it->layout_element() == &elem) {
                    return it;
                }
            }
            return mitems.end();
        }

        typename ContainerT::const_iterator find_elem(const ILayoutElement& elem) const {
            for (auto it = mitems.begin(); it != mitems.end(); ++it) {
                if (it->layout_element() == &elem) {
                    return it;
                }
            }
            return mitems.end();
        }

        bool _remove_widget_fnlh(Widget& w) override {
            for (auto it = mitems.begin(); it != mitems.end(); ++it) {
                ILayoutElement* le = it->layout_element();
                if (le) {
                    if (static_cast<ILayoutElement*>(&w) == le) {
                        mitems.erase(it);
                        w.remove_widget_listener(this);
                        _purge_removed_widget(*it);
                        return true;
                    }
                    else if (le->layout_element_type() == LayoutElementLayout) {
                        Layout* lyt = static_cast<Layout*>(le);
                        if (lyt->_remove_widget_fnlh(w))
                            return true;
                    }
                }
            }
            return false;
        }

        /** You *have* to implement this if you store info about the items in your
         *  derived class, i.e. not contained in mitems! */
        virtual void _purge_removed_widget(ItemT& item) { (void) item; }

        ContainerT mitems;
        bool mtarget_being_cleaned = false;
};

/** Template version of LayoutItemContainerBase that takes a container template parameter instead of a type parameter.
 *  Defaults to std::vector as a container type. */
template<class ItemT, template<typename...> class ContainerTemplT=std::vector>
class LayoutItemContainerBase2 : public LayoutItemContainerBase<ItemT, ContainerTemplT<ItemT>> {
};

}

#endif // LGUI_LAYOUTITEMCONTAININGBASE_H
