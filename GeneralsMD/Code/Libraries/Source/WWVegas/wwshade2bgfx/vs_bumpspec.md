The old DX8 [[shd8bumpspec.vsh]] shader handles tangent space basis transformation, lighting vectors, and outputs several interpolators for per-pixel lighting. Here’s a summary of what needs to be addressed and a draft translation to modern BGFX GLSL-style code.

Key points from the DX8 shader
Inputs:
Vertex position, normal, color, texture coords, tangent (S), bitangent (T), and SxT (cross product of S and T).
Uniforms:
Model/view/projection, world matrix, light directions, light colors, material parameters, eye position, etc.
Outputs:
Transform basis to world, compute tangent space light direction, normalize, output various lighting factors, compute and output specular, etc.
You need these in your BGFX shader:
Vertex Attributes:

a_position, a_normal, a_texcoord0
Add: a_tangent, a_bitangent (and possibly color)
Uniforms:

Model/view/projection
World matrix
Light directions/colors
Eye position
Material parameters (diffuse, ambient, specular, bumpiness)
Varyings:

Pass tangent, bitangent, normal to fragment shader
Pass tangent-space light vector, view vector
Pass texcoords
Example: BGFX GLSL Vertex Shader
Below is a starting point for your BGFX vertex shader, matching the DX8 logic as closely as possible.
Note: You’ll need to set up all required uniforms and attributes in your engine code as well.

GLSL
// BGFX vertex shader for bump specular effect with gloss map
$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_normal, v_tangent, v_bitangent, v_texcoord0
$output v_lightDirTS, v_viewDirTS, v_color

#include <bgfx_shader.sh>

uniform mat4 u_modelViewProj;
uniform mat4 u_model;
uniform mat4 u_world; // or u_model if that’s world
uniform vec3 u_eyePos; // world space eye position
uniform vec3 u_lightDir0; // world space
uniform vec4 u_lightColor0;
// Add uniforms for more lights as needed...

void main()
{
    // Transform position to clip space
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    
    // Transform vertex position to world space
    vec3 worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));

    // Transform normal, tangent, bitangent to world space
    vec3 N = normalize(vec3(mul((mat3)u_model, a_normal)));
    vec3 T = normalize(vec3(mul((mat3)u_model, a_tangent)));
    vec3 B = normalize(vec3(mul((mat3)u_model, a_bitangent)));

    // Build TBN matrix
    mat3 TBN = mat3(T, B, N);

    // Light direction (assumed normalized, world space)
    vec3 L = normalize(u_lightDir0);

    // Vector from vertex to eye (in world space)
    vec3 V = normalize(u_eyePos - worldPos);

    // Transform light and view to tangent space
    v_lightDirTS = TBN * L;
    v_viewDirTS  = TBN * V;

    // Pass normal, tangent, bitangent for possible per-fragment use
    v_normal = N;
    v_tangent = T;
    v_bitangent = B;

    // Pass through texcoords
    v_texcoord0 = a_texcoord0;

    // Add color if needed, or calculate lighting here if doing per-vertex
    v_color = vec4(1.0); // Placeholder, set to vertex color or calculated
}
Fragment Shader
The fragment shader will use v_lightDirTS, v_viewDirTS, and normal map to compute the per-pixel bump/spec/gloss effects, using the same math as your old pixel pipeline.

Action Steps
Modify your BGFX vertex shader as above:
Add tangent/bitangent attributes.
Add/declare necessary uniforms.
Transform and output tangent-space light and view vectors.
Pass all needed data to the pixel shader for per-pixel lighting.
Update your fragment shader to use these vectors and normal/gloss maps.
