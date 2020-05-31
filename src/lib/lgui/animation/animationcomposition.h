#ifndef LGUI_ANIMATIONCOMPOSITION_H
#define LGUI_ANIMATIONCOMPOSITION_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <list>

#include "animation.h"
#include "animation_listener.h"

namespace lgui {

class AnimationComposition : public IAnimation, public AnimationListener {
    public:
        using Callback = std::function<void ()>;

        AnimationComposition() = default;
        AnimationComposition(AnimationComposition&& other) = default;
        AnimationComposition(const AnimationComposition& other) = delete;
        AnimationComposition& operator=(AnimationComposition&&) = default;
        ~AnimationComposition() override;

        void animation_started(Animation& animation) override;
        void animation_cancelled(Animation& animation) override;
        void animation_ended(Animation& animation) override;

        void add_at_start(Animation& animation);
        void add_with(Animation& animation, Animation& with);
        void add_after(Animation& animation, Animation& after);

        void add_at_start(std::unique_ptr<Animation>&& animation);
        void add_with(std::unique_ptr<Animation>&& animation, Animation& with);
        void add_after(std::unique_ptr<Animation>&& animation, Animation& after);

        void set_callback_after(const Callback& callback, Animation& after);

        void start() override;
        void end() override;
        void cancel() override;

    private:
        void do_cancel();

        // This is a simple implementation, for reversing, we probably need to do something else.
        struct Node {
            std::vector<Animation*> started;
            std::vector<Animation*> finished;
            Callback finished_callback;
            uint64_t id;
        };

        void add_started_at_node(Animation& animation, Node& start_node);
        void add_finished_at_node(Animation& animation, Node& finish_node);
        static void start_node(Node& node);
        Node* new_node();

        std::list<Node> mnodes;
        std::unordered_map<Animation*, Node*> mani_start_to_node, mani_finished_to_node;
        std::vector<Animation*> mcurrently_playing;
        std::vector<std::unique_ptr<Animation>> mowned_anis;
        Node* mstart_node = nullptr;

        static uint64_t mid;
};

}

#endif //LGUI_ANIMATIONCOMPOSITION_H
