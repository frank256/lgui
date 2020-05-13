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

#ifndef LGUI_WIDGET_H
#define LGUI_WIDGET_H

#include <forward_list>
#include <cstddef>
#include <cstdint>
#include <functional>

#include "lgui.h"
#include "ieventlistener.h"
#include "ilayoutelement.h"

namespace lgui {

class EventFilter;
class GUI;
class Style;
class IWidgetListener;
class DrawEvent;
class Graphics;
class Font;

namespace dtl { class EventHandlerBase;  class FocusManager; }


/**
 * The widget base class. It has only an abstract capability to have children.
 * Use a `BasicContainer` for that.
 * @see BasicContainer
 */
class Widget : public IEventListener, public ILayoutElement
{
        friend class GUI;
        friend class dtl::EventHandlerBase;
    public:
        Widget();

        Widget(const Widget& other) = delete;
        Widget(Widget&& other) = delete;
        Widget operator=(const Widget& other) = delete;

        ~Widget() override;

        /** Draw the widget.
         */
        virtual void draw(const DrawEvent& de) const;

        const Rect& rect() const { return mrect; }
        Position pos() const { return mrect.pos(); }
        int pos_x() const { return mrect.x(); }
        int pos_y() const { return mrect.y(); }
        int left() const { return mrect.x(); }
        int top() const { return mrect.y(); }
        int right() const { return mrect.x() + mrect.w(); }
        int bottom() const { return mrect.y() + mrect.h(); }
        Size size() const { return mrect.size(); }
        int width() const { return mrect.w(); }
        int height() const { return mrect.h(); }

        /** Convenience function to return a Rect encompassing the whole widget in widget coordinates. */
        Rect size_rect() const { return Rect(0, 0, size()); }

        /** Resizes the widget. Reimplement if you want to change the size passed before applying.
         *  Reimplement resized() if you just want to be notified about the size having been changed.
         *  To enable the (int, int) variant in a derived class that overrides set_size, use
         * `using %Widget::set_size;` alongside overriding the (Size) variant.
         *  When using layouts, you normally do not need to call this function yourself - apart from setting
         *  the size of the top widget in the hierarchy.
         *  @see resized
         */
        virtual void set_size(Size s);
        void set_size(int w, int h) { set_size(Size(w, h)); }

        void set_pos(Position p);
        void set_pos(int x, int y) { set_pos(Position(x, y)); }

        void set_rect(Position p, Size s);
        void set_rect(const lgui::Rect& rect);
        void set_rect(int x, int y, int w, int h);


        Widget* parent() { return mparent; }
        const Widget* parent() const { return mparent; }

        /** Retrieve the child at coordinates `x, y`. Reimplement for children. */
        virtual Widget* get_child_at(int x, int y);

        /** Return the rectangle children are supposed to reside in. Reimplement for children. */
        virtual Rect children_area() const { return Rect(); }

        /** Visits this widget and all widgets in the hierarchy below. Needs to be reimplemented by widgets
         *  with children so that all children are visited */
        virtual void visit_down(const std::function<void (Widget& w)>& f) { f(*this); }

        /** Test all parents, going up the hierarchy recursively, whether w is among them.
         *  Also tests widget itself, i.e. `(this==w)` will `return true`. */
        bool is_child_of_recursive(const Widget* w) const;

        /** Return bottom-most widget that is at pos `x,y`  in `w`. */
        static Widget* get_leaf_widget_at_recursive(Widget* w, int x, int y);

        /** Return the absolute position of the widget, going up the hierarchy. */
        Position get_absolute_position() const;

        /** Map a position to parent coordinates. */
        Position map_to_parent(Position rel_pos) const;

        /** Map a position to widget from parent pos. */
        Position map_from_parent(Position parent_pos) const;

        /** Return true when the passed position (in parent coordinates) is considered inside the widget. */
        bool is_inside(Position parent_pos) const;

        /** Return the rectangle the widget occupies in absolute coordinates. This might be an empty rectangle
         *  if the widget is not visible. */
        Rect get_absolute_rect() const;

        /** Returns whether the position is inside the irregular shape of this widget. This is only called for extended
         *  checks if the flag IrregularShape is set. Note the widget rectangle is checked first anyway. */
        virtual bool is_inside_irregular_shape(Position pos) const { (void) pos; return true; }

