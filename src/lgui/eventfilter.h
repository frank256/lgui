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

#ifndef LGUI_EVENTFILTER_H
#define LGUI_EVENTFILTER_H


namespace lgui {

    class Widget;
    class MouseEvent;
    class KeyEvent;
    class DragDropEvent;
    class FocusEvent;

    /** An event-filter is an object that can be set on a widget in order to inspect and/or filter the events
     *  it is going to receive. The filter will be called with all events *before* the widget receives them.
     *  The widget will only receive the event if the filter's method returns true. The default implementation
     *  doesn't filter anything.
     *
     *  You can only set one event-filter for every widget.
     *
     *  There's also the possibility to set one default- (or rather, fallback-) filter for all widgets; this
     *  is mainly intended for debugging/logging, not for filtering).
     *
     *  @see Widget::set_event_filter(), Widget::set_default_filter() */
    class EventFilter {
        public:
            EventFilter() = default;
            virtual ~EventFilter() = default;

            virtual bool mouse_event(Widget& w, MouseEvent& event);
            virtual bool key_event(Widget& w, KeyEvent& event);

            virtual bool focus_event(Widget& w, FocusEvent& event);

            virtual bool dragdrop_event(Widget& w, DragDropEvent& event);
    };

    /** Class to dump all events a widget receives. Uses RTTI at the moment. */
    class DebugEventFilter : public EventFilter
    {
        public:
            DebugEventFilter()
                : mdont_dump_move(false) {}

            bool mouse_event(Widget& w, MouseEvent& event) override;
            bool key_event(Widget& w, KeyEvent& event) override;

            bool focus_event(Widget& w, FocusEvent& event) override;

            bool dragdrop_event(Widget& w, DragDropEvent& event) override;
            /** Can be used to prevent mouse move events to be dumped. Also implies
             *  mouse dragged events. */
            void set_dump_move(bool dump_move) { mdont_dump_move = !dump_move; }
        private:
            bool mdont_dump_move;

    };

}

#endif // LGUI_EVENTFILTER_H
