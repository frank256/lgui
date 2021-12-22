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

#ifndef LGUI_WIDGETTRAVERSALSTACK_H
#define LGUI_WIDGETTRAVERSALSTACK_H

#include <list>
#include <array>

#include "lgui/lgui_types.h"
#include "lgui/platform/error.h"

namespace lgui {

class Widget;

namespace dtl {

class WidgetTreeTraversalStack {
    public:
        struct Entry {
            Widget* w;
            PointF p;
        };

        WidgetTreeTraversalStack()
                : mpointer(0), mbase(0) {}

        const static int MAX_TRAVERSAL_DEPTH = 32;

        void push(Widget* w, PointF p) {
            push({w, p});
        }

        void pop() {
            ASSERT(mpointer > 0);
            --mpointer;
        }

        void new_traversal() {
            clear();
        }

        void new_backwards_traversal() {
            mpointer = MAX_TRAVERSAL_DEPTH;
            mbase = 0;
        }

        void backwards_traversal_finished() {
            mbase = mpointer;
            mpointer = MAX_TRAVERSAL_DEPTH - mbase;
        }

        void push_backwards(Widget* w, PointF p) {
            ASSERT(mpointer >= 0);
            mstack[--mpointer] = {w, p};
        }

        int get_no_entries() const {
            return mpointer;
        }

        Entry& get(int idx) {
            return mstack.at(mbase + idx);
        }

        const Entry& get(int idx) const {
            return mstack.at(mbase + idx);
        }

        bool is_empty() const { return get_no_entries() <= 0; }

        Widget* get_topmost_widget() const {
            return mstack.at(mbase + mpointer - 1).w;
        }

        const Widget* topmost_widget() const {
            return mstack.at(mbase + mpointer - 1).w;
        }

        PointF topmost_widget_pos() const {
            return mstack.at(mbase + mpointer - 1).p;
        }

        bool contains(const Widget& w) const {
            for (int i = 0; i < get_no_entries(); ++i) {
                if (get(i).w == &w)
                    return true;
            }
            return false;
        }

        void print() {
            for (int i = 0; i < mpointer; ++i) {
                printf("%d. %p, %0f, %0f\n", i, get(i).w, get(i).p.x(), get(i).p.y());
            }
        }

    private:

        void push(Entry&& entry) {
            ASSERT(mpointer < MAX_TRAVERSAL_DEPTH);
            mstack[mpointer++] = entry;
        }

        void clear() {
            mpointer = 0;
            mbase = 0;
        }

        std::array<Entry, MAX_TRAVERSAL_DEPTH> mstack;
        int mpointer, mbase;
};

}
}

#endif //LGUI_WIDGETTRAVERSALSTACK_H
