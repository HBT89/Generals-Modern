// vs_terrain.sc — Terrain vertex shader (BGFX .sc format)
// Phase A: pass-through for CPU-pre-projected terrain vertices.
// Two UV sets: uv0 = base texture tiling, uv1 = alpha-blend mask tiling.
// Normal passed in world-space (HeightMap.cpp computes them per-vertex).
// Vertex color carries pre-baked directional+ambient light from HeightMap::updateVB().
//
// Phase B upgrade: switch to GPU-side WVP by reading u_modelViewProj and
//   removing the CPU TransformWVP() call for terrain draws.
// Phase C upgrade: use uv1 + s_texAlpha for 4-layer terrain blend.
$input a_position, a_normal, a_texcoord0, a_texcoord1, a_color0
$output v_texcoord0, v_texcoord1, v_normal, v_color

#include <bgfx_shader.sh>

void main()
{
    // CPU already projected vertices to NDC — pass through with w=1.
    gl_Position = vec4(a_position, 1.0);

    // World-space normal (used by fs_terrain for directional light in Phase B).
    v_normal    = a_normal;

    // uv0: base + blend texture coordinates (HeightMap u1/v1 field)
    v_texcoord0 = a_texcoord0;

    // uv1: alpha-mask coordinates (HeightMap u2/v2 field, used for terrain-type blending)
    v_texcoord1 = a_texcoord1;

    // Vertex color: BGRA-packed u8 normalised to [0,1] by bgfx.
    // HeightMap::updateVB() bakes directional + ambient lighting into this value.
    // Swizzle BGRA → RGBA.
    v_color = vec4(a_color0.z, a_color0.y, a_color0.x, a_color0.w);
}
