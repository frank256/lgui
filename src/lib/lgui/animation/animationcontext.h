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

#ifndef LGUI_ANIMATIONCONTEXT_H
#define LGUI_ANIMATIONCONTEXT_H

#include <memory>

#include "lgui/vector_utils.h"
#include "lgui/platform/error.h"

#include "ianimation.h"

namespace lgui {

class AnimationContext {
    public:
        ~AnimationContext() {
            clear();
        }

        void take(std::unique_ptr<IAnimation>&& animation) {
            ASSERT(animation);
            manimations.emplace_back(std::move(animation));
        }

        void clear() {
            for (auto& ani : manimations) {
                if (ani) {
                    bool can_delete = ani->can_delete();
//                    printf("%p can delete?: %d\n", ani.get(), can_delete);
                    if (can_delete) {
                        ani = nullptr;
                    }
                }
            }
            erase_remove_if(manimations, [](auto& ptr) -> bool { return ptr == nullptr; });
        }

        void test_print() {
//            int i = 1;
//            for (auto& ani : manimations) {
//                bool is_compo = dynamic_cast<AnimationComposition*>(&*ani);
//                printf("%d. %p (%s): is_playing: %d, can be deleted: %d\n", i, &*ani, is_compo ? "composition" : "animation", ani->is_playing(), ani->can_delete());
//                ++i;
//            }
        }

    private:
        std::vector<std::unique_ptr<IAnimation>> manimations;
};

}

#endif //LGUI_ANIMATIONCONTEXT_H
