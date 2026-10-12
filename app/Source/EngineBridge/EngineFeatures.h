#ifndef MW3_ENGINE_FEATURES_H
#define MW3_ENGINE_FEATURES_H

#include "EngineBridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MW3GameMode {
    MW3_MODE_STORY = 0,
    MW3_MODE_MULTIPLAYER = 1,
    MW3_MODE_SPEC_OPS = 2
} MW3GameMode;

typedef enum MW3GraphicsPreset {
    MW3_PRESET_PERFORMANCE = 0,
    MW3_PRESET_BALANCED = 1,
    MW3_PRESET_QUALITY = 2,
    MW3_PRESET_NEAR_NATIVE = 3
} MW3GraphicsPreset;

/* Preset is advisory only until a real Metal renderer integrates it. */
typedef struct MW3GraphicsSettings {
    float render_scale;
    int prefer_high_quality_effects;
} MW3GraphicsSettings;

typedef struct MW3GyroAimInput {
    float delta_x;
    float delta_y;
    float sensitivity;
    int enabled;
} MW3GyroAimInput;

int mw3_valid_mode(int mode);
int mw3_valid_preset(int preset);
MW3GraphicsSettings mw3_graphics_settings(MW3GraphicsPreset preset);

/* Updates LOOK ONLY; never modifies movement, steering, or triggers.
   Input is a normalized, pre-filtered angular delta from CoreMotion. */
MW3ControllerState mw3_apply_gyro_aim(
    MW3ControllerState base, MW3GyroAimInput gyro
);

#ifdef __cplusplus
}
#endif
#endif
