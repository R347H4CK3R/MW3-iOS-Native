#include "D3D9StateBridge.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    MW3D3D9State state = {0};
    mw3_d3d9_reset(&state);
    assert(state.depth_enabled == 1 && state.depth_write == 1);
    assert(state.depth_compare == MW3_D3DCMP_LESSEQUAL);
    assert(state.cull_mode == MW3_D3DCULL_CCW);
    assert(state.src_blend == MW3_D3DBLEND_ONE);
    assert(state.dst_blend == MW3_D3DBLEND_ZERO);
    assert(state.color_write_mask == 15);
    assert(!state.alpha_blend_enabled);
    assert(!state.separate_alpha_enabled);

    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_SRCBLEND,
                                      MW3_D3DBLEND_SRCALPHA) == MW3_D3D9_APPLIED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_DESTBLEND,
                                      MW3_D3DBLEND_INVSRCALPHA) == MW3_D3D9_APPLIED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_ALPHABLENDENABLE,
                                      1) == MW3_D3D9_APPLIED);
    assert(state.src_blend == MW3_D3DBLEND_SRCALPHA);
    assert(state.dst_blend == MW3_D3DBLEND_INVSRCALPHA);
    assert(state.alpha_blend_enabled == 1);

    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_ZFUNC,
                                      MW3_D3DCMP_GREATER) == MW3_D3D9_APPLIED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_CULLMODE,
                                      MW3_D3DCULL_NONE) == MW3_D3D9_APPLIED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_COLORWRITEENABLE,
                                      5) == MW3_D3D9_APPLIED);
    assert(state.depth_compare == MW3_D3DCMP_GREATER);
    assert(state.cull_mode == MW3_D3DCULL_NONE);
    assert(state.color_write_mask == 5);

    MW3D3D9State unchanged = state;
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_ZENABLE,
                                      2) == MW3_D3D9_UNSUPPORTED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_SRCBLEND,
                                      MW3_D3DBLEND_BLENDFACTOR) == MW3_D3D9_UNSUPPORTED);
    assert(mw3_d3d9_set_render_state(&state, MW3_D3DRS_ZFUNC,
                                      100) == MW3_D3D9_INVALID);
    assert(mw3_d3d9_set_render_state(&state, 999,
                                      0) == MW3_D3D9_UNSUPPORTED);
    assert(mw3_d3d9_set_render_state(&state, 0,
                                      0) == MW3_D3D9_UNSUPPORTED);
    assert(!memcmp(&state, &unchanged, sizeof(state)));
    assert(mw3_d3d9_set_render_state(0, MW3_D3DRS_ZENABLE,
                                      1) == MW3_D3D9_INVALID);
    puts("D3D9 state bridge tests passed");
    return 0;
}
