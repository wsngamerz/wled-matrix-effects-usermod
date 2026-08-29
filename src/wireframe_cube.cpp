#include "wled.h"

#include <math.h>

#include "matrix_effects/drawing.h"
#include "matrix_effects/effects.h"

namespace matrix_effects {
    namespace {
        struct Vector3 {
            float x;
            float y;
            float z;
        };

        constexpr Vector3 vertices[8] = {
            {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f},
        };

        constexpr uint8_t edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7},
        };

        void modeWireframeCube() {
            if (!strip.isMatrix || !SEGMENT.is2D()) {
                SEGMENT.fill(BLACK);
                return;
            }

            const uint16_t width = SEG_W;
            const uint16_t height = SEG_H;
            if (width < 8 || height < 8) {
                SEGMENT.fill(BLACK);
                return;
            }

            const float angularVelocity = 0.15f + (SEGMENT.speed / 255.0f) * 2.10f;
            const float angle = (strip.now / 1000.0f) * angularVelocity;
            const float sinX = sinf(angle * 0.70f);
            const float cosX = cosf(angle * 0.70f);
            const float sinY = sinf(angle);
            const float cosY = cosf(angle);
            const float shortestSide = min(width, height);
            const float cubeHalfSize = 0.15f + (SEGMENT.intensity / 255.0f) * 0.85f;
            const float cameraDistance = 5.5f - (SEGMENT.custom1 / 255.0f) * 2.0f;
            const float focalLength = shortestSide * 1.15f;
            const float centerX = (width - 1) * 0.5f;
            const float centerY = (height - 1) * 0.5f;
            Point points[8];

            for (uint8_t index = 0; index < 8; index++) {
                const Vector3 vertex = vertices[index];
                const float x = vertex.x * cosY + vertex.z * sinY;
                const float z = -vertex.x * sinY + vertex.z * cosY;
                const float y = vertex.y * cosX - z * sinX;
                const float rotatedZ = vertex.y * sinX + z * cosX;
                const float projectedScale = focalLength / (cameraDistance + rotatedZ);

                points[index] = {
                    static_cast<int16_t>(roundf(centerX + x * cubeHalfSize * projectedScale)),
                    static_cast<int16_t>(roundf(centerY - y * cubeHalfSize * projectedScale)),
                };
            }

            SEGMENT.fill(BLACK);
            const uint32_t colour = SEGMENT.color_from_palette(0, false, false, 0);
            for (uint8_t index = 0; index < 12; index++) {
                drawLine(points[edges[index][0]], points[edges[index][1]], colour);
            }
        }

        static const char data[] PROGMEM = "Wireframe Cube@Rotation speed,Cube size,Perspective;Cube;!;2;sx=96,ix=180,c1=128";
    } // namespace

    void registerWireframeCube() {
        strip.addEffect(255, &modeWireframeCube, data);
    }
} // namespace matrix_effects
