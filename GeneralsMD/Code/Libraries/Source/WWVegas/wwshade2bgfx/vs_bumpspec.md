# Directions: Updating BGFX Bump Specular Shader to Match DX8 Logic

## 1. Add Required Vertex Attributes

Ensure your shader receives the following per-vertex attributes:
- `a_position` (vec3): Vertex position
- `a_normal` (vec3): Vertex normal
- `a_tangent` (vec3): Tangent vector (S)
- `a_bitangent` (vec3): Bitangent vector (T)
- `a_texcoord0` (vec2): Texture coordinates
- *(Optional but recommended)*: Vertex color

## 2. Declare Required Uniforms

Add these uniforms to your shader:
- `u_modelViewProj` (mat4): Model-View-Projection matrix
- `u_model` (mat4): Model (world) matrix
- `u_eyePos` (vec3): Camera/eye position in world space
- `u_lightDir0` (vec3): Light direction in world space (add more for additional lights as needed)
- `u_lightColor0` (vec4): Light color/attenuation (add more for additional lights)
- *(Optional)*: Material parameters (diffuse, ambient, specular, bumpiness)

## 3. Transform Inputs to World Space

In your main vertex shader function:
- Transform the vertex position to world space:  
  `worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));`
- Transform the normal, tangent, and bitangent to world space (and normalize):
  ```
  N = normalize(vec3(mul((mat3)u_model, a_normal)));
  T = normalize(vec3(mul((mat3)u_model, a_tangent)));
  B = normalize(vec3(mul((mat3)u_model, a_bitangent)));
  ```

## 4. Build the TBN Matrix

- Construct the tangent space basis matrix:
  ```glsl
  mat3 TBN = mat3(T, B, N);
  ```

## 5. Compute Light and View Vectors in Tangent Space

- Calculate light direction in world space and normalize:
  `vec3 L = normalize(u_lightDir0);`
- Calculate view direction in world space:
  `vec3 V = normalize(u_eyePos - worldPos);`
- Transform both to tangent space:
  ```
  v_lightDirTS = TBN * L;
  v_viewDirTS  = TBN * V;
  ```

## 6. Output All Required Data to the Fragment Shader

- Output these varyings:
  - `v_texcoord0` (pass-through from `a_texcoord0`)
  - `v_normal`, `v_tangent`, `v_bitangent` (world space, for per-fragment calculations if needed)
  - `v_lightDirTS` (light vector in tangent space)
  - `v_viewDirTS` (view vector in tangent space)
  - *(Optional)*: `v_color` (vertex color or calculated lighting)

## 7. Update the Fragment Shader

- Ensure it uses `v_lightDirTS`, `v_viewDirTS`, and your normal/gloss maps to perform per-pixel bump/specular/gloss calculations, replicating the DX8 pixel pipeline logic.

---

## Example BGFX GLSL Vertex Shader

```glsl
$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_normal, v_tangent, v_bitangent, v_texcoord0
$output v_lightDirTS, v_viewDirTS, v_color

#include <bgfx_shader.sh>

uniform mat4 u_modelViewProj;
uniform mat4 u_model;
uniform vec3 u_eyePos;
uniform vec3 u_lightDir0;
uniform vec4 u_lightColor0;

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec3 worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));
    vec3 N = normalize(vec3(mul((mat3)u_model, a_normal)));
    vec3 T = normalize(vec3(mul((mat3)u_model, a_tangent)));
    vec3 B = normalize(vec3(mul((mat3)u_model, a_bitangent)));
    mat3 TBN = mat3(T, B, N);
    vec3 L = normalize(u_lightDir0);
    vec3 V = normalize(u_eyePos - worldPos);
    v_lightDirTS = TBN * L;
    v_viewDirTS  = TBN * V;
    v_normal = N;
    v_tangent = T;
    v_bitangent = B;
    v_texcoord0 = a_texcoord0;
    v_color = vec4(1.0); // Replace if using vertex color or calculated value
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
| 5    | Compute light/view in TS    | For per-pixel bump/specular         |
| 6    | Output all needed varyings  | Data for fragment shader            |
| 7    | Update fragment shader      | Use new data for effects            |

---

**Follow these steps to update your BGFX shader to achieve feature parity with the DX8 bump specular shader.**
