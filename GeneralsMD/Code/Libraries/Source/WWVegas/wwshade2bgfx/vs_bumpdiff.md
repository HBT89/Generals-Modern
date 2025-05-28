# Directions: Updating BGFX Bump Diffuse Shader to Match DX8 Logic

## 1. Add Required Vertex Attributes

Ensure your shader receives the following per-vertex attributes:
- `a_position` (vec3): Vertex position
- `a_normal` (vec3): Vertex normal
- `a_tangent` (vec3): Tangent vector (S)
- `a_bitangent` (vec3): Bitangent vector (T)
- `a_texcoord0` (vec2): Texture coordinates
- `a_color` (vec4): Vertex color

## 2. Declare Required Uniforms

Add these uniforms to your shader:
- `u_modelViewProj` (mat4): Model-View-Projection matrix
- `u_model` (mat4): Model (world) matrix
- `u_texTransform0` (mat4): Texture transform
- `u_lightDir0` ... `u_lightDir3` (vec3): Light directions in world space (up to 4)
- `u_lightColor0` ... `u_lightColor3` (vec4): Light color/attenuation (up to 4)
- `u_diffuse` (vec4): Material diffuse
- `u_ambient` (vec4): Material ambient
- `u_bumpiness` (vec2): Bump scale and bias (optional, for fragment shader)

## 3. Transform Inputs to World Space

In your main vertex shader function:
- Transform the vertex position to clip space:
  `gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));`
- Transform the normal, tangent, and bitangent to world space (and normalize):
  ```
  mat3 model3 = mat3(u_model);
  N = normalize(model3 * a_normal);
  T = normalize(model3 * a_tangent);
  B = normalize(model3 * a_bitangent);
  ```

## 4. Build the TBN Matrix

- Construct the tangent space basis matrix:
  ```glsl
  mat3 TBN = mat3(T, B, N);
  ```

## 5. Compute Light Vectors in Tangent Space

- For each light (0-3):
  - Calculate light direction in world space and normalize:
    `vec3 L = normalize(u_lightDirN);`
  - Transform to tangent space:
    `v_lightDirTSN = TBN * L;`

## 6. Calculate Per-Vertex Diffuse Lighting

- For each light (0-3):
  - Compute Lambertian factor:
    `float NdotL = max(dot(N, L), 0.0) * u_lightColorN.w;`
  - Accumulate color:
    `col += u_lightColorN.rgb * NdotL;`
- Apply vertex color, material diffuse, and ambient:
  `col *= a_color.rgb * u_diffuse.rgb;`
  `col += u_ambient.rgb;`
- Output as `v_color`:
  `v_color = vec4(col, 1.0);`

## 7. Output All Required Data to the Fragment Shader

- Output these varyings:
  - `v_texcoord0` (transformed texture coordinates)
  - `v_normal`, `v_tangent`, `v_bitangent` (world space, for per-fragment calculations if needed)
  - `v_lightDirTS0` ... `v_lightDirTS3` (light vectors in tangent space)
  - `v_color` (per-vertex diffuse color)

## 8. Update the Fragment Shader

- Ensure it uses `v_lightDirTS*` for bump mapping and `v_color` for diffuse color.
- Apply bumpiness scaling/bias in the fragment shader using `u_bumpiness` if needed.

---

## Example BGFX GLSL Vertex Shader

```glsl
$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0, a_color
$output v_normal, v_tangent, v_bitangent, v_texcoord0
$output v_lightDirTS0, v_lightDirTS1, v_lightDirTS2, v_lightDirTS3
$output v_color

#include <bgfx_shader.sh>

uniform mat4 u_modelViewProj;
uniform mat4 u_model;
uniform mat4 u_texTransform0;

uniform vec3 u_lightDir0;
uniform vec3 u_lightDir1;
uniform vec3 u_lightDir2;
uniform vec3 u_lightDir3;

uniform vec4 u_lightColor0;
uniform vec4 u_lightColor1;
uniform vec4 u_lightColor2;
uniform vec4 u_lightColor3;

uniform vec4 u_diffuse;
uniform vec4 u_ambient;
uniform vec2 u_bumpiness;

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_texcoord0 = (u_texTransform0 * vec4(a_texcoord0.xy, 0.0, 1.0)).xy;
    mat3 model3 = mat3(u_model);
    vec3 N = normalize(model3 * a_normal);
    vec3 T = normalize(model3 * a_tangent);
    vec3 B = normalize(model3 * a_bitangent);
    v_normal = N;
    v_tangent = T;
    v_bitangent = B;
    mat3 TBN = mat3(T, B, N);
    vec3 L0 = normalize(u_lightDir0);
    vec3 L1 = normalize(u_lightDir1);
    vec3 L2 = normalize(u_lightDir2);
    vec3 L3 = normalize(u_lightDir3);
    v_lightDirTS0 = TBN * L0;
    v_lightDirTS1 = TBN * L1;
    v_lightDirTS2 = TBN * L2;
    v_lightDirTS3 = TBN * L3;
    float NdotL0 = max(dot(N, L0), 0.0) * u_lightColor0.w;
    float NdotL1 = max(dot(N, L1), 0.0) * u_lightColor1.w;
    float NdotL2 = max(dot(N, L2), 0.0) * u_lightColor2.w;
    float NdotL3 = max(dot(N, L3), 0.0) * u_lightColor3.w;
    vec3 col =
        u_lightColor0.rgb * NdotL0 +
        u_lightColor1.rgb * NdotL1 +
        u_lightColor2.rgb * NdotL2 +
        u_lightColor3.rgb * NdotL3;
    col *= a_color.rgb * u_diffuse.rgb;
    col += u_ambient.rgb;
    v_color = vec4(col, 1.0);
}
```

---

**Summary Table**

| Step | What to Add/Change          | Why                                 |
|------|----------------------------|-------------------------------------|
| 1    | Vertex attributes           | Needed for tangent space lighting   |
| 2    | Shader uniforms             | Provide transformation & lighting   |
| 3    | Transform to world space    | Accurate lighting calculations      |
| 4    | Build TBN matrix            | Convert between spaces              |
| 5    | Compute light in TS         | For per-pixel bump mapping          |
| 6    | Per-vertex diffuse lighting | Matches DX8 pipeline                |
| 7    | Output all needed varyings  | Data for fragment shader            |
| 8    | Update fragment shader      | Use new data for effects            |

---

**Follow these steps to update your BGFX shader to achieve feature parity with the DX8 bump diffuse shader.**