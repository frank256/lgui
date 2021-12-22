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

#ifndef LGUI_ANIMATIONFACILITIES_H
#define LGUI_ANIMATIONFACILITIES_H

#include "animationcontext.h"
#include "animationbuilder.h"

namespace lgui {

/** Wrapper class to create animations which are placed into an AnimationContext. Its methods create builders for
 *  creating animations. Each widget can access the GUI's main instance via Widget::animate(). */
class AnimationFacilities {
    public:

        /** C'tor. Needs a reference to an animation context where the builders will place their products. */
        explicit AnimationFacilities(AnimationContext& context)
                : mcontext(context) {}

        /** Create a builder for a ValueAnimation for animating a value of type T. */
        template<typename T>
        ValueAnimationBuilderWithContext <T> animate_value() {
            return ValueAnimationBuilderWithContext<T>(mcontext);
        }

        /** Create a builder for animating a widget transform.
            Use this if you need to modify several transformation properties at once. */
        TransformationAnimationBuilderWithContext animate_transform() {
            return TransformationAnimationBuilderWithContext(mcontext);
        }

        /** Create a builder wrapping several animations that shall be played simultaneously.
            Use lgui::animate_value(), lgui::animate_transform(), lgui::sequence() to construct
            the individual animations. */
        template<typename... Args>
        SimultaneousAnimationsBuilderWithContext simultaneous(Args&& ... args) {
            static_assert(sizeof...(args) > 0, "Please pass arguments to simultaneous().");
            return SimultaneousAnimationsBuilderWithContext(mcontext, std::forward<Args>(args)...);
        }

        /** Create a builder wrapping several animations that shall be played one after the other.
            Use lgui::animate_value(), lgui::animate_transform(), lgui::simultaneous() to construct
            the individual animations. */
        template<typename... Args>
        AnimationSequenceBuilderWithContext sequence(Args&& ... args) {
            static_assert(sizeof...(args) > 0, "Please pass arguments to sequence().");
            return AnimationSequenceBuilderWithContext(mcontext, std::forward<Args>(args)...);
        }

    private:
        AnimationContext& mcontext;
};

}

#endif //LGUI_ANIMATIONFACILITIES_H
