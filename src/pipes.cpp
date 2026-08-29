#include "wled.h"

#include "matrix_effects/drawing.h"
#include "matrix_effects/effects.h"

namespace matrix_effects {
    namespace {
        constexpr uint8_t maximumPipes = 5;
        constexpr uint8_t maximumPipePoints = 25;

        struct Pipe {
            Point points[maximumPipePoints];
            int8_t dx;
            int8_t dy;
            uint8_t pointCount;
            uint8_t paletteIndex;
        };

        struct PipesState {
            uint32_t nextStep;
            uint16_t width;
            uint16_t height;
            uint8_t cellSize;
            uint8_t pipeWidth;
            uint8_t pipeCount;
            uint8_t maximumLength;
            Pipe pipes[maximumPipes];
        };

        bool canMove(const Pipe &pipe, int8_t dx, int8_t dy, uint8_t cellSize, uint16_t width, uint16_t height) {
            const Point &head = pipe.points[pipe.pointCount - 1];
            const int16_t nextX = head.x + dx * cellSize;
            const int16_t nextY = head.y + dy * cellSize;
            const int16_t margin = cellSize / 2;
            return nextX >= margin && nextX < width - margin && nextY >= margin && nextY < height - margin;
        }

        void spawnPipe(Pipe &pipe, uint8_t cellSize, uint16_t width, uint16_t height, uint8_t paletteIndex) {
            const uint16_t columns = max<uint16_t>(1, width / cellSize);
            const uint16_t rows = max<uint16_t>(1, height / cellSize);
            pipe.points[0] = {
                static_cast<int16_t>((hw_random16(columns) * cellSize) + cellSize / 2),
                static_cast<int16_t>((hw_random16(rows) * cellSize) + cellSize / 2),
            };
            pipe.pointCount = 1;
            pipe.paletteIndex = paletteIndex;

            constexpr int8_t directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
            const uint8_t firstDirection = hw_random8() & 3;
            for (uint8_t index = 0; index < 4; index++) {
                const uint8_t direction = (firstDirection + index) & 3;
                if (canMove(pipe, directions[direction][0], directions[direction][1], cellSize, width, height)) {
                    pipe.dx = directions[direction][0];
                    pipe.dy = directions[direction][1];
                    return;
                }
            }
            pipe.dx = 0;
            pipe.dy = 0;
        }

        void initialise(PipesState &state, uint16_t width, uint16_t height, uint8_t pipeWidth, uint8_t pipeCount) {
            state.width = width;
            state.height = height;
            state.pipeWidth = pipeWidth;
            state.pipeCount = pipeCount;
            state.cellSize = max<uint8_t>(pipeWidth + 2, min(width, height) / 16);
            state.cellSize = max<uint8_t>(state.cellSize, 3);
            const uint16_t columns = max<uint16_t>(1, width / state.cellSize);
            const uint16_t rows = max<uint16_t>(1, height / state.cellSize);
            state.maximumLength = min<uint8_t>(maximumPipePoints, max<uint16_t>(8, columns + rows));
            for (uint8_t index = 0; index < state.pipeCount; index++) {
                spawnPipe(state.pipes[index], state.cellSize, width, height, index * 255 / state.pipeCount);
            }
        }

        void chooseDirection(Pipe &pipe, const PipesState &state) {
            const bool mustTurn = !canMove(pipe, pipe.dx, pipe.dy, state.cellSize, state.width, state.height);
            if (!mustTurn && hw_random8() > 72) return;

            const int8_t leftX = -pipe.dy;
            const int8_t leftY = pipe.dx;
            const int8_t rightX = pipe.dy;
            const int8_t rightY = -pipe.dx;
            const bool canTurnLeft = canMove(pipe, leftX, leftY, state.cellSize, state.width, state.height);
            const bool canTurnRight = canMove(pipe, rightX, rightY, state.cellSize, state.width, state.height);

            if (canTurnLeft && canTurnRight) {
                if (hw_random8() & 1) {
                    pipe.dx = leftX;
                    pipe.dy = leftY;
                } else {
                    pipe.dx = rightX;
                    pipe.dy = rightY;
                }
            } else if (canTurnLeft) {
                pipe.dx = leftX;
                pipe.dy = leftY;
            } else if (canTurnRight) {
                pipe.dx = rightX;
                pipe.dy = rightY;
            } else {
                spawnPipe(pipe, state.cellSize, state.width, state.height, hw_random8());
            }
        }

        void advancePipe(Pipe &pipe, const PipesState &state) {
            chooseDirection(pipe, state);
            const Point &head = pipe.points[pipe.pointCount - 1];
            const Point next = {
                static_cast<int16_t>(head.x + pipe.dx * state.cellSize),
                static_cast<int16_t>(head.y + pipe.dy * state.cellSize),
            };

            if (pipe.pointCount == state.maximumLength) {
                for (uint8_t index = 1; index < pipe.pointCount; index++) pipe.points[index - 1] = pipe.points[index];
                pipe.pointCount--;
            }
            pipe.points[pipe.pointCount++] = next;
        }

        void drawPipe(const Pipe &pipe, const PipesState &state) {
            const uint32_t colour = SEGMENT.color_from_palette(pipe.paletteIndex, false, true, 0);
            for (uint8_t index = 1; index < pipe.pointCount; index++) {
                // The three oldest segments fade before leaving the capped history.
                const uint8_t brightness = (index < 3) ? (80 + index * 60) : 255;
                const uint32_t segmentColour = color_fade(colour, brightness, true);
                drawThickLine(pipe.points[index - 1], pipe.points[index], color_fade(segmentColour, 112, true), state.pipeWidth + 2);
                drawThickLine(pipe.points[index - 1], pipe.points[index], segmentColour, state.pipeWidth);
            }
        }

        void modePipes() {
            if (!strip.isMatrix || !SEGMENT.is2D() || SEG_W < 8 || SEG_H < 8) {
                SEGMENT.fill(BLACK);
                return;
            }

            const uint8_t pipeWidth = 2 + (SEGMENT.intensity * 4) / 255;
            const uint8_t pipeCount = 1 + (SEGMENT.custom1 * (maximumPipes - 1)) / 255;
            if (!SEGENV.allocateData(sizeof(PipesState))) {
                SEGMENT.fill(BLACK);
                return;
            }
            PipesState &state = *reinterpret_cast<PipesState *>(SEGENV.data);

            if (SEGENV.call == 0 || state.width != SEG_W || state.height != SEG_H ||
                state.pipeWidth != pipeWidth || state.pipeCount != pipeCount) {
                initialise(state, SEG_W, SEG_H, pipeWidth, pipeCount);
            }

            const uint16_t interval = 25 + (255 - SEGMENT.speed) * 3;
            if (strip.now < state.nextStep) return;
            state.nextStep = strip.now + interval;

            for (uint8_t index = 0; index < state.pipeCount; index++) advancePipe(state.pipes[index], state);
            SEGMENT.fill(SEGCOLOR(1));
            for (uint8_t index = 0; index < state.pipeCount; index++) drawPipe(state.pipes[index], state);
        }

        static const char data[] PROGMEM = "Pipes@Speed,Pipe width,Pipe count;Pipe,Background;!;2;sx=96,ix=128,c1=96";
    } // namespace

    void registerPipes() {
        strip.addEffect(255, &modePipes, data);
    }
} // namespace matrix_effects
