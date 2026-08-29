#include "wled.h"

#include "matrix_effects/drawing.h"

namespace matrix_effects {
    namespace {
        int16_t absolute(int16_t value) {
            return value < 0 ? -value : value;
        }

        int8_t direction(int16_t value) {
            return value < 0 ? -1 : 1;
        }
    } // namespace

    void drawLine(Point from, Point to, uint32_t colour) {
        int16_t dx = absolute(to.x - from.x);
        int16_t dy = -absolute(to.y - from.y);
        int8_t sx = direction(to.x - from.x);
        int8_t sy = direction(to.y - from.y);
        int16_t error = dx + dy;

        while (true) {
            SEGMENT.setPixelColorXY(from.x, from.y, colour);
            if (from.x == to.x && from.y == to.y) return;

            int16_t doubledError = 2 * error;
            if (doubledError >= dy) {
                error += dy;
                from.x += sx;
            }
            if (doubledError <= dx) {
                error += dx;
                from.y += sy;
            }
        }
    }

    void drawThickLine(Point from, Point to, uint32_t colour, uint8_t width) {
        const int8_t radius = (width - 1) / 2;
        for (int8_t offsetY = -radius; offsetY <= radius; offsetY++) {
            for (int8_t offsetX = -radius; offsetX <= radius; offsetX++) {
                drawLine({static_cast<int16_t>(from.x + offsetX), static_cast<int16_t>(from.y + offsetY)},
                         {static_cast<int16_t>(to.x + offsetX), static_cast<int16_t>(to.y + offsetY)},
                         colour);
            }
        }
    }
} // namespace matrix_effects
