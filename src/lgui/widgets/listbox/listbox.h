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

#ifndef LGUI_LISTBOX_H
#define LGUI_LISTBOX_H

#include "lgui/widgets/scrollarea.h"
#include "stringlistview.h"
#include "lgui/style/style.h"

namespace lgui {
    /** A generic list box with proper scrolling.
     *  This class wraps a concrete, non-generic %ListView in a ScrollArea.
     *  You can use all the size-controlling methods of ScrollArea.
     *
     *  The %ListBox will provide its own model, which you can access by calling model(). You can also set
     *  your own model via set_model(). When you pass nullptr to set_model(), the internal model will continue
     *  to be used again.
     *
     *  You may access the internal %ListView by calling list_view().
     *  Signals are re-emitted for convenience.
     */

    template <class M, class V>
    class GenListBox : public ScrollArea, public IListModelListener
    {
        public:
            explicit GenListBox(M* model=nullptr)
                : mlist_view(model != nullptr ? model : &mmy_model)
            {
                set_padding(style().get_list_box_padding());
                set_content_size_behavior(ContentForceWidth);
                mmodel = mlist_view.model();
                mlist_view.on_selection_changed.connect([this](int idx) { selection_changed(idx); });
                mlist_view.on_item_activated.connect(on_item_activated);
                set_content(&mlist_view);
            }

            void draw(const DrawEvent& de) const override
            {
                style().draw_list_box_bg(de.gfx(), StyleArgs(*this, de));
                ScrollArea::draw(de);
                style().draw_list_box_fg(de.gfx(), StyleArgs(*this, de));
            }

            Signal <int> on_selection_changed;
            Signal <int, const std::string& > on_item_activated;

            /** Return the index of the selected item. */
            int selected_idx() const { return mlist_view.selected_idx(); }

            /** Set the selected item. */
            void set_selected_idx(int idx) { mlist_view.set_selected_idx(idx); }

            /** Return the model in use, const-variant. Will return GenListBox's own
             *  internal model by default. */
            const M& model() const {
                return *mmodel;
            }

            /** Return the model in use. Will return GenListBox's own internal model
             *  by default. */
            M& model() {
                return *mmodel;
            }

            /** Sets the model to use. When you pass nullptr, %GenListBox will
             *  revert to using its own internal model. */
            void set_model(M *model)
            {
                if(model != mmodel) {
                    if(mmodel && mmodel != &mmy_model)
                        mmodel->remove_listener(*this);
                    mmodel = model;
                    if(mmodel == nullptr)
                        mmodel = &mmy_model;
                    else if(mmodel != &mmy_model)
                        mmodel->add_listener(*this);
                    mlist_view.set_model(mmodel);
                }
            }

            V& list_view() { return mlist_view; }
            const V& list_view() const { return mlist_view; }

            void make_visible(int idx) {
                if(idx >= 0 && idx < mmodel->no_items()) {
                    Rect r = mlist_view.rect_for_item(idx);
                    // avoid stupid horizontal scrolling
                    // by considering Y only
                    r.set_pos_x(0);
                    r.set_width(mlist_view.width());
                    ScrollArea::make_visible(r);
                }
            }


        protected:
            void selection_changed(int idx) {
                make_visible(idx);
                on_selection_changed.emit(idx);
            }

            /** React to dying model. */
            void model_about_to_die() override
            {
                mmodel = nullptr;
            }

        private:
            // shield these from outside access
            using ScrollArea::set_content;
            using ScrollArea::content;

            M mmy_model;
            M *mmodel;

            V mlist_view;
    };

    /** A listbox for a StringListModel using a StringListView. */
    using ListBox = GenListBox <StringListModel, StringListView>;

}

#endif // LGUI_LISTBOX_H
