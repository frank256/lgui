#ifndef LGUI_SIMULTANEOUSANIMATIONS_H
#define LGUI_SIMULTANEOUSANIMATIONS_H

#include <vector>
#include "abstractanimation.h"

namespace lgui {

class SimultaneousAnimations : public AbstractAnimation {
    public:
        SimultaneousAnimations() = default;
        SimultaneousAnimations(const SimultaneousAnimations& other) = delete;

        void start() override {
            for (auto& ani : mchildren) {
                ani->start();
            }
            AbstractAnimation::start();
        }

        void end() override {
            for (auto& ani : mchildren) {
                ani->end();
            }
            AbstractAnimation::end();
        }

        void cancel() override {
            for (auto& ani : mchildren) {
                ani->cancel();
            }
            AbstractAnimation::cancel();
        }

        void add(AbstractAnimation* ani) {
            mchildren.push_back(ani);
        }

        void add(std::unique_ptr<AbstractAnimation>&& ani) {
            add(ani.get());
            mowned_children.push_back(std::move(ani));
        }

    protected:
        std::vector<AbstractAnimation*> mchildren;
        std::vector<std::unique_ptr<AbstractAnimation>> mowned_children;
};

}

#endif //LGUI_SIMULTANEOUSANIMATIONS_H
