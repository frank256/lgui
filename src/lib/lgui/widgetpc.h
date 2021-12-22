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
class WidgetPC : public lgui::Widget {
    public:
        ~WidgetPC() override;

        void visit_down(const std::function<void(Widget&)>& f) override;
        Widget* get_child_at(PointF p) override;

    protected:
        void style_changed() override;
        void child_about_to_die(Widget& child) override;

        void draw_private_children(const DrawEvent& de,
                                   bool do_ca_pass = true, bool clip_ca = false) const;
        void add_private_child(Widget& widget);
        void remove_private_child(Widget& widget);
        void clear_private_children();

        void _bring_child_to_front(Widget& child) override;
        void _send_child_to_back(Widget& child) override;

    private:
        using container_t = std::list<Widget*>;
        container_t mprivate_children;
};

}

#endif // LGUI_WIDGETPC_H
