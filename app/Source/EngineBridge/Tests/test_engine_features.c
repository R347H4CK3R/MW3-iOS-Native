#include "EngineFeatures.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

int main(void) {
    assert(mw3_valid_mode(MW3_MODE_STORY));
    assert(mw3_valid_mode(MW3_MODE_MULTIPLAYER));
    assert(mw3_valid_mode(MW3_MODE_SPEC_OPS));
    assert(!mw3_valid_mode(-1));
    assert(!mw3_valid_mode(3));
    assert(mw3_valid_preset(MW3_PRESET_NEAR_NATIVE));
    assert(!mw3_valid_preset(4));
    MW3GraphicsSettings perf = mw3_graphics_settings(MW3_PRESET_PERFORMANCE);
    MW3GraphicsSettings native = mw3_graphics_settings(MW3_PRESET_NEAR_NATIVE);
    assert(perf.render_scale < native.render_scale);
    assert(native.render_scale == 1.0f);

    MW3ControllerState initial = {
        .move_x=0.3f, .move_y=0.4f, .look_x=0.2f,
        .look_y=-0.1f, .left_trigger=0.7f,
        .right_trigger=0.8f, .buttons=0x70u
    };
    MW3GyroAimInput on = {.delta_x=0.15f, .delta_y=-0.05f,
                             .sensitivity=2.0f, .enabled=1};
    MW3ControllerState result = mw3_apply_gyro_aim(initial, on);
    assert(fabsf(result.look_x - 0.5f) < 0.0001f);
    assert(fabsf(result.look_y - (-0.2f)) < 0.0001f);
    assert(result.move_x == initial.move_x && result.move_y == initial.move_y);
    assert(result.left_trigger == initial.left_trigger);
    assert(result.right_trigger == initial.right_trigger);
    assert(result.buttons == initial.buttons);
    on.enabled = 0;
    result = mw3_apply_gyro_aim(initial, on);
    assert(result.look_x == initial.look_x);
    assert(result.look_y == initial.look_y);
    assert(result.move_x == initial.move_x && result.move_y == initial.move_y);
    puts("engine features tests passed");
    return 0;
}
