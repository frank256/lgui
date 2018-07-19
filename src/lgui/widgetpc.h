#ifndef LGUI_WIDGETPC_H
#define LGUI_WIDGETPC_H

#include "widget.h"
#include <list>

namespace lgui {

    /** A convenience class to provide derived classes with functionality for having private children. This is
     *  merely intended for widgets that need helper widgets for their own internal use. Supports both
     *  children inside and outside of children area. You can use draw_private_children() to draw private
     *  children, but it might be faster to draw them yourself.You have to take care of layout and measuring
     *  yourself.
     */
    class WidgetPC : public lgui::Widget
    {
        public:
            ~WidgetPC() override;

            void _recursive_configure(const ConfigInfo& ci) override;
            Widget* get_child_at(int x, int y) override;

        protected:
            void style_changed() override;
            void child_about_to_die(Widget& child) override;

            void draw_private_children(const DrawEvent& de,
                                       bool do_ca_pass=true, bool clip_ca=false) const;
            void add_private_child(Widget& widget);
            void remove_private_child(Widget& widget);
            void clear_private_children();

            void _bring_child_to_front(Widget& child) override;
            void _send_child_to_back(Widget& child) override;

        private:
            using container_t = std::list <Widget*>;
            container_t mprivate_children;
    };

}

#endif // LGUI_WIDGETPC_H
