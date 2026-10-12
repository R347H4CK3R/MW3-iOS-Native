#include "EngineFeatures.h"
#include <math.h>

int mw3_valid_mode(int mode) {
    return mode >= MW3_MODE_STORY && mode <= MW3_MODE_SPEC_OPS;
}

int mw3_valid_preset(int preset) {
    return preset >= MW3_PRESET_PERFORMANCE && preset <= MW3_PRESET_NEAR_NATIVE;
}

MW3GraphicsSettings mw3_graphics_settings(MW3GraphicsPreset preset) {
    switch (preset) {
    case MW3_PRESET_PERFORMANCE: return (MW3GraphicsSettings){0.60f, 0};
    case MW3_PRESET_BALANCED:    return (MW3GraphicsSettings){0.75f, 0};
    case MW3_PRESET_QUALITY:     return (MW3GraphicsSettings){0.90f, 1};
    case MW3_PRESET_NEAR_NATIVE: return (MW3GraphicsSettings){1.00f, 1};
    default:                     return (MW3GraphicsSettings){0.60f, 0};
    }
}

MW3ControllerState mw3_apply_gyro_aim(
    MW3ControllerState base, MW3GyroAimInput gyro
) {
    base = mw3_normalize_controller_state(base);
    if (!gyro.enabled) return base;
    if (!isfinite(gyro.delta_x) || !isfinite(gyro.delta_y) ||
        !isfinite(gyro.sensitivity) || gyro.sensitivity <= 0.0f) {
        return base;
    }
    base.look_x += gyro.delta_x * gyro.sensitivity;
    base.look_y += gyro.delta_y * gyro.sensitivity;
    return mw3_normalize_controller_state(base);
}
