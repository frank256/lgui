#ifndef LGUI_SIMULTANEOUSANIMATIONS_H
#define LGUI_SIMULTANEOUSANIMATIONS_H

#include <vector>
#include "animation.h"

namespace lgui {

class SimultaneousAnimations : public Animation, public AnimationListener {
    public:
        SimultaneousAnimations() = default;
        SimultaneousAnimations(const SimultaneousAnimations& other) = delete;

        void start() override {
            if (!is_playing()) {
                mplaying_count = 0;
                for (auto& ani : mchildren) {
                    ani->start();
                    ++mplaying_count;
                }
                Animation::start();
            }
        }

        void end() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->end();
                    --mplaying_count;
                }
                Animation::end();
            }
        }

        void cancel() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->cancel();
                    --mplaying_count;
                }
                Animation::cancel();
            }
        }

        void start_reverse() override {
            if (!is_playing()) {
                mplaying_count = 0;
                for (auto& ani : mchildren) {
                    ani->start_reverse();
                    ++mplaying_count;
                }
                Animation::start_reverse();
            }
        }

        void end_reverse() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    ani->end_reverse();
                    --mplaying_count;
                }
                Animation::end_reverse();
            }
        }

        void reverse() override {
            if (is_playing()) {
                for (auto& ani : mchildren) {
                    if (ani->is_playing()) {
                        ani->reverse();
                    }
                }
                Animation::reverse();
            }
        }

        void add(Animation* ani) {
            ani->set_animation_listener(this);
            mchildren.push_back(ani);
        }

        void add(std::unique_ptr<Animation>&& ani) {
            add(ani.get());
            mowned_children.push_back(std::move(ani));
        }

        void animation_ended(Animation&) override {
            --mplaying_count;
            if (mplaying_count == 0) {
                Animation::end();
            }
        }

        void animation_ended_reversed(Animation&) override {
            --mplaying_count;
            if (mplaying_count == 0) {
                Animation::end_reverse();
            }
        }

    protected:
        std::vector<Animation*> mchildren;
        std::vector<std::unique_ptr<Animation>> mowned_children;
        int mplaying_count = 0;
};

}

#endif //LGUI_SIMULTANEOUSANIMATIONS_H
