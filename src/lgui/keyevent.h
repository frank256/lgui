#ifndef LGUI_KEYEVENT_H
#define LGUI_KEYEVENT_H

#include "inputevent.h"
#include <string>
#include "platform/keycodes.h"

namespace lgui {

    /** A key event. A key that is repeated will continue to send `Char` events. Only the widget that
     *  currently has the focus will receive KeyEvents. */
    class KeyEvent : public InputEvent
    {
        public:

            enum Type { Pressed, Released, Char };

            KeyEvent(double timestamp, Type type, KeyCode key_code, int modifiers, int unichar, bool repeated)
                : InputEvent(timestamp, modifiers), mtype(type), mkey_code(key_code),
                  munichar(unichar), mrepeated(repeated)
            {}

            KeyCode key_code() const { return mkey_code; }
            Type type() const { return mtype; }
            int unichar() const { return munichar; }
            bool repeated() const { return mrepeated; }

            std::string to_string() const;

        private:
            Type mtype;
            KeyCode mkey_code;
            int munichar;
            bool mrepeated;
    };

}

#endif // LGUI_KEYEVENT_H

