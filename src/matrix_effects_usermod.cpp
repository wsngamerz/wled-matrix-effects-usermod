#include "wled.h"

#include "matrix_effects/effects.h"

class MatrixEffectsUsermod : public Usermod {
public:
    void setup() override {
        matrix_effects::registerWireframeCube();
        matrix_effects::registerPipes();
    }

    void loop() override {
    }
};

static MatrixEffectsUsermod matrixEffectsUsermod;
REGISTER_USERMOD (matrixEffectsUsermod);
