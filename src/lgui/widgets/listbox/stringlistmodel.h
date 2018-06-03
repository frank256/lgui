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

#ifndef LGUI_STRING_LIST_MODEL_H
#define LGUI_STRING_LIST_MODEL_H

#include <vector>
#include <string>
#include "abstractlistmodel.h"
#include "platform/error.h"

namespace lgui {

    /** A generic list model, wrapping a std::vector<T> in a class that will tell its listeners about changes.
    */
    template <class T>
    class GenListModel : public AbstractListModel {
        public:

            /** Return the number of items. */
            int no_items() const { return mitems.size(); }

            /** Return whether the index is valid. */
            bool is_valid_index(int idx) const
            {
                return idx >= 0 && idx < signed(mitems.size());
            }

            /** Return a const-reference to an item referred to by an index.
             *
             * @warning This is unsafe, since the reference may become invalid as soon as the model is changed.
             *          Besides, it will terminate/crash if the index is not valid.
             */
            const T& item_at(int idx) const {
                ASSERT(is_valid_index(idx));
                return mitems[idx];
            }

            /** Return a copy of an item referred to by an index (safe).
             *
             *  Will return a default-constructed item when the index is invalid. */
            T item_at_safe(int idx) const
            {
                if(is_valid_index(idx))
                    return mitems[idx];
                else
                    return T();
            }

            /** Clear all items of the model. */
            void clear_items()
            {
                emit_about_to_invalidate_items();
                mitems.clear();
                emit_items_invalidated();
            }

            /** Add a single item to the model. */
            void add_item(const T& item)
            {
                emit_about_to_add_items(mitems.size(), 1);
                mitems.push_back(item);
                emit_items_added(mitems.size()-1, 1);
            }

            /** Add a list of items to a model. */
            void add_items(const std::vector<T>& items)
            {
                if(!items.empty()) {
                    emit_about_to_add_items(mitems.size(), items.size());
                    for(const auto& it : items) {
                        mitems.push_back(it);
                    }
                    emit_items_added(mitems.size()-items.size(), items.size());
                }
            }

            /** Insert an item into the model. If the index is not valid, the item will be appended to the end.
            */
            void insert_item(const T& item, int idx)
            {
                if(is_valid_index(idx)) {
                    emit_about_to_add_items(idx, 1);
                    mitems.insert(mitems.begin()+idx, item);
                    emit_items_added(idx, 1);
                }
                else
                    add_item(item);
            }

            /** Remove the first ocurrence of an item from the model. */
            void remove_item(const T& item)
            {
                int idx = index_of(item);
                if(idx >= 0) {
                    emit_about_to_remove_items(idx, 1);
                    mitems.erase(mitems.begin()+idx);
                    emit_items_removed(idx, 1);
                }

            }

            /** Remove an item from the model. Will return true if the index has been valid and the item has
             *  been removed, otherwise will return false. */
            bool remove_item(int idx)
            {
                if(is_valid_index(idx)) {
                    emit_about_to_remove_items(idx, 1);
                    mitems.erase(mitems.begin()+idx);
                    emit_items_removed(idx, 1);
                    return true;
                }
                else
                    return false;
            }

            /** Return the index of the first occurence of the item in the model or -1 if there is none. */
            int index_of(const T& item) const
            {
                int idx = 0;
                for(const auto& it : mitems) {
                    if(it == item)
                        return idx;
                    idx++;
                }
                return -1;
            }

        private:
            std::vector <T> mitems;
    };

    /** A string list model. */
    using StringListModel = GenListModel<std::string>;
}

#endif // LGUI_STRING_LIST_MODEL_H
