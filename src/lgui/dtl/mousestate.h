#ifndef LGUI_MOUSESTATETRACKER_H
#define LGUI_MOUSESTATETRACKER_H

#include "lgui/lgui.h"

namespace lgui {

namespace dtl {

class MouseState {
    public:
        MouseState()
            : mdragged_button(0), mpressed_button(0),
              mtimestamp(0.0) {}

        const Position& pos() const { return mpos; }
        void set_pos(const Position& pos)
        {
            mpos = pos;
        }

        int dragged_button() const { return mdragged_button; }
        void set_dragged_button(int dragged_button)
        {
            mdragged_button = dragged_button;
        }

        double timestamp() const { return mtimestamp; }
        void set_timestamp(double timestamp) { mtimestamp = timestamp; }

        int pressed_button() const { return mpressed_button; }
        void set_pressed_button(int pressed_button)
        {
            mpressed_button = pressed_button;
        }

    private:
        Position mpos;
        int mdragged_button, mpressed_button;
        double mtimestamp;
};


}

}


#endif //LGUI_MOUSESTATETRACKER_H
