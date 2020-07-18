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
