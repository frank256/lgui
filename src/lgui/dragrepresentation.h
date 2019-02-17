/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-18 frank256
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

#ifndef LGUI_DRAGREPRESENTATION_H
#define LGUI_DRAGREPRESENTATION_H

#include "lgui.h"
#include <string>



namespace lgui {
    class Bitmap;
    class Graphics;
}

namespace lgui {

    namespace dtl { class MouseTrackHelper; class EventHandler; class DragDropTrackHelper; }

    class Widget;
    /** Graphically represents an ongoing drag operation. Pass a new object of this type to
     *  MouseEvent::spawn_drag(). Customize via subclassing. You should set a proper size for this in the
     *  constructor. Resizing is currently not allowed. */
    class DragRepresentation
    {
        friend class dtl::EventHandler;
        friend class dtl::DragDropTrackHelper;
        public:
            enum RequestedSrcAction { None, Remove };

            /** Constructs a drag representation. `src_widget` is mandatory.
             *  `hotspot` is the point under which the drag representation will be glued under the mouse.
             *  Use `content_descr` to identify the contents - not every widget will want to receive every
             *  drag. Don't forget to set a proper size via set_size(). */
            DragRepresentation(Widget& src_widget, const Position& hotspot,
                               const std::string& content_descr);
            virtual ~DragRepresentation();

            /** Draws the DragRepresentation. Will be clipped to the dimensions set via set_size. */
            virtual void draw(Graphics& gfx) const;

            Widget* source_widget() { return msrc_widget; }
            Widget* target_widget() { return mtarget_widget; }

            bool has_target_been_reached() const { return mtarget_widget != nullptr; }

            RequestedSrcAction requested_src_action() const { return mrequested_src_action; }

            /** Can be used to tell source widget what to do. The source widget has the opportunity to query
             *  this when it receives the drag_ended event. */
            void set_requested_src_action(RequestedSrcAction action)
                        { mrequested_src_action = action; }

            const Position& hotspot() const { return mhotspot; }
            const std::string& content_descr() const { return mcontent_descr; }

            const Rect& rect() const { return mrect; }
            Size size() const { return mrect.size(); }
            Position pos() const { return mrect.pos(); }

            void *data() { return mdata; }
            void set_data(void *data) { mdata = data; }

        protected:
            /** To be called once within the constructor. Resizing later is currently
             *  not supported. The %DragRepresentation will be clipped to this size when drawn. */
            void set_size(const Size& s);

        private:
            // for moving and initializing, called by our friend EventHandler
            void _set_pos(const Position& pos) { mrect.set_pos(pos); }
            void _set_target_widget(Widget* w) { mtarget_widget = w; }
            void _clear_source_widget() { msrc_widget = nullptr; }

            Widget* msrc_widget, *mtarget_widget;
            Rect mrect;
            Position mhotspot;
            RequestedSrcAction mrequested_src_action;
            std::string mcontent_descr;
            void *mdata;
    };

    /** Provide a simple class that just draws an image.*/
    class ImageDragRepresentation : public DragRepresentation
    {
        public:
            ImageDragRepresentation(Widget& src_widget, const Position& hotspot,
                               const std::string& content_descr, const Bitmap& bmp);
            void draw(Graphics& gfx) const override;
        private:
            const Bitmap& mbmp;
    };

}

#endif // LGUI_DRAGREPRESENTATION_H
