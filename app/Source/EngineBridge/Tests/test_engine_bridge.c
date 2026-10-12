#include "EngineBridge.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static MW3ControllerState received;
static int received_count = 0;

static void sink(const MW3ControllerState *state, void *context) {
    (void)context;
    received = *state;
    received_count++;
}

int main(void) {
    MW3ControllerState input = {
        .move_x = 2.0f,
        .move_y = -2.0f,
        .look_x = NAN,
        .look_y = 0.5f,
        .left_trigger = -5.0f,
        .right_trigger = 5.0f,
        .buttons = 0x55u
    };
    assert(!mw3_route_controller_state(input, 0, 0));
    assert(mw3_route_controller_state(input, sink, 0));
    assert(received_count == 1);
    assert(received.move_x == 1.0f);
    assert(received.move_y == -1.0f);
    assert(received.look_x == 0.0f);
    assert(received.look_y == 0.5f);
    assert(received.left_trigger == 0.0f);
    assert(received.right_trigger == 1.0f);
    assert(received.buttons == 0x55u);
    puts("input bridge tests passed");
    return 0;
}
