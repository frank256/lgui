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
            mnodes.back()->set_index(count() - 1);
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
                while (is_valid_index(mplaying_index)) {
                    mnodes[mplaying_index]->end();
                    ++mplaying_index;
                }
                Animation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                if (!mnodes.empty() && is_valid_index(mplaying_index)) {
                    mnodes[mplaying_index]->cancel();
                }
                Animation::cancel();
            }
        }

        void start_reverse() override {
            if (!is_playing()) {
                Animation::start_reverse();
                mplaying_index = count() - 1;
                if (!mnodes.empty()) {
                    mnodes.back()->start_reverse();
                }
            }
        }

        void end_reverse() override {
            if (is_playing()) {
                while (is_valid_index(mplaying_index)) {
                    mnodes[mplaying_index]->end_reverse();
                    --mplaying_index;
                }
                Animation::end_reverse();
            }
        }

        void reverse() override {
            if (is_playing()) {
                if (!mnodes.empty() && is_valid_index(mplaying_index)) {
                    mnodes[mplaying_index]->reverse();
                }
                Animation::reverse();
            }
        }

        void animation_ended(Animation& animation) override {
            int new_index = animation.index() + 1;
            if (is_valid_index(new_index)) {
                mplaying_index = new_index;
                mnodes[new_index]->start();
            } else if (new_index >= count()) {
                Animation::end();
            }
        }

        void animation_ended_reversed(Animation& animation) override {
            int new_index = animation.index() - 1;
            if (is_valid_index(new_index)) {
                mplaying_index = new_index;
                mnodes[new_index]->start_reverse();
            } else if (new_index < 0) {
                Animation::end_reverse();
            }
        }

        int count() const { return int(mnodes.size()); }

    private:
        bool is_valid_index(int index) const {
            return index >= 0 && index < count();
        }

        int mplaying_index = 0;
        std::vector<Animation*> mnodes;
        std::vector<std::unique_ptr<Animation>> mowned_animations;
};

}


#endif //LGUI_ANIMATIONSEQUENCE_H
