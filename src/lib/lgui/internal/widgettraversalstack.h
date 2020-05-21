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
            return mstack.at(mbase + mpointer-1).w;
        }

        const Widget * topmost_widget() const {
            return mstack.at(mbase + mpointer-1).w;
        }

        PointF topmost_widget_pos() const {
            return mstack.at(mbase + mpointer-1).p;
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

#endif //LGUITEST_WIDGETTRAVERSALSTACK_H
