// fs_terrain.sc — Terrain fragment shader (BGFX .sc format)
// Phase A: single-texture base pass. Samples s_texBase and multiplies by
//   vertex color (which already contains baked directional+ambient lighting).
//
// Phase B upgrade: remove baked lighting from vertex color (delete
//   HeightMap::doTheDynamicLight), add u_lights[8] uniform array,
//   compute diffuse+specular per-fragment.
//
// Phase C upgrade: enable multi-texture blend:
//   color = lerp(s_texBase, s_texBlend, s_texAlpha.r)
//   Gate on u_terrainLayerCount > 1 to avoid blending with 1x1 white stubs.
//
// Phase D: add s_shadowMap sample, multiply NdotL by (1 - shadow).
$input v_texcoord0, v_texcoord1, v_normal, v_color

#include <bgfx_shader.sh>

// Stage 0 — base terrain texture (always bound)
uniform sampler2D s_texColor;   // matches k_fsMesh sampler name so existing
                                // bgfx::createUniform("s_texColor") works.

// Stage 1 — blend texture (Phase C, stub-bound to 1x1 white for now)
// uniform sampler2D s_texBlend;

// Stage 2 — alpha mask (Phase C, stub-bound to 1x1 white for now)
// uniform sampler2D s_texAlpha;

// Phase B lighting uniforms (stub declarations — wired up when GPU lighting lands)
// uniform vec4 u_lightDir;      // world-space directional light xyz, unused w
// uniform vec4 u_lightColor;    // rgb + intensity
// uniform vec4 u_ambientColor;  // rgb + fill factor

void main()
{
    // -- Phase A: base texture * pre-baked vertex lighting --
    vec4 base = texture2D(s_texColor, v_texcoord0);

    // -- Phase C multi-texture blend (uncomment when u_terrainLayerCount > 1) --
    // vec4 blend = texture2D(s_texBlend, v_texcoord0);
    // float alpha = texture2D(s_texAlpha, v_texcoord1).r;
    // base = mix(base, blend, alpha);

    // -- Phase B GPU lighting (uncomment when doTheDynamicLight() is removed) --
    // vec3 N = normalize(v_normal);
    // float NdotL = max(dot(N, normalize(u_lightDir.xyz)), 0.0);
    // vec3 light = u_ambientColor.rgb * u_ambientColor.w
    //            + u_lightColor.rgb   * u_lightColor.w * NdotL;
    // light = clamp(light, 0.0, 2.0);
    // gl_FragColor = vec4(base.rgb * light * v_color.rgb, base.a * v_color.a);

    gl_FragColor = base * v_color;
}
