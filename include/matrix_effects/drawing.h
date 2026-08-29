#pragma once

#include <stdint.h>

namespace matrix_effects {
    struct Point {
        int16_t x;
        int16_t y;
    };

    void drawLine(Point from, Point to, uint32_t colour);

    void drawThickLine(Point from, Point to, uint32_t colour, uint8_t width);
} // namespace matrix_effects