        /** Measure the widget according to the given size constraints. This method is used by the
         *  layout system during its first pass. A widget with children should take their sizes into
         *  account when measuring. A widget must never return a size that does not satisfy
         *  the given constraints. Since these may be below the widget's minimal size, widgets
         *  should also reimplement min_size_hint() to allow layouts to query their minimum size.
         */
        MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override
        { return force_size_constraints(size(), wc, hc); } // FIXME: sensible default implementation?

        /** Return a sensible minimum size for the widget.
         *  You should be aware of the fact that there are cases for which this method
         *  will not be able to return sensible values (for both dimensions), since the height
         *  could depend on the width for example (as with a word-wrapped text or a FlowLayout).
         * */
        Size min_size_hint() override
        { return Size(); } // FIXME: sensible default implementation?

        /** Lay out the widget. This method is used during the second pass of the layout system.
         *  The default implementation just changes the rectangle.
         *  Be sure to call post_layout() when you reimplement this. */
        void layout(const Rect& r) override;

        LayoutElementType layout_element_type() const final { return LayoutElementWidget; }

        /** Request a layout process. Call this whenever measure() or min_size_hint() would return different
         *  values than before. The actual layout process is deferred. This method will go up in the hierarchy
         *  until it finds the top widget or a widget having a parent with no layout, which it will register
         *  with the GUI for deferred relayouting, setting Flags::NeedsRelayout on the way up. It will return
         *  immediately when the flag is already set. Apart from that, it will also never trigger a layout process when
         *  one is currently in progress. */
        void request_layout();

        /** Actually trigger a layout process. You shouldn't call this directly. Use request_layout()
         *  to request a layout process instead. This will be called by GUI,  especially on top widgets.
         *  The size of the widget is understood to remain fixed, i.e. it serves as a constraint for its
         *  children. */
        void _relayout();

        /** Returns true when the widget has a layout. */
        virtual bool has_layout() const { return false; }

        /** Shortcut for set_size(min_size_hint()). */
        void set_min_size() { set_size(min_size_hint()); }

        /** Shortcut to measure the widget against a given max size in both dimensions. */
        MeasureResults measure_max(const Size& s) {
            return measure(SizeConstraint(s.w(), SizeConstraintMode::Maximum),
                           SizeConstraint(s.h(), SizeConstraintMode::Maximum));
        }


        /** Request focus for the widget.
         *  @return whether acquiring the focus has been successful */
        bool focus();

        bool has_focus() const;
        bool has_modal_focus() const;

        /** Return true when the widget *itself* or any of its children are currently focused. */
        bool is_focus_among_children() const;

        /** Register a widget listener with the widget. */
        void add_widget_listener(IWidgetListener *listener);

        /** Remove a widget listener from the widget. */
        void remove_widget_listener(IWidgetListener *listener);


        /** Return the opacity of the widget. */
        float opacity() const { return mopacity; }

        /** Change the widget's opacity. */
        virtual void set_opacity(float opacity) {
            mopacity = opacity;
        }

        /** Change the style of the widget. This will (usually) also recursively change the
         *  style of its children. */
        void set_style(const Style* style);
        const Style& style() const;

        /** Change the default style for all instances of the Widget class. */
        static void set_default_style(Style* style);

        /** Retrieve the current font of the widget. This is a property of the widget, not the style. Will
         *  return the style's default font if no font has explictly been set on the widget. The idea is to
         *  always provide a valid font. */
        const Font& font() const;

        /** Set a font for the widget. Contrary to the style-property, this is only a property of the widget
         *  itself, not affecting its children. Pass nullptr to cease using a special font: the widget will
         *  revert to using the style's font again. */
        virtual void set_font(const Font* font) {
            mfont = font;
            request_layout();
        }


        /** Install an EventFilter on the widget. You can only register one event filter per widget. The
         *  filter will be informed about any event that is going to be sent to the widget, before it is being
         *  sent. */
        void set_event_filter(EventFilter* filter) { mfilter = filter; }

        /** Return the event filter of the widget. */
        EventFilter* event_filter() {
            if(mfilter) return mfilter;
            return mdefault_filter;
        }

        /** Set a default event filter for all instances of the Widget class. Mainly intended for
         *  debugging purposes. */
        static void set_default_event_filter(EventFilter* filter)
        { mdefault_filter = filter; }

        /** Return the default event filter of the widget class. */
        static EventFilter* get_default_event_filter() { return mdefault_filter; }


        /** Return `true` if widget has been added to a GUI. That means the widget is part of a valid
         *  widget hierarchy whose top-widget has been pushed to an GUI. */
        bool is_added_to_gui() const { return mgui != nullptr; }

        /** Return `true` if widget is added to an GUI as a top-level widget. */
        bool is_top_widget() const { return is_added_to_gui() && mparent == nullptr; }

        /** Return the top widget of the GUI hierarchy the widget is part of.
         *  Note: If the widget is not added to a GUI, it will return nullptr. */
        Widget* top_widget();

        /** Return the (child) widget that will receive focus when the widget is added to the GUI as a
         *  top-level widget. */
        Widget* focus_child() { return mfocus_child; }


        /** Bring the widget to front (regarding its siblings). This is usually deferred. */
        void bring_to_front();
        /** Send the widget to the back (regarding its siblings). This is usually deferred. */
        void send_to_back();

        /** After a widget has changed its size/position/visibility, you can call this on any widget added to
         *  a GUI to make the GUI update its under-mouse-queue. It merely sets a flag and does the
         *  work later, so there's no penalty for calling it multiple times during one update. This does have
         *  an overhead, however, so don't call for every mouse move event. */
        void invalidate_under_mouse();

        /** Shortcut for calling GUI::pop_top_widget(). Be careful!
         * FIXME: change name &  behavior?*/
        void close_popup();

        /** Send a key event to the widget. */
        bool send_key_event(KeyEvent& event);
        /** Send a drag&  drop event to the widget. */
        bool send_dragdrop_event(DragDropEvent& event);
        /** Send a mouse event to the widget. */
        bool send_mouse_event(MouseEvent& event);
        /** Send a focus event to the widget. */
        void send_focus_event(FocusEvent& event);

     private:
        /** Flags indicating widget state. Per default, none of these flags are set. */
        enum Flags {
            Inactive = 1,
             /**< when `Inactive` is set, widgets will receive no input events
                  (but remain visible) */
            _Visibility1 = 2,
            /**< Used to encode visibility */
            _Visibility2 = 4,
            /**< Used to encode visibility */
            ClipMe = 8,
            /**< widgets will only be clipped to their rectangle when `ClipMe` is set */
            ChildOutsideChildrenArea = 0x10,
            /**< set this flag for children outside the usual children area */
            Focusable = 0x20,
            /**< to be set if widget wants to be able to receive focus */
            Closed = 0x40,
            /**< `Closed` will be set by the GUI when a top-widget is popped and
                 cleared when it is pushed */
            HasStrongStyle = 0x80,
            /**< if set, the widget's style won't be overriden by the parent's style
             *   when set_style is called by the parent */
            MayTabInto = 0x100,
            /**< if set, widget can be tabbed into -- only valid if widget
                 is focusable and tab key not handled by other widgets */
            MayTabOutOf = 0x200,
            /**< if set, widget can be tabbed out of -- only valid if widget
                 is focusable and tab key not handled by other widgets */
            Hovered = 0x400,
            /**< if set, widget is hovered; the only flag that will be set/unset
                 automatically by the GUI */
            NeedsRelayout = 0x800,
            /**< set on widgets that have requested layout */
            LayoutInProgress = 0x1000,
            /**< set on widgets while layout is in progress */
            ReceiveTimerTicks = 0x2000,
            /**< set on widgets that want to subscribe to timer ticks */
            SuppressLayoutScheduling = 0x4000,
            /**< if you do not want your widget to schedule a layout process if it is at the top of a
             *   layout-hierarchy, you may set this flag to suppress it */
            IrregularShape = 0x8000
            /**< set to enable an in-depth shape check. If not set (default), widgets are assumed to be
             *   of rectangular shape. If set, the method is_inside_irregular_shape() will be used to determine
             *   whether e.g. the mouse position is "inside" the widget. As it may be called very often, the method
             *   should be rather fast. Note that it is only ever called for coordinates inside the widget rectangle. */
        };
    public:

        /** The visibility state of a widget. The difference between Invisible and Gone is that
            a gone widget does not occupy any space while an invisible one still does. */
        enum Visibility {
            /** Widget is visible. */
            Visible = 0,
            /** Widget is invisible, but will still occupy space in a layout. */
            Invisible = Flags::_Visibility1,
            /** Widget is invisible and will not occupy space in a layout. */
            Gone = Flags::_Visibility1 | Flags::_Visibility2
        };

        /** Return the visibility state of a widget. @see Visibility */
        Visibility visibility() const { return Visibility(mflags&  Gone); }

        /** Return whether the widget will receive events. */
        bool is_active() const { return !is_flag_set(Flags::Inactive); }

        /** Return whether the widget will receive events. */
        bool is_inactive() const { return is_flag_set(Flags::Inactive); }

        /** Return whether the widget is visible. If it isn't, it can be invisible or "gone".
            This is a convenience method. */
        bool is_visible() const { return visibility() == Visible; }

        /** Return whether the widget will occupy any space in a layout. Being gone implies being
         *  invisible. This is a convenience method. */
        bool is_gone() const { return visibility() == Gone; }

        /** Return whether the widget will be drawn with clipping enabled. */
        bool is_clipped() const { return is_flag_set(Flags::ClipMe); }

        /** Return whether the widget is located outside its parent's children
         *  area. I.e. its position is relative to its parent position, not the position
         *  of the parent's children area. */
        bool is_outside_children_area() const { return is_flag_set(Flags::ChildOutsideChildrenArea); }

        /** Return whether the widget is able to receive focus. */
        bool is_focusable() const { return is_flag_set(Flags::Focusable); }

        /** Return whether the widget is closed. This is set if a top-widget is popped from the GUI
            and cleared when it is pushed. */
        bool is_closed() const { return is_flag_set(Flags::Closed); }

        /** Return whether the widget's style setting will remain in place when the style of the parent changes. */
        bool has_strong_style() const { return is_flag_set(Flags::HasStrongStyle); }

        /** Return whether the widget can receive focus when focus is moved with the tab key. */
        bool may_tab_into() const { return is_flag_set(Flags::MayTabInto); }

        /** Return whether the widget can lose focus when focus is moved with the tab key. */
        bool may_tab_out_of() const { return is_flag_set(Flags::MayTabOutOf); }

        /** Return whether the mouse is currently on top of the widget.
            Changed automatically by the GUI. */
        bool is_hovered() const { return is_flag_set(Flags::Hovered); }

        /** Return whether the widget needs be laid out (again). */
        bool needs_relayout() const { return is_flag_set(Flags::NeedsRelayout); }

        /** Return whether the widget is currently being laid out. */
        bool layout_in_progress() const { return is_flag_set(Flags::LayoutInProgress); }

        /** Return whether the widget is currently receiving timer ticks. The widget will only start to receive
         *  timer ticks after it has been added to a GUI. */
        bool receives_timer_ticks() const { return is_flag_set(Flags::ReceiveTimerTicks); }

        /** Return whether layout scheduling is currently suppressed. If set to `true`, the widget will not
         *  call into GUI to schedule a layout process if it finds itself at the top of a layout hierarchy. */
        bool is_layout_scheduling_suppressed() const { return is_flag_set(Flags::SuppressLayoutScheduling); }

        /** Return whether widget has an irregular shape necessitating in-depth contains check for mouse and drag-drop
         * handling. */
        bool is_irregular_shape() const { return is_flag_set(Flags::IrregularShape); }

        /** Change whether the widget shall receive input events. */
        void set_active(bool active);

        /** Change the visibility of the widget. @see Visibility.*/
        void set_visibility(Visibility v);

        /** Convenience method to sets the widget's visibility to "gone". */
        void set_gone() { set_visibility(Gone); }

        /** Convenience method to sets the widget's visibility to "invisible". */
        void set_invisible() { set_visibility(Invisible); }

        /** Convenience method to sets the widget's visibility to "visible". */
        void set_visible() { set_visibility(Visible); }

        /** Convenience method to set the widget's visibility to either visible or invisible. */
        void set_invisible(bool inv) { set_visibility(inv ? Invisible : Visible); }

        /** Convenience method to set the widget's visibility to either visible or gone. */
        void set_gone(bool gone) { set_visibility(gone ? Gone : Visible); }

        /** Change whether the widget shall be drawn with clipping enabled. */
        void set_clipped(bool clipme) {
            set_unset_flag(Flags::ClipMe, clipme);
        }

        /** Change whether the widget should be considered to be located outside its parent's children area.
         * Its position is then relative to its parent's position, not the position of the children area of
         * its parent. */
        void set_outside_children_area(bool outside) {
            set_unset_flag(Flags::ChildOutsideChildrenArea, outside);
        }

        /** Change whether the widget shall be able to receive focus. */
        void set_focusable(bool focusable) {
            set_unset_flag(Flags::Focusable, focusable);
        }

        /** Change whether the widget is considered closed. Set by the GUI when a top-widget is popped,
         *  cleared when it is pushed.
         *
         *  @see GUI::push_top_widget(), GUI::pop_top_widget() */
        void set_closed(bool closed) {
            set_unset_flag(Flags::Closed, closed);
        }

        /** Change whether the widget's (custom) style shall remain in place even if the style of its parent
         *  is changed. */
        void set_strong_style(bool strong_s) {
            set_unset_flag(Flags::HasStrongStyle, strong_s);
        }

        /** Change whether the widget may receive focus via the tab key. */
        void set_may_tab_into(bool mti) {
            set_unset_flag(Flags::MayTabInto, mti);
        }

        /** Change whether the widget may lose focus via the tab key. */
        void set_may_tab_out_of(bool mto) {
            set_unset_flag(Flags::MayTabOutOf, mto);
        }

        /** This is called by the GUI to facilitate keeping track of being hovered. */
        void set_hovered(bool hovered) {
            set_unset_flag(Flags::Hovered, hovered);
        }

        /** This is set to indicate the widget needs to be laid out. You normally don't need to call this
         * directly, use request_layout() instead. */
        void set_need_relayout(bool nrl) {
            set_unset_flag(Flags::NeedsRelayout, nrl);
        }

        /** This is/should be set while the widget is being laid out. */
        void set_layout_in_progress(bool lip) {
            set_unset_flag(Flags::LayoutInProgress, lip);
        }

        /** Set to suppress layout scheduling from this widget if it finds itself at the top of a layout
         *  hierarchy.
         *  Only set this if you want to manage layout yourself or you do not want the layout process to be
         *  scheduled automatically. You can still schedule a layout process by yourself in response to the
         *  needs_relayout() flag being set. Calls to request_layout will still be forwarded up the layout
         *  hierarchy (so needs_relayout() will be set appropriately), the widget will just cease to call into
         *  GUI to schedule a layout process with itself as the layout root. */
        void set_suppress_layout_scheduling(bool sls) {
            set_unset_flag(Flags::SuppressLayoutScheduling, sls);
        }

        void set_irregular_shape(bool ir) {
            set_unset_flag(Flags::IrregularShape, ir);
        }

        /** Change whether the widget shall receive timer ticks via timer_ticked(). */
        void set_receive_timer_ticks(bool rtt);

        /** Set how many timer ticks the widget should skip (rather: that the widget should receive every
         *  `x`'th timer tick). Do not forget to enable receiving timer ticks for the widget.
         *  Example: `3` means the widget will only receive every third tick (`2` are skipped).
         *  Note: `skip_mod` > 0 should apply
         **/
        void set_timer_tick_skip_mod(int skip_mod);

        /** Return how many timer ticks the widget will skip if receiving timer ticks is enabled. */
        int timer_tick_skip_mod() const { return mtimer_skip_ticks_mod; }

    protected:
        struct ConfigInfo; // forward declaration
    public:
        /** Configure a widget and its children when it's added to/removed from the GUI. Consider the
         *  ConfigInfo struct to be opaque. */
         void _recursive_configure(const ConfigInfo& ci) {
             visit_down([&ci](Widget& w) { w._configure(ci); });
         }
    protected:
        /** Configure a widget according to ConfigInfo. You normally do not need to call this directly. */
        void _configure(const ConfigInfo& ci);

        /** Call this when a new child is added to a widget. This will set up some behind-the-scenes stuff
         *  via Widget::_recursive_configure() and sets the widget as the parent of the child. */
        void configure_new_child(Widget& child);

        /** Use this when a child is going to be removed from a widget. This will clear up some
         *  behind-the-scenes stuff and sets the parent of the child to `nullptr`. */
        void configure_child_to_be_removed(Widget& child);

        /** Called in every child's destructor. Reimplement this for widgets having children.*/
        virtual void child_about_to_die(Widget& child);

        /** Called after size has been changed, but before widget listeners get to know. */
        virtual void resized(const Size& old_size) {(void) old_size;}

        /** Called after layout has run for a widget. */
        virtual void post_layout() {}

        /** Called whenever the style changed. Should be used to forward style changes to children. The
         *  `HasStrongStyle` flag should be honored when using it for that purpose. */
        virtual void style_changed() {
            request_layout();
        }

        /** Called on all widgets in a hierarchy whenever that hierarchy is newly added to a GUI.
         *  Can be used for initialization. Overriding functions should call the default implementation unless
         *  there are good reasons not to do so. The default implementation ensures that even layouts for
         *  widgets whose parents don't have layouts themselves run once the hierarchy is inserted:
         *  A BasicContainer without layout will call _relayout() on all of its children that have the flag
         *  NeedsRelayout set.
         */
        virtual void added_to_gui()
        {
            set_need_relayout(true);
        }

        /** Registers a deferred action to be called after other deferred actions (such as layout, bringing
         *  widgets to the front / back) have been processed. Will have no effect if widget is not added to
         *  a GUI. */
        void defer(const std::function<void ()>& callback);

        /** Requests modal focus for the widget.
         * @return whether request has been successful.  */
        bool request_modal_focus();
        /** Release the modal focus held by the widget. */
        bool release_modal_focus();

        /** Request that `w` shall become the modal widget. That means w will receive every input event and
         *  will be drawn on top of all other widgets.
         *  @return whether the request has been successful. */
        bool request_modal_widget(Widget& w);
        /** Tell `w` to cease being the modal widget. */
        bool release_modal_widget(Widget& w);

        /** Utility function to draw a widget. Will push and pop appropriate rects and call c's draw method.
        */
        static void draw_child(const Widget& c, const DrawEvent& parent_de);

        /** Sets the child to focus when the widget is introduced as a new top-widget.
         * This is used (i.e. overwritten) by the GUI when the widget is sent to the background by another
         * top-widget, so it remembers which child to focus when the widget becomes active again. You can use
         * it to specify which child will receive focus the first time the widget pops up, e.g. call it in a
         * constructor. ´nullptr´ and ´this´ will have the same effect.
         * The GUI won't use this value without checking whether the child is still valid.
         * The widget itself doesn't have to be focusable if the property refers to another widget. */
        void set_focus_child(Widget* child) { mfocus_child = child; }

        /** Emits a size changed event to widget listeners. */
        void _emit_size_changed();

        /** Emits a position changed event to widget listeners. */
        void _emit_pos_changed();

        /** Emits a visibility changed event to widget listeners. */
        void _emit_visibility_changed(bool gone_changed);

        /** Emits a child added event to widget listeners. */
        void _emit_child_added(Widget& child);

        /** Emits a child removed devent to widget listeners. */
        void _emit_child_removed(Widget& child);

        /** Internal. */
        struct ConfigInfo {
            dtl::FocusManager *focus_mngr;
            GUI *gui;
        };

        virtual void _bring_child_to_front(Widget& child) {(void) child; }
        virtual void _send_child_to_back(Widget& child) {(void) child; }

    private:
        void set_focus_manager(dtl::FocusManager* focus_mngr);

        bool is_flag_set(Flags flag) const { return mflags & flag; }
        void set_unset_flag(Flags flag, bool unset_set) {
            if(unset_set)
                mflags |= flag;
            else
                mflags &= ~flag;
        }

        Rect mrect;
        uint32_t mflags;
        Widget* mparent;
        dtl::FocusManager* mfocus_manager;
        GUI* mgui;
        std::forward_list <IWidgetListener*> mwidget_listeners;
        EventFilter* mfilter;
        Widget* mfocus_child;
        const Style* mstyle;
        const Font* mfont;
        float mopacity;
        int mtimer_skip_ticks_mod;

        static EventFilter* mdefault_filter;
        static const Style* mdefault_style;
};

}

#endif // LGUI_WIDGET_H
