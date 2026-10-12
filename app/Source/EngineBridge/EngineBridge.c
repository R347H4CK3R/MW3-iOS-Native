#include "EngineBridge.h"

#include <math.h>

static float clamp_axis(float value) {
    if (!isfinite(value)) return 0.0f;
    if (value < -1.0f) return -1.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static float clamp_trigger(float value) {
    if (!isfinite(value)) return 0.0f;
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

MW3ControllerState mw3_normalize_controller_state(MW3ControllerState state) {
    state.move_x = clamp_axis(state.move_x);
    state.move_y = clamp_axis(state.move_y);
    state.look_x = clamp_axis(state.look_x);
    state.look_y = clamp_axis(state.look_y);
    state.left_trigger = clamp_trigger(state.left_trigger);
    state.right_trigger = clamp_trigger(state.right_trigger);
    return state;
}

bool mw3_route_controller_state(
    MW3ControllerState state,
    MW3ControllerSink sink,
    void *context
) {
    if (sink == 0) return false;
    state = mw3_normalize_controller_state(state);
    sink(&state, context);
    return true;
}
