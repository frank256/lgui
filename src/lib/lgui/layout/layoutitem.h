#ifndef LGUI_LAYOUTITEM_H
#define LGUI_LAYOUTITEM_H

#include "layout.h"

namespace lgui {
/** Invisible class to pass margins and alignment together with an ILayoutElement when adding widgets
 *  (or nested layouts - %ILayoutElements) to layouts.
 *  The idea is to use it implicitly only, i.e. via {} initializers or by implicit conversion. All layouts
 *  should have methods to add %ILayoutElements (i.e. widgets or nested layouts) which take one of these as
 *  argument rather than an %ILayoutElement itself. LayoutItem will extract the margin and alignment and honor
 *  it when measuring and laying out the %ILayoutElement it wraps.
 *
 *  Example: Use { widget, {left, top, right, bottom} } to implicitly construct one of
 *           these objects when required as an argument to pass a widget with margin.
 */
class LayoutItemProxy {
    public:
        /** Carry an ILayoutElement without margin. */
        LayoutItemProxy(ILayoutElement& elem) // NOLINT: implicit conversion wanted
                : melem(&elem) {}

        /** Carry an ILayoutElement with margin specifications. */
        LayoutItemProxy(ILayoutElement& elem, const Margin& margin)
                : melem(&elem), mmargin(margin) {}

        /** Carry an ILayoutElement with alignment settings. */
        LayoutItemProxy(ILayoutElement& elem, Alignment align)
                : melem(&elem), malign(align) {}

        /** Carry an ILayoutElement with margin specifications and alignment settings. */
        LayoutItemProxy(ILayoutElement& elem, const Margin& margin, Alignment align)
                : melem(&elem), mmargin(margin), malign(align) {}

        /** Carry an empty ILayoutElement consisting of a margin only. Use this for
         *  pure spacing elements.*/
        LayoutItemProxy(const Margin& margin) // NOLINT: implicit conversion wanted
                : melem(nullptr), mmargin(margin) {}

        ILayoutElement* elem() const { return melem; }
        const Margin& margin() const { return mmargin; }
        Alignment align() const { return malign; }
    private:
        ILayoutElement* melem;
        Margin mmargin;
        Alignment malign;
};

/** Class representing an item in a layout. Typically, an item will wrap a widget or
 *  a nested layout together with layout parameters (added in derived classes).
 *  Items can be empty, too, to serve as spacing or stretching items.
 *  The LayoutItem provides a rectangle that can be used to store measurement
 *  information.
 *
 *  Each layout will either provide its own derived class or use this one directly.
 *  The layout / its derived item class should typically construct one of these via
 *  a LayoutItemProxy.
 */
class LayoutItem {
    public:
        explicit LayoutItem(const LayoutItemProxy& proxy)
                : mle(proxy.elem()), mmargin(proxy.margin()), malign(proxy.align()) {}

        explicit LayoutItem(ILayoutElement* le)
                : mle(le) {}

        LayoutItem(ILayoutElement* le, Alignment align)
                : mle(le), malign(align) {}

        LayoutItem(ILayoutElement* le, const Margin& margin)
                : mle(le), mmargin(margin) {}

        LayoutItem(ILayoutElement* le, const Margin& margin, Alignment align)
                : mle(le), mmargin(margin), malign(align) {}

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
        void set_allotted_rect(const Rect& r) { mallotted_rect = r; }

        /** Query the rectangle set for this item. */
        const Rect& allotted_rect() const { return mallotted_rect; }

        /** Query the margin set for this item. */
        const Margin& margin() const { return mmargin; }

        /** Query the alignment set for this item. */
        Alignment alignment() const { return malign; }

        /** Change the margin set for this item.
         *  A layout should call request_layout() after calling this
         *  (if the item is not to be skipped).
        */
        void set_margin(const Margin& margin) {
            mmargin = margin;
        }

        /** Change the alignment. Layout should call request_layout() after this. */
        void set_alignment(const Alignment& align) {
            malign = align;
        }

        /** Return whether the item should be considered for layout. */
        virtual bool skip() const {
            const Widget* w = widget();
            if (w)
                return w->is_gone();
            return false;
        }

        /** Return the ILayoutElement contained in the LayoutItem as a %Widget if it is one.
            @return the widget wrapped in the %LayoutItem or nullptr if there is no %ILayoutElement
                    or if it isn't a %Widget. */
        Widget* widget() {
            if (mle && mle->layout_element_type() == ILayoutElement::LayoutElementWidget)
                return static_cast<Widget*> (mle);
            return nullptr;
        }
        const Widget* widget() const {
            if (mle && mle->layout_element_type() == ILayoutElement::LayoutElementWidget)
                return static_cast<const Widget*> (mle);
            return nullptr;
        }

        /** Return the ILayoutElement contained in the LayoutItem as a %Layout if it is one.
            @return the layout wrapped in the %LayoutItem or nullptr if there is no %ILayoutElement
                    or if it isn't a %Layout. */
        Layout* child_layout() {
            if (mle && mle->layout_element_type() == ILayoutElement::LayoutElementLayout)
                return static_cast<Layout*> (mle);
            return nullptr;
        }
        const Layout* child_layout() const {
            if (mle && mle->layout_element_type() == ILayoutElement::LayoutElementLayout)
                return static_cast<const Layout*> (mle);
            return nullptr;
        }

        ILayoutElement* layout_element() { return mle; }
        const ILayoutElement* layout_element() const { return mle; }

        virtual MeasureResults measure(SizeConstraint wc, SizeConstraint hc);

        virtual Size min_size_hint() {
            if (mle)
                return mle->min_size_hint() + margin();
            else
                return Size() + margin();
        }

        virtual void layout(const Rect& r);

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
        SizeConstraint get_horz_constraint(SizeConstraint wc) const;
        SizeConstraint get_vert_constraint(SizeConstraint hc) const;

        Rect mallotted_rect;
        Size mlast_measure_size;
        Margin mmargin;
        Align malign;
};

}


#endif //LGUI_LAYOUTITEM_H
