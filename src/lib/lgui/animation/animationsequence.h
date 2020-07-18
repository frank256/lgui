#ifndef LGUI_ANIMATIONSEQUENCE_H
#define LGUI_ANIMATIONSEQUENCE_H

#include "abstractanimation.h"

namespace lgui {

class AnimationSequence : public AbstractAnimation, public AnimationListener {
    public:
        using Callback = std::function<void()>;

        AnimationSequence() = default;
        AnimationSequence(const AnimationSequence& other) = delete;

        void add(AbstractAnimation* ani) {
            ani->set_animation_listener(this);
            mnodes.push_back(ani);
            mnodes.back()->set_index(int(mnodes.size()) - 1);
        }

        void add(std::unique_ptr<AbstractAnimation>&& ani) {
            add(&*ani);
            mowned_animations.push_back(std::move(ani));
        }

        void start() override {
            AbstractAnimation::start();
            mplaying_index = 0;
            if (!mnodes.empty()) {
                mnodes.front()->start();
            }
        }

        void end() override {
            while (mplaying_index < mnodes.size()) {
                mnodes[mplaying_index]->end();
                ++mplaying_index;
            }
            AbstractAnimation::end();
        }

        void cancel() override {
            if (!mnodes.empty() && mplaying_index < mnodes.size()) {
                mnodes[mplaying_index]->cancel();
            }
            AbstractAnimation::cancel();
        }

        void animation_ended(AbstractAnimation& animation) override {
            if (animation.index() + 1 < int(mnodes.size())) {
                mnodes[animation.index() + 1]->start();
            }
            AnimationListener::animation_ended(animation);
        }

    private:
        size_t mplaying_index = 0;
        std::vector<AbstractAnimation*> mnodes;
        std::vector<std::unique_ptr<AbstractAnimation>> mowned_animations;
};

}


#endif //LGUI_ANIMATIONSEQUENCE_H
