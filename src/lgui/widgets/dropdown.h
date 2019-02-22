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

#ifndef LGUI_DROPDOWN_H
#define LGUI_DROPDOWN_H

#include "listbox/listbox.h"
#include "lgui/widget.h"
#include "lgui/eventfilter.h"


namespace lgui {

    /** A drop-down widget. */
    class DropDown : public lgui::Widget
    {
            friend class DropDownEventFilter;
        public:
            DropDown();

            Signal <int> on_selected_changed;

            void draw(const DrawEvent& de) const override;

            int selected() const { return mlistbox.selected_idx(); }
            void set_selected(int idx);

            /** Set the model to use. Pass nullptr to revert to using the
             *  internal default model (provided by ListBox). */
            void set_model(StringListModel *model);

            /** Return the model in use. Will return the internal model per default. */
            StringListModel& model() { return mlistbox.model(); }
            const StringListModel& model() const { return mlistbox.model(); }

            /** Sets the size. Currently, drop-down widget will simply ignore the height and insist on its
             *  minimum width. */
            void set_size(Size s) override;
            using Widget::set_size;

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            void listbox_activated(int idx);

            void mouse_pressed(MouseEvent& event) override;
            void key_pressed(KeyEvent& event) override;
            void key_char(KeyEvent& event) override;
            void style_changed() override;
            void set_font(const Font* font) override;

            void drop_down();
            void fold_up();

        private:
            class DropDownEventFilter : public EventFilter {
                public:
                    explicit DropDownEventFilter(DropDown& dd)
                        : mdd(dd) {}
                protected:
                    bool mouse_event(Widget& w, MouseEvent& event) override;
                private:
                    DropDown& mdd;

            } mdrop_down_event_filter;

            Padding mpadding;
            ListBox mlistbox;
            bool mis_dropped_down, mabove;
    };

}

#endif // LGUI_DROPDOWN_H
