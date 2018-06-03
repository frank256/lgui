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

#ifndef LGUI_STRINGLISTVIEW_H
#define LGUI_STRINGLISTVIEW_H

#include "lgui/widget.h"
#include "lgui/signal.h"
#include "stringlistmodel.h"


namespace lgui {
    class MouseEvent;
    class KeyEvent;
    class Font;

    /** A widget that provides a view on a string list model.
     *  It doesn't have scroll bars itself, but is intended to be wrapped in a ScrollArea. See ListBox for a
     *  specially adapted %ScrollArea. */
    class StringListView : public Widget, public IListModelListener {
        public:
            StringListView(StringListModel *model=nullptr, const Font* font = nullptr);

            Signal <int> on_selection_changed;
            Signal <int, const std::string& > on_item_activated;

            virtual void draw(const DrawEvent& de) const override;

            bool select_on_hover_activate_on_click() const {
                return mselect_on_hover_activate_on_click;
            }
            /** When this is set, selection will be altered on hover and
             *  activation will be emitted upon click.
             *  Default behavior is to alter selection on click and
             *  emit "activation" on pressing enter. */
            void set_select_on_hover_activate_on_click(bool enable) {
                mselect_on_hover_activate_on_click = enable;
            }

            bool wrap_around_keys() const { return mwrap_around_keys; }

            /** Set whether the arrow keys should wrap-around the selection
             *  when reaching the start/end of the list.
             *  Disabled per default. */
            void set_wrap_around_keys(bool enable) {
                mwrap_around_keys = enable;
            }

            int selected_idx() const { return mselected_idx; }
            void set_selected_idx(int idx);

            /** Return a rectangle that describes where the item with
             *  idx would be drawn in coordinates of this widget. */
            Rect rect_for_item(int idx) const;

            /** Return the page height the list will use for scrolling on page-up/dn
             *  and optimizing what items to draw.
             *  The list will use the children area height of its parent.
             *  -1 means the property is not available.
             *
             *  @todo: this doesn't have to be public?
             */
            int page_height() const;

            /** Sets the size. The list will always ignore the height and ignore
             *  widths that are too narrow. */
            virtual void set_size(Size s) override;
            using Widget::set_size;

            /** Sets the model to use. */
            void set_model(StringListModel *model);

            /** Return the model in use. */
            StringListModel *model() { return mmodel; }
            /** Return the model in use. */
            const StringListModel *model() const { return mmodel; }

            /** A property to indent the text when drawing. This will
             *  *not* affect the size, i.e. be careful to set useful
             *  sizes yourself when you use it.
             *  If negative, the style will use its standard value. */
            void set_text_indent(int indent) { mindent = indent; }

            Size min_size_hint() override;
            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;

        protected:
            void about_to_add_items(int start_idx, int n) override;
            void about_to_remove_items(int start_idx, int n) override;
            void about_to_invalidate_items() override;
            void items_added(int start_idx, int n) override;
            void items_removed(int start_idx, int n) override;
            void items_invalidated() override;
            void model_about_to_die() override;

            int get_idx_from_pos(const Position& pos) const;

            void emit_activated();

            virtual void mouse_moved(MouseEvent& event) override;
            virtual void mouse_pressed(MouseEvent& event) override;
            virtual void mouse_clicked(MouseEvent& event) override;
            virtual void mouse_dragged(MouseEvent& event) override;
            virtual void key_char(KeyEvent& event) override;
            virtual void key_pressed(KeyEvent& event) override;

        private:
            Padding mpadding;
            StringListModel *mmodel;
            int mselected_idx, mitem_height, mmax_width,
                mindent;
            bool mselect_on_hover_activate_on_click,
                 mwrap_around_keys;
    };
}
#endif // LGUI_STRINGLISTVIEW_H
