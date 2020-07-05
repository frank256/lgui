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

#ifndef LGUI_CONTAINER_H
#define LGUI_CONTAINER_H

#include "lgui/basiccontainer.h"

namespace lgui {

    /** This class provides padding on top of a BasicContainer. Changing the  padding will not resize the
     *  container, but only alter the children area. Contrary to Container, it won't draw a background and
     *  will not alter its padding through styles.
     */
    class PaddedContainer : public BasicContainer {
        public:
            PaddedContainer();

            const Padding& padding() const { return mpadding; }

            /** Adjust the padding. This will not resize the container,
             *  but rather the children area accordingly. */
            void set_padding(const Padding& padding);

            MeasureResults measure(SizeConstraint wc, SizeConstraint hc) override;
            Size min_size_hint() override;

        protected:
            void resized(const Size& old_size) override;

            Size get_children_area_size_for_size(Size size) override;

        private:
            void update_children_area();

            Padding mpadding;
    };

    /** A container class providing a drawn background and ability to set padding.
     *  The container will get its padding from the style per default.
     */
    class Container : public PaddedContainer
    {
        public:
            Container();

        protected:
            void style_changed() override;

            void draw_background(const DrawEvent& de) const override;
    };

}

#endif // LGUI_CONTAINER_H
