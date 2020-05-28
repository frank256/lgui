#ifndef LGUITEST_ANIMATIONCONTEXT_H
#define LGUITEST_ANIMATIONCONTEXT_H

#include "lgui/vector_utils.h"
#include <memory>

#include "animation.h"

namespace lgui {

class AnimationContext {
    public:

        void take(std::unique_ptr<IAnimation>&& animation) {
            manimations.emplace_back(std::move(animation));
        }

        void clear() {
            for (auto& ani : manimations) {
                if (!ani->is_playing())
                    ani = nullptr;
            }
            erase_remove_if(manimations, [](auto& ptr) -> bool { return ptr == nullptr; });
        }

    private:
        std::vector<std::unique_ptr<IAnimation>> manimations;
};

}

#endif //LGUITEST_ANIMATIONCONTEXT_H
