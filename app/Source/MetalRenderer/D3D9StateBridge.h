#ifndef MW3_D3D9_STATE_BRIDGE_H
#define MW3_D3D9_STATE_BRIDGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Subset of actual Direct3D 9 numeric render-state identifiers.
 * Unhandled state types MUST return MW3_D3D9_UNSUPPORTED, not silently pass. */
enum {
    MW3_D3DRS_ZENABLE = 7,
    MW3_D3DRS_ZWRITEENABLE = 14,
    MW3_D3DRS_SRCBLEND = 19,
    MW3_D3DRS_DESTBLEND = 20,
    MW3_D3DRS_CULLMODE = 22,
    MW3_D3DRS_ZFUNC = 23,
    MW3_D3DRS_ALPHABLENDENABLE = 27,
    MW3_D3DRS_COLORWRITEENABLE = 168,
    MW3_D3DRS_BLENDOP = 171,
    MW3_D3DRS_SCISSORTESTENABLE = 174,
    MW3_D3DRS_SEPARATEALPHABLENDENABLE = 206,
    MW3_D3DRS_SRCBLENDALPHA = 207,
    MW3_D3DRS_DESTBLENDALPHA = 208,
    MW3_D3DRS_BLENDOPALPHA = 209
};

/* D3DBLEND, D3DCMPFUNC, D3DCULL and D3DBLENDOP enum values. */
enum {
    MW3_D3DBLEND_ZERO=1, MW3_D3DBLEND_ONE=2,
    MW3_D3DBLEND_SRCCOLOR=3, MW3_D3DBLEND_INVSRCCOLOR=4,
    MW3_D3DBLEND_SRCALPHA=5, MW3_D3DBLEND_INVSRCALPHA=6,
    MW3_D3DBLEND_DESTALPHA=7, MW3_D3DBLEND_INVDESTALPHA=8,
    MW3_D3DBLEND_DESTCOLOR=9, MW3_D3DBLEND_INVDESTCOLOR=10,
    MW3_D3DBLEND_SRCALPHASAT=11,
    MW3_D3DBLEND_BLENDFACTOR=14, MW3_D3DBLEND_INVBLENDFACTOR=15
};
enum {
    MW3_D3DCMP_NEVER=1, MW3_D3DCMP_LESS=2, MW3_D3DCMP_EQUAL=3,
    MW3_D3DCMP_LESSEQUAL=4, MW3_D3DCMP_GREATER=5,
    MW3_D3DCMP_NOTEQUAL=6, MW3_D3DCMP_GREATEREQUAL=7,
    MW3_D3DCMP_ALWAYS=8
};
enum { MW3_D3DCULL_NONE=1, MW3_D3DCULL_CW=2, MW3_D3DCULL_CCW=3 };
enum {
    MW3_D3DBLENDOP_ADD=1, MW3_D3DBLENDOP_SUBTRACT=2,
    MW3_D3DBLENDOP_REVSUBTRACT=3, MW3_D3DBLENDOP_MIN=4,
    MW3_D3DBLENDOP_MAX=5
};
typedef enum MW3D3D9Result {
    MW3_D3D9_APPLIED = 0,
    MW3_D3D9_UNSUPPORTED = 1,
    MW3_D3D9_INVALID = 2
} MW3D3D9Result;

typedef struct MW3D3D9State {
    uint32_t depth_enabled;
    uint32_t depth_write;
    uint32_t depth_compare;
    uint32_t cull_mode;
    uint32_t alpha_blend_enabled;
    uint32_t src_blend;
    uint32_t dst_blend;
    uint32_t blend_op;
    uint32_t separate_alpha_enabled;
    uint32_t src_alpha_blend;
    uint32_t dst_alpha_blend;
    uint32_t alpha_blend_op;
    uint32_t color_write_mask;
    uint32_t scissor_enabled;
} MW3D3D9State;

void mw3_d3d9_reset(MW3D3D9State *state);
MW3D3D9Result mw3_d3d9_set_render_state(
    MW3D3D9State *state, uint32_t render_state, uint32_t value
);

#ifdef __cplusplus
}
#endif
#endif
