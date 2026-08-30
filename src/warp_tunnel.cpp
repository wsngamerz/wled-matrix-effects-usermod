#include "wled.h"

#include <math.h>

#include "matrix_effects/drawing.h"
#include "matrix_effects/effects.h"

namespace matrix_effects {
    namespace {
        constexpr uint8_t particles = 52;

        float fract(float value) {
            return value - floorf(value);
        }

        void modeWarpTunnel() {
            if (!strip.isMatrix || !SEGMENT.is2D() || SEG_W < 8 || SEG_H < 8) {
                SEGMENT.fill(BLACK);
                return;
            }

            const uint16_t width = SEG_W;
            const uint16_t height = SEG_H;
            const float speed = 0.16f + (SEGMENT.speed / 255.0f) * 1.55f;
            const float phase = strip.now * 0.001f * speed;
            const float stretch = 0.035f + (SEGMENT.intensity / 255.0f) * 0.14f;
            const float drift = (SEGMENT.custom1 / 255.0f) * min(width, height) * 0.12f;
            const float centreX = (width - 1) * 0.5f + sinf(phase * 0.39f) * drift;
            const float centreY = (height - 1) * 0.5f + cosf(phase * 0.47f) * drift;
            const float aspect = width / static_cast<float>(height);

            SEGMENT.fill(SEGCOLOR(1));
            for (uint8_t particle = 0; particle < particles; particle++) {
                const float seed = particle * 0.6180339f;
                const float depth = fract(seed + phase * 0.34f);
                const float previousDepth = max(0.0f, depth - stretch);
                const float angle = fract(seed * 1.73205f) * 6.2831853f + sinf(phase * 0.27f + particle) * 0.16f;
                const float lane = 0.12f + fract(seed * 2.41421f) * 0.92f;
                const float radius = lane * depth * depth * height * 0.72f;
                const float previousRadius = lane * previousDepth * previousDepth * height * 0.72f;
                const Point tail = {
                    static_cast<int16_t>(roundf(centreX + cosf(angle) * previousRadius * aspect)),
                    static_cast<int16_t>(roundf(centreY + sinf(angle) * previousRadius)),
                };
                const Point head = {
                    static_cast<int16_t>(roundf(centreX + cosf(angle) * radius * aspect)),
                    static_cast<int16_t>(roundf(centreY + sinf(angle) * radius)),
                };
                const uint32_t colour = SEGMENT.color_from_palette(
                    particle * 255 / particles + static_cast<uint8_t>(phase * 54), false, true, 0);
                drawLine(tail, head, colour);
            }
        }

        static const char data[] PROGMEM = "Warp Tunnel@Speed,Streak length,Horizon drift;Streaks,Background;!;2;sx=120,ix=160,c1=72";
    } // namespace

    void registerWarpTunnel() {
        strip.addEffect(255, &modeWarpTunnel, data);
    }
} // namespace matrix_effects
