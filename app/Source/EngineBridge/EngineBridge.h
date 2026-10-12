#ifndef MW3_ENGINE_BRIDGE_H
#define MW3_ENGINE_BRIDGE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Platform-independent normalized controller state.
   Button bit meanings are intentionally deferred until IW5 mapping is known. */
typedef struct MW3ControllerState {
    float move_x;
    float move_y;
    float look_x;
    float look_y;
    float left_trigger;
    float right_trigger;
    uint32_t buttons;
} MW3ControllerState;

typedef void (*MW3ControllerSink)(const MW3ControllerState *state, void *context);

MW3ControllerState mw3_normalize_controller_state(MW3ControllerState state);

/* Dispatches sanitized touch/GameController state to an engine integration sink.
   The real IW5 controller callback is not yet linked. */
bool mw3_route_controller_state(
    MW3ControllerState state,
    MW3ControllerSink sink,
    void *context
);

#ifdef __cplusplus
}
#endif

#endif
