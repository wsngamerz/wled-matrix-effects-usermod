#include "wled.h"

#include <math.h>

#include "matrix_effects/drawing.h"
#include "matrix_effects/effects.h"

namespace matrix_effects {
    namespace {
        constexpr uint8_t sides = 8;
        constexpr uint8_t rings = 12;

        void modeNeonTunnel() {
            if (!strip.isMatrix || !SEGMENT.is2D() || SEG_W < 8 || SEG_H < 8) {
                SEGMENT.fill(BLACK);
                return;
            }

            const uint16_t width = SEG_W;
            const uint16_t height = SEG_H;
            const float speed = 0.10f + (SEGMENT.speed / 255.0f) * 1.10f;
            const float phase = strip.now * 0.001f * speed;
            const float pulse = 0.80f + sinf(phase * 2.3f) * (SEGMENT.intensity / 255.0f) * 0.26f;
            const float drift = (SEGMENT.custom1 / 255.0f) * min(width, height) * 0.13f;
            const float centreX = (width - 1) * 0.5f + sinf(phase * 0.41f) * drift;
            const float centreY = (height - 1) * 0.5f + cosf(phase * 0.53f) * drift;
            const float farRadius = min(width, height) * 0.055f;
            const float nearRadius = min(width, height) * 0.92f;

            SEGMENT.fill(SEGCOLOR(1));
            Point previousRing[sides];
            bool hasPreviousRing = false;
            for (int8_t ring = rings - 1; ring >= 0; ring--) {
                const float depth = 0.10f + fmodf(ring / static_cast<float>(rings) + phase * 0.17f, 1.0f);
                const float radius = farRadius + (nearRadius - farRadius) * depth * depth * pulse;
                const float rotation = phase * (0.22f + depth * 0.34f) + depth * depth * 1.4f;
                Point currentRing[sides];

                for (uint8_t side = 0; side < sides; side++) {
                    const float angle = rotation + side * 6.2831853f / sides;
                    currentRing[side] = {
                        static_cast<int16_t>(roundf(centreX + cosf(angle) * radius)),
                        static_cast<int16_t>(roundf(centreY + sinf(angle) * radius)),
                    };
                }

                const uint32_t colour = SEGMENT.color_from_palette(
                    static_cast<uint16_t>(depth * 255) + static_cast<uint8_t>(phase * 37), false, true, 0);
                for (uint8_t side = 0; side < sides; side++) {
                    drawLine(currentRing[side], currentRing[(side + 1) % sides], colour);
                    if (hasPreviousRing) drawLine(currentRing[side], previousRing[side], colour);
                }
                for (uint8_t side = 0; side < sides; side++) previousRing[side] = currentRing[side];
                hasPreviousRing = true;
            }
        }

        static const char data[] PROGMEM = "Neon Tunnel@Speed,Pulse,Horizon drift;Tunnel,Background;!;2;sx=112,ix=148,c1=80";
    } // namespace

    void registerNeonTunnel() {
        strip.addEffect(255, &modeNeonTunnel, data);
    }
} // namespace matrix_effects
