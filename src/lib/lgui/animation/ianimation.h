#ifndef LGUI_IANIMATION_H
#define LGUI_IANIMATION_H

namespace lgui {

class IAnimation {
    public:
        virtual ~IAnimation() = default;

        virtual void start() {
            mis_playing = true;
        }

        virtual void end() {
            mis_playing = false;
        }

        virtual void cancel() {
            mis_playing = false;
        }

        virtual bool is_playing() const { return mis_playing; }
        virtual bool can_delete() const { return !mis_playing; }

    private:
        bool mis_playing = false;
};

}

#endif //LGUI_IANIMATION_H
