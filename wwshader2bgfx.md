## 1. **What Needs to Be Recreated or Replaced**

### **A. Shader Logic (vsh/psh → .sc)**

- **Every effect** (diffuse, bump, specular, gloss, cubemap, etc.) that was implemented as a `.vsh` (vertex shader) and `.psh` (pixel shader) in `wwshade` must be **recreated as BGFX-compatible shaders** (GLSL/HLSL/Metal, typically `.sc` for BGFX).
- **You do NOT need to port every .cpp/.h file as a shader.**
    - The `.cpp`/`.h` files are C++ logic for shader management, parameter passing, and engine integration.
    - The **actual GPU code** is in the `.vsh`/`.psh` files, which must be rewritten for BGFX.

### **B. Shader Management and Integration (C++ side)**

- The C++ classes in `wwshade` (e.g., `ShdSimpleClass`, [Shd8BumpDiffClass](vscode-file://vscode-app/c:/Users/joshu/AppData/Local/Programs/Microsoft%20VS%20Code/resources/app/out/vs/code/electron-sandbox/workbench/workbench.html), etc.) are responsible for:
    - Managing shader parameters (textures, colors, matrices)
    - Setting up render state
    - Selecting and binding the correct shader for each material/pass
- **These classes must be refactored** to:
    - Remove all DX8/D3D8 logic
    - Use BGFX API for setting uniforms, binding textures, and submitting draw calls
    - Reference the new BGFX shader binaries (compiled `.bin` files)

---

## 2. **How to Recreate the Library in `wwshade2bgfx`**

### **Step 1: Inventory and Analyze All Effects**

- List all unique shader effects in `wwshade` (e.g., simple, bumpdiff, bumpspec, glossmask, cubemap, etc.).
- For each, identify:
    - The `.vsh` and `.psh` files (GPU code)
    - The corresponding C++ class (e.g., [Shd8BumpDiffClass](vscode-file://vscode-app/c:/Users/joshu/AppData/Local/Programs/Microsoft%20VS%20Code/resources/app/out/vs/code/electron-sandbox/workbench/workbench.html))

### **Step 2: Recreate Shaders as BGFX Shaders**

- For each effect:
    1. **Write a new vertex shader** (`vs_<effect>.sc`) and fragment shader (`fs_<effect>.sc`) in BGFX’s shader language.
        - Use the old `.vsh`/`.psh` as a **reference for logic only** (math, lighting, texture usage).
        - Implement using BGFX conventions for attributes, varyings, and uniforms.
        - Use [varying.def.sc](vscode-file://vscode-app/c:/Users/joshu/AppData/Local/Programs/Microsoft%20VS%20Code/resources/app/out/vs/code/electron-sandbox/workbench/workbench.html) to define shared varyings.
    2. **Compile** these shaders with `shaderc.exe` for your target platforms.
>> YOU ARE HERE:
>> All major BGFX shader stubs in `wwshade2bgfx` have now been expanded into minimally functional, resolution-independent GLSL/HLSL logic for 1440p and beyond:

- **Bump Diffuse/Specular (multi-pass):** Now output tangent/bitangent, perform tangent-space lighting, normal mapping, and (for specular) a basic highlight.
- **Simple:** Now does basic diffuse lighting.
- **Gloss Mask:** Uses a gloss mask texture for per-pixel glossiness.
- **Cubemap:** Samples a cubemap for environment mapping and blends with the base texture.

These shaders are ready for further effect-specific logic and can be compiled and tested at 2560x1440 or higher. You can now proceed to test, refine, and add more advanced features as needed for full parity with the original effects. Let me know if you want to generate batch compile commands or need help with a specific effect!
### **Step 3: Update/Refactor C++ Shader Classes**

- For each effect’s C++ class:
    - Remove all DX8/D3D8 code (shader creation, state setting, etc.).
    - Add logic to:
        - Load the compiled BGFX shader binaries.
        - Set BGFX uniforms (matrices, colors, etc.) and bind textures.
        - Submit draw calls using BGFX.
    - Ensure all shader parameters (textures, colors, matrices) are passed as BGFX uniforms.

### **Step 4: Integrate with Engine**

- Update the renderer and mesh/submesh classes to:
    - Use the new BGFX shader classes.
    - Set up vertex/index buffers in BGFX format.
    - Call BGFX submit with the correct program and state.

### **Step 5: Test and Iterate**

- Test each effect in-game.
- Compare visuals to the original.
- Adjust shaders and C++ integration for parity.

---

## 3. **Do You Need to Port Every .cpp/.h File?**

- **No.** Only port/refactor the C++ files that implement shader logic, parameter management, and rendering integration.
- **You must** rewrite every unique shader effect as a new BGFX shader pair (`vs_*.sc`/`fs_*.sc`).
- **You do not** need to port utility, loader, or legacy DX8-specific files unless they are still relevant for BGFX.

---

## 4. **Example Mapping**

|Old WWShade File(s)|New BGFX Equivalent(s) in `wwshade2bgfx`|
|---|---|
|`shd8bumpdiff.vsh/psh`|`vs_bumpdiff.sc`, `fs_bumpdiff.sc`|
|`shd8bumpspec.vsh/psh`|`vs_bumpspec.sc`, `fs_bumpspec.sc`|
|`shdsimple.cpp/h`|`vs_simple.sc`, `fs_simple.sc` + refactored C++|
|`shdglossmask.cpp/h`|`vs_glossmask.sc`, `fs_glossmask.sc` + refactored C++|
|...|...|

---

## 5. **Summary Table**

|Task|Where/How|
|---|---|
|Recreate all unique shader effects|New BGFX `.sc` shaders in `wwshade2bgfx`|
|Refactor C++ shader/material classes|Remove DX8, use BGFX API, bind new shaders|
|Remove/replace DX8-specific code|All engine and shader management code|
|Compile new shaders|Use `shaderc.exe` for all platforms|
|Integrate and test|Update renderer, mesh, and material logic|

---

## 6. **Key Points**

- **Every effect** must have a new BGFX shader pair and C++ integration.
- **Not every .cpp/.h** becomes a shader; only those that implement GPU logic.
- **All DX8/D3D8 code must be removed** from the runtime.
- **Testing and visual parity** are essential—use the old game as a reference.

---

## 7. **References**

- See your new starter shaders in [`wwshade2bgfx`](vscode-file://vscode-app/c:/Users/joshu/AppData/Local/Programs/Microsoft%20VS%20Code/resources/app/out/vs/code/electron-sandbox/workbench/workbench.html).
- Use the old `.vsh`/`.psh` files in [`wwshade`](vscode-file://vscode-app/c:/Users/joshu/AppData/Local/Programs/Microsoft%20VS%20Code/resources/app/out/vs/code/electron-sandbox/workbench/workbench.html) as **logic/math reference only**.