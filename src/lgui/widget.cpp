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

#include "widget.h"
#include "lgui/internal/focusmanager.h"
#include "lgui/platform/error.h"
#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"
#include "lgui/style/style.h"
#include "gui.h"
#include "eventfilter.h"
#include "focusevent.h"
#include "iwidgetlistener.h"

namespace lgui {

    const Style* Widget::mdefault_style = nullptr;
    EventFilter* Widget::mdefault_filter = nullptr;

    Widget::Widget()
        : mflags(0), mparent(nullptr), mfocus_manager(nullptr),
          mgui(nullptr), mfilter(nullptr), mfocus_child(nullptr),
          mstyle(nullptr), mfont(nullptr), mopacity(1.0f), mtimer_skip_ticks_mod(1)
    {
    }

    Widget::~Widget()
    {
        if(mparent)
            mparent->child_about_to_die(*this);
        if(mfocus_manager)
            set_focus_manager(nullptr); // Unregister from focus manager (remove focus if necessary).
        if(mgui)
            mgui->_handle_widget_deregistered(*this, true);
    }

    void Widget::draw(const DrawEvent& de) const
    {
        (void) de;
    }

    void Widget::draw_child(const Widget& c, const DrawEvent& parent_de)
    {
        parent_de.gfx().push_draw_area(c.rect(), c.is_clipped());
        c.draw(DrawEvent(parent_de.gfx(), c.is_inactive() || parent_de.draw_inactive(), c.opacity() * parent_de.opacity()));
        parent_de.gfx().pop_draw_area();
    }

    void Widget::set_size(Size s)
    {
        lgui::Size old_size = mrect.size();
        mrect.set_size(s);
        resized(old_size);
        _emit_size_changed();
    }

    void Widget::set_pos(Position p)
    {
        mrect.set_pos(p);
        _emit_pos_changed();
    }

    void Widget::set_rect(Position p, Size s)
    {
        set_size(s);
        set_pos(p);
    }

    void Widget::set_rect(const Rect& rect)
    {
        set_size(rect.size());
        set_pos(rect.pos());
    }

    void Widget::set_rect(int x, int y, int w, int h)
    {
        set_size(w, h);
        set_pos(x, y);
    }

    void Widget::layout(const Rect& r) {
        set_layout_in_progress(true);
        set_rect(r);
        post_layout();
        set_layout_in_progress(false);
        set_need_relayout(false);
    }

    void Widget::request_layout()
    {
        if(layout_in_progress())
            return;

        if (needs_relayout())
            return;

        set_need_relayout(true);

        if(!is_added_to_gui()) {
            // Widget::added_to_gui() will automatically set NeedsLayout on every Widget in a
            // hierarchy newly added to a GUI.
            return;
        }
        if(parent() && parent()->has_layout())
            parent()->request_layout();
        else {
            if (!is_layout_scheduling_suppressed())
                mgui->_request_layout(*this);
        }
    }

    void Widget::_relayout() {
        if(needs_relayout()) {
            // Remeasure
            measure(SizeConstraint(width(), SizeConstraintMode::Exactly),
                    SizeConstraint(height(), SizeConstraintMode::Exactly));
            layout(rect());
        }
    }

    void Widget::set_active(bool active) {
        if(active != is_active()) {
            set_unset_flag(Flags::Inactive, !active);
            if(!active) {
                if(mfocus_manager)
                    mfocus_manager->widget_became_inactive(*this);
                if(mgui)
                    mgui->_handle_widget_invisible_or_inactive(*this);
            }
        }
    }

    void Widget::set_visibility(Visibility v) {
        Visibility oldv = visibility();
        if(oldv != v) {
            bool gone_changed = (oldv == Gone);
            if(v == Invisible || v == Gone) {
                set_unset_flag(Flags::_Visibility1, true);
                if(v == Gone) {
                    gone_changed = true;
                    set_unset_flag(Flags::_Visibility2, true);
                    request_layout();
                }
                else
                    set_unset_flag(Flags::_Visibility2, false);
                if(mfocus_manager)
                    mfocus_manager->widget_became_invisible(*this);
                if(mgui)
                    mgui->_handle_widget_invisible_or_inactive(*this);
            }
            else {
                set_unset_flag(_Visibility1, false);
                set_unset_flag(_Visibility2, false);
                if(gone_changed)
                    request_layout();
            }
            _emit_visibility_changed(gone_changed);
        }
    }

    bool Widget::focus()
    {
        ASSERT(mfocus_manager);
        return mfocus_manager->request_focus(*this);
    }

    bool Widget::has_focus() const
    {
        if(!mfocus_manager)
            return false;
        return mfocus_manager->is_focused(*this);
    }

    bool Widget::request_modal_focus()
    {
        ASSERT(mfocus_manager);
        return mfocus_manager->request_modal_focus(*this);
    }

    bool Widget::has_modal_focus() const
    {
        ASSERT(mfocus_manager);
        return mfocus_manager->has_modal_focus(*this);
    }

    bool Widget::is_focus_among_children() const
    {
        ASSERT(mfocus_manager);
        return mfocus_manager->is_parent_of_focus_widget(this);
    }

    bool Widget::release_modal_focus()
    {
        ASSERT(mfocus_manager);
        return mfocus_manager->release_modal_focus(*this);
    }

    void Widget::set_receive_timer_ticks(bool rtt) {
        if (rtt != receives_timer_ticks()) {
            set_unset_flag(Flags::ReceiveTimerTicks, rtt);
            if (is_added_to_gui()) {
                if (rtt)
                   mgui->_subscribe_to_timer_ticks(*this);
                else
                    mgui->_unsubscribe_from_timer_ticks(*this);
            }
        }
    }

    void Widget::set_timer_tick_skip_mod(int skip_mod) {
        ASSERT(skip_mod >  0);
        mtimer_skip_ticks_mod = skip_mod;
    }


    void Widget::defer(const std::function<void ()>& callback)
    {
        if (mgui) {
            mgui->_enqueue_deferred(callback);
        }
    }

    bool Widget::request_modal_widget(Widget& w)
    {
        if(!mgui)
            return false;
        return mgui->_request_modal_widget(w);
    }

    bool Widget::release_modal_widget(Widget& w)
    {
        if(!mgui)
            return false;
        return mgui->_release_modal_widget(w);
    }

    void Widget::configure_new_child(Widget& child)
    {
        child.mparent = this;
        ConfigInfo ci { this->mfocus_manager, this->mgui };
        child._recursive_configure(ci);
    }

    void Widget::configure_child_to_be_removed(Widget& child)
    {
        ConfigInfo ci { nullptr, nullptr };
        child._recursive_configure(ci);
        child.mparent = nullptr;
    }

    // no children, reimplement for children
    Widget* Widget::get_child_at(int x, int y)
    {
        (void) x; (void) y;
        return nullptr;
    }

    void Widget::child_about_to_die(Widget& child)
    {
        (void) child;
    }

    void Widget::invalidate_under_mouse()
    {
        if(mgui)
            mgui->set_under_mouse_invalid();
    }

    void Widget::close_popup()
    {
        if(mgui)
            mgui->pop_top_widget();
    }

    Widget* Widget::top_widget()
    {
        if(mgui)
            return mgui->top_widget();
        return nullptr;
    }

    void Widget::bring_to_front()
    {
        if(mgui) {
            mgui->_bring_to_front(*this);
        }
    }

    void Widget::send_to_back()
    {
        if(mgui) {
            mgui->_send_to_back(*this);
        }
    }

    void Widget::set_style(const Style* style)
    {
        if(mstyle != style) {
            mstyle = style;
            style_changed();
        }
    }

    const Style& Widget::style() const
    {
        if(mstyle)
            return *mstyle;
        else
            return *mdefault_style;
    }

