#include "D3D9StateBridge.h"
#include <string.h>

static int valid_blend(uint32_t v) {
    switch (v) {
        case MW3_D3DBLEND_ZERO: case MW3_D3DBLEND_ONE:
        case MW3_D3DBLEND_SRCCOLOR: case MW3_D3DBLEND_INVSRCCOLOR:
        case MW3_D3DBLEND_SRCALPHA: case MW3_D3DBLEND_INVSRCALPHA:
        case MW3_D3DBLEND_DESTALPHA: case MW3_D3DBLEND_INVDESTALPHA:
        case MW3_D3DBLEND_DESTCOLOR: case MW3_D3DBLEND_INVDESTCOLOR:
        case MW3_D3DBLEND_SRCALPHASAT:
            return 1;
        default:
            /* Blend-factor and both-src-alpha require extra compatibility handling. */
            return 0;
    }
}
static int valid_op(uint32_t v) {
    return v >= MW3_D3DBLENDOP_ADD && v <= MW3_D3DBLENDOP_MAX;
}
static int valid_compare(uint32_t v) {
    return v >= MW3_D3DCMP_NEVER && v <= MW3_D3DCMP_ALWAYS;
}

void mw3_d3d9_reset(MW3D3D9State *s) {
    if (!s) return;
    memset(s, 0, sizeof(*s));
    s->depth_enabled=1;
    s->depth_write=1;
    s->depth_compare=MW3_D3DCMP_LESSEQUAL;
    s->cull_mode=MW3_D3DCULL_CCW;
    s->src_blend=MW3_D3DBLEND_ONE;
    s->dst_blend=MW3_D3DBLEND_ZERO;
    s->blend_op=MW3_D3DBLENDOP_ADD;
    s->src_alpha_blend=MW3_D3DBLEND_ONE;
    s->dst_alpha_blend=MW3_D3DBLEND_ZERO;
    s->alpha_blend_op=MW3_D3DBLENDOP_ADD;
    s->color_write_mask=15;
}

MW3D3D9Result mw3_d3d9_set_render_state(
    MW3D3D9State *s, uint32_t type, uint32_t value
) {
    if (!s) return MW3_D3D9_INVALID;
    /* Reject invalid or unsupported values without changing the previous state. */
    switch (type) {
        case MW3_D3DRS_ZENABLE:
            if (value > 1) return MW3_D3D9_UNSUPPORTED; /* W-buffer omitted */
            s->depth_enabled=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_ZWRITEENABLE:
            s->depth_write=(value!=0); return MW3_D3D9_APPLIED;
        case MW3_D3DRS_ZFUNC:
            if (!valid_compare(value)) return MW3_D3D9_INVALID;
            s->depth_compare=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_CULLMODE:
            if (value < MW3_D3DCULL_NONE || value > MW3_D3DCULL_CCW)
                return MW3_D3D9_INVALID;
            s->cull_mode=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_ALPHABLENDENABLE:
            s->alpha_blend_enabled=(value!=0); return MW3_D3D9_APPLIED;
        case MW3_D3DRS_SRCBLEND:
            if (!valid_blend(value)) return MW3_D3D9_UNSUPPORTED;
            s->src_blend=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_DESTBLEND:
            if (!valid_blend(value)) return MW3_D3D9_UNSUPPORTED;
            s->dst_blend=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_BLENDOP:
            if (!valid_op(value)) return MW3_D3D9_INVALID;
            s->blend_op=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_SEPARATEALPHABLENDENABLE:
            s->separate_alpha_enabled=(value!=0); return MW3_D3D9_APPLIED;
        case MW3_D3DRS_SRCBLENDALPHA:
            if (!valid_blend(value)) return MW3_D3D9_UNSUPPORTED;
            s->src_alpha_blend=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_DESTBLENDALPHA:
            if (!valid_blend(value)) return MW3_D3D9_UNSUPPORTED;
            s->dst_alpha_blend=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_BLENDOPALPHA:
            if (!valid_op(value)) return MW3_D3D9_INVALID;
            s->alpha_blend_op=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_COLORWRITEENABLE:
            if (value > 15) return MW3_D3D9_INVALID;
            s->color_write_mask=value; return MW3_D3D9_APPLIED;
        case MW3_D3DRS_SCISSORTESTENABLE:
            s->scissor_enabled=(value!=0); return MW3_D3D9_APPLIED;
        default:
            return MW3_D3D9_UNSUPPORTED;
    }
}
