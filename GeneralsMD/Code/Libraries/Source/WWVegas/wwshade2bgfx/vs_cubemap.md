# Directions: Updating BGFX Cubemap Shader to Match DX8 Logic

## 1. Add Required Vertex Attributes

Ensure your shader receives the following per-vertex attributes:
- `a_position` (vec3): Vertex position
- `a_normal` (vec3): Vertex normal
- `a_texcoord0` (vec2): Texture coordinates

## 2. Declare Required Uniforms

Add these uniforms to your shader:
- `u_modelViewProj` (mat4): Model-View-Projection matrix
- `u_model` (mat4): Model (world) matrix
- `u_eyePos` (vec3): Camera/eye position in world space
- `s_texColor` (sampler2D): Base texture
- `s_cubemap` (samplerCube): Cubemap texture

## 3. Transform Inputs to World Space

In your main vertex shader function:
- Transform the vertex position to world space:
  `worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));`
- Transform the normal to world space (and normalize):
  `N = normalize(vec3(mul((mat3)u_model, a_normal)));`
- Compute the view vector:
  `v_viewVec = normalize(u_eyePos - worldPos);`
- Pass through texture coordinates:
  `v_texcoord0 = a_texcoord0;`

## 4. Output All Required Data to the Fragment Shader

- Output these varyings:
  - `v_normal` (world space normal)
  - `v_texcoord0` (texture coordinates)
  - `v_viewVec` (view vector from surface to camera)

## 5. Update the Fragment Shader

- Accept `v_normal`, `v_texcoord0`, and `v_viewVec` as inputs.
- Sample the base texture and cubemap.
- Compute the reflection vector:
  `reflectDir = reflect(-normalize(v_viewVec), normalize(v_normal));`
- Sample the cubemap with `reflectDir`.
- Modulate the base texture with the cubemap color:
  `finalColor = texColor.rgb * envColor.rgb;`
- Output the final color:
  `gl_FragColor = vec4(finalColor, texColor.a);`

---

## Example BGFX GLSL Vertex Shader

```glsl
$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0, v_viewVec

#include <bgfx_shader.sh>

uniform mat4 u_modelViewProj;
uniform mat4 u_model;
uniform vec3 u_eyePos;

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec3 worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));
    v_normal = normalize(vec3(mul((mat3)u_model, a_normal)));
    v_texcoord0 = a_texcoord0;
    v_viewVec = normalize(u_eyePos - worldPos);
}
```

## Example BGFX GLSL Fragment Shader

```glsl
$input v_normal, v_texcoord0, v_viewVec

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform samplerCube s_cubemap;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 viewVec = normalize(v_viewVec);
    vec3 reflectDir = reflect(-viewVec, normal);
    vec4 envColor = textureCube(s_cubemap, reflectDir);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    vec3 finalColor = texColor.rgb * envColor.rgb;
    gl_FragColor = vec4(finalColor, texColor.a);
}
```

---

**Summary Table**

| Step | What to Add/Change          | Why                                 |
|------|----------------------------|-------------------------------------|
| 1    | Vertex attributes           | Needed for reflection math          |
| 2    | Shader uniforms             | Provide transformation & textures   |
| 3    | Transform to world space    | Accurate reflection calculations    |
| 4    | Output all needed varyings  | Data for fragment shader            |
| 5    | Update fragment shader      | Use new data for cubemap effect     |

---

**Follow these steps to update your BGFX cubemap shader to achieve feature parity with the DX8 cubemap logic.**