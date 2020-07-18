#ifndef LGUI_ANIMATIONSEQUENCE_H
#define LGUI_ANIMATIONSEQUENCE_H

#include "animation.h"

namespace lgui {

class AnimationSequence : public Animation, public AnimationListener {
    public:
        using Callback = std::function<void()>;

        AnimationSequence() = default;
        AnimationSequence(const AnimationSequence& other) = delete;

        void add(Animation* ani) {
            ani->set_animation_listener(this);
            mnodes.push_back(ani);
            mnodes.back()->set_index(int(mnodes.size()) - 1);
        }

        void add(std::unique_ptr<Animation>&& ani) {
            add(&*ani);
            mowned_animations.push_back(std::move(ani));
        }

        void start() override {
            if (!is_playing()) {
                Animation::start();
                mplaying_index = 0;
                if (!mnodes.empty()) {
                    mnodes.front()->start();
                }
            }
        }

        void end() override {
            if (is_playing()) {
                while (mplaying_index < mnodes.size()) {
                    mnodes[mplaying_index]->end();
                    ++mplaying_index;
                }
                Animation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                if (!mnodes.empty() && mplaying_index < mnodes.size()) {
                    mnodes[mplaying_index]->cancel();
                }
                Animation::cancel();
            }
        }

        void animation_ended(Animation& animation) override {
            if (animation.index() + 1 < int(mnodes.size())) {
                mnodes[animation.index() + 1]->start();
            }
            AnimationListener::animation_ended(animation);
        }

    private:
        size_t mplaying_index = 0;
        std::vector<Animation*> mnodes;
        std::vector<std::unique_ptr<Animation>> mowned_animations;
};

}


#endif //LGUI_ANIMATIONSEQUENCE_H