    void Widget::set_default_style(Style* style)
    {
        mdefault_style = style;
        // TODO: notification?
    }

    const Font& Widget::font() const {
        if(mfont)
            return *mfont;
        else
            return style().default_font();
    }

    void Widget::set_focus_manager(dtl::FocusManager* focus_mngr)
    {
        if(mfocus_manager)
            mfocus_manager->remove(*this);
        mfocus_manager = focus_mngr;
        if(mfocus_manager)
            mfocus_manager->add(*this);
    }

    void Widget::_configure(const Widget::ConfigInfo& ci)
    {
        bool added = !mgui && ci.gui;
        set_focus_manager(ci.focus_mngr);
        if(mgui && mgui != ci.gui)
            mgui->_handle_widget_deregistered(*this);
        mgui = ci.gui;
        if (mgui && receives_timer_ticks())
            mgui->_subscribe_to_timer_ticks(*this);
        // every widget newly added to GUI needs a relayout
        if(added)
            added_to_gui();
    }

    void Widget::_emit_size_changed()
    {
        for(auto wl : mwidget_listeners)
            wl->size_changed_wl(*this);
    }

    void Widget::_emit_pos_changed()
    {
        for(auto wl : mwidget_listeners)
            wl->pos_changed_wl(*this);
    }

    void Widget::_emit_visibility_changed(bool gone_changed)
    {
        for(auto wl : mwidget_listeners)
            wl->visibility_changed_wl(*this, gone_changed);
    }


    void Widget::_emit_child_added(Widget& child)
    {
        for(auto wl : mwidget_listeners)
            wl->child_added_wl(*this, child);
    }

    void Widget::_emit_child_removed(Widget& child)
    {
        for(auto wl : mwidget_listeners)
            wl->child_removed_wl(*this, child);
    }

    void Widget::add_widget_listener(IWidgetListener* listener)
    {
        // take precautions against double-add
        bool found = false;
        for(IWidgetListener* lis_p : mwidget_listeners) {
            if(lis_p == listener) {
                found = true;
                break;
            }
        }
        if(!found)
            mwidget_listeners.push_front(listener);
    }

    void Widget::remove_widget_listener(IWidgetListener* listener)
    {
        mwidget_listeners.remove(listener);
    }

    bool Widget::is_child_of_recursive(const Widget* w) const
    {
        if(w == nullptr)
            return false;
        if(w == this)
            return true;
        if(mparent) {
            if(w == mparent)
                return true;
            if(mparent->is_child_of_recursive(w))
                return true;
        }
        return false;
    }

    Widget* Widget::get_leaf_widget_at_recursive(Widget* w, int x, int y)
    {
        if(w->is_active() && w->is_visible() && w->size_rect().contains(x, y)) {
            Widget* c = w->get_child_at(x, y);
            if(c) {
                int cx = x - c->pos_x(),
                    cy = y - c->pos_y();
                if(!c->is_outside_children_area()) {
                    cx -= w->children_area().x();
                    cy -= w->children_area().y();
                }
                Widget* cc = get_leaf_widget_at_recursive(c, cx, cy);
                if(cc)
                    return cc;
                else
                    return c;
            }
            else
                return w;
        }
        return nullptr;
    }

    Position Widget::map_to_parent(Position rel_pos) const
    {
        rel_pos += pos();
        const Widget* parent = this->parent();
        if(parent && !is_outside_children_area())
            rel_pos += parent->children_area().pos();
        return rel_pos;
    }

    Position Widget::map_from_parent(Position parent_pos) const
    {
        Position pos = parent_pos - this->pos();
        const Widget* parent = this->parent();
        if(parent && !is_outside_children_area())
            pos -= parent->children_area().pos();
        return pos;
    }

    bool Widget::is_inside(Position parent_pos) const 
    {
        Position pos = map_from_parent(parent_pos);
        return size_rect().contains(pos) && (!is_irregular_shape() || is_inside_irregular_shape(pos));
    }

