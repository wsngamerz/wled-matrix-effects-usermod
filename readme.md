# WLED Matrix Effects Usermod

This is a usermod containing a number of wled effects for matrix panels.

Based off the https://github.com/wled/wled-usermod-example template.

## Effects

Currently includes:
- `Wireframe Cube`
- `Pipes`

### Wireframe Cube

Select **Wireframe Cube** from WLED's Effects list. Its normal effect controls are:

| WLED control   | Cube behaviour                         |
|----------------|----------------------------------------|
| Effect colour  | Wireframe colour                       |
| Rotation speed | Angular velocity                       |
| Cube size      | Projected cube size                    |
| Perspective    | Strength of the perspective projection |

### Pipes

Select **Pipes** from WLED's Effects list. It is a growing, right-angled pipe animation inspired by the Windows Pipes screensaver.

| WLED control   | Pipes behaviour                         |
|----------------|-----------------------------------------|
| Effect palette | Colours sampled by the growing pipes    |
| Pipe colour    | Colour used with the Default palette    |
| Background     | Screen background                       |
| Speed          | Rate at which each pipe grows           |
| Pipe width     | Thickness of pipes and their outlines   |
| Pipe count     | Number of simultaneously growing pipes  |

Pipes grow in straight segments, make random right-angle turns, and retain a
bounded trail. Once a pipe reaches its maximum length, its oldest segments dim
and disappear while new segments continue to grow at the head. It is designed
as a matrix-friendly version of the classic screensaver rather than a 3D
rendering.

## Notes

This was built for WLED 16.x and has only been tested with a single HUB75 64x64 panel. YMMV

## Project layout

- `src/matrix_effects_usermod.cpp` registers every effect in this package.
- `src/<effect>.cpp` contains one effect and its WLED control metadata.
- `src/drawing.cpp` and `include/matrix_effects/drawing.h` provide shared line drawing.

To add an effect, create its source file, declare its registration function in `include/matrix_effects/effects.h`, and call it from `MatrixEffectsUsermod::setup()`.
