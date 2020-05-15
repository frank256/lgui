#ifndef LGUITEST_A5TRANSFORM_H
#define LGUITEST_A5TRANSFORM_H

#include <allegro5/transformations.h>

#include "lgui/lgui_types.h"

namespace lgui {

class A5Transform {
    public:
        void set_identity() { al_identity_transform(&mtransform); }

        void add_translation(PointF offset) {
            al_translate_transform(&mtransform, offset.x(), offset.y());
        }

        const ALLEGRO_TRANSFORM& a5_transform() const {
            return mtransform;
        }

    private:
        ALLEGRO_TRANSFORM mtransform;
};

}

#endif //LGUITEST_A5TRANSFORM_H