    Position Widget::get_absolute_position() const
    {
        const Widget* w = this;
        Position pos(0, 0);
        while(w != nullptr) {
            pos = w->map_to_parent(pos);
            w = w->parent();
        }
        return pos;
    }

    Rect Widget::get_absolute_rect() const
    {
        const Widget* w = this;
        ASSERT(w);
        Rect r = w->rect();
        r.set_pos(0, 0);
        while(w != nullptr) {
            const Widget* p = w->parent();
            //debug("\nRect: %d, %d, %d, %d", r.x(), r.y(), r.w(), r.h());
            r.set_pos(w->map_to_parent(r.pos()));
            //debug("\nafter map_to_parent: Rect: %d, %d, %d, %d", r.x(), r.y(), r.w(), r.h());
            if(p) {
                Rect pr;
                // FIXME: we assume we don't have to clip children area to widget's rect,
                //        widgets should get that right
                // clip to parent's children area, or if it's a child outside the children area, to parent.
                if(!w->is_outside_children_area())
                    pr = p->children_area();
                else
                    pr = p->size_rect();

                r.clip_to(pr);
            }
            //debug("\nafter clip: Rect: %d, %d, %d, %d", r.x(), r.y(), r.w(), r.h());
            w = p;
        }
        return r;
    }

    bool Widget::send_mouse_event(MouseEvent& event)
    {
        if(event_filter()) {
            if(!event_filter()->mouse_event(*this, event))
                return false;
        }
        switch(event.type()) {
            case MouseEvent::Moved:
                mouse_moved(event);
                break;
            case MouseEvent::Dragged:
                mouse_dragged(event);
                break;
            case MouseEvent::WheelUp:
                mouse_wheel_up(event);
                break;
            case MouseEvent::WheelDown:
                mouse_wheel_down(event);
                break;
            case MouseEvent::Entered:
                set_hovered(true);
                mouse_entered(event);
                break;
            case MouseEvent::Left:
                set_hovered(false);
                mouse_left(event);
                break;
            case MouseEvent::Pressed:
                mouse_pressed(event);
                break;
            case MouseEvent::Released:
                mouse_released(event);
                break;
            case MouseEvent::Clicked:
                mouse_clicked(event);
                break;
            default:
                // error
                break;
        }
        return event.has_been_consumed();
    }

    void Widget::send_focus_event(FocusEvent& event)
    {
        if(event_filter()) {
            if(!event_filter()->focus_event(*this, event))
                return;
        }
        switch(event.type()) {
            case FocusEvent::FocusGained:
                focus_gained(event);
                break;
            case FocusEvent::FocusLost:
                focus_lost(event);
                break;
            case FocusEvent::ModalFocusGained:
                modal_focus_gained(event);
                break;
            case FocusEvent::ModalFocusLost:
                modal_focus_lost(event);
                break;
            default:
                break; // error
        }
    }

    bool Widget::send_dragdrop_event(DragDropEvent& event)
    {
        if(event_filter()) {
            if(!event_filter()->dragdrop_event(*this, event))
                return false;
        }
        switch(event.type()) {
            case DragDropEvent::Moved:
                drag_moved(event);
                break;
            case DragDropEvent::Entered:
                drag_entered(event);
                break;
            case DragDropEvent::Left:
                drag_left(event);
                break;
            case DragDropEvent::DragEnded:
                drag_ended(event);
                break;
            case DragDropEvent::Dropped:
                dropped(event);
                break;
            default:
                break; // error
        }
        return event.has_been_consumed();
    }

    bool Widget::send_key_event(KeyEvent& event)
    {
        if(event_filter()) {
            if(!event_filter()->key_event(*this, event))
                return false;
        }
        switch(event.type()) {
            case KeyEvent::Pressed:
                key_pressed(event);
                break;
            case KeyEvent::Released:
                key_released(event);
                break;
            case KeyEvent::Char:
                key_char(event);
                break;
            default:
                // error
                break;
        }
        return event.has_been_consumed();
    }


}
