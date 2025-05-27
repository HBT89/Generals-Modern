### Step 1: Inventory and Analyze All Effects

#### **A. Inventory Table**

| Effect Name      | Vertex Shader(s) (.vsh)                | Pixel Shader(s) (.psh)                | C++ Class (.cpp/.h)                |
|------------------|----------------------------------------|---------------------------------------|------------------------------------|
| Simple           | (likely shdsimple.cpp/h, no .vsh/.psh) | (none)                                | shdsimple.cpp/h                    |
| Bump Diffuse     | shd6bumpdiff.vsh, shd7bumpdiffpass0.vsh, shd7bumpdiffpass1.vsh, shd8bumpdiff.vsh, shd8ssbumpdiff.vsh | shd8bumpdiff.psh, shd8ssbumpdiff.psh | shd6bumpdiff.cpp/h, shd7bumpdiff.cpp/h, shd8bumpdiff.cpp/h |
| Bump Specular    | shd6bumpspec.vsh, shd7bumpspecpass0.vsh, shd7bumpspecpass1.vsh, shd8bumpspec.vsh, shd8ssbumpspec.vsh | shd8bumpspec.psh, shd8ssbumpspec.psh | shd6bumpspec.cpp/h, shd7bumpspec.cpp/h, shd8bumpspec.cpp/h |
| Gloss Mask       | (none found, likely in C++ only)        | (none found)                          | shdglossmask.cpp/h                 |
| Cubemap          | (none found, likely in C++ only)        | (none found)                          | shdcubemap.cpp/h                   |
| Submesh/Renderer | (none)                                 | (none)                                | shdsubmesh.cpp/h, shdrenderer.cpp/h|
| Misc/Legacy      | (none)                                 | (none)                                | shdlegacyw3d.cpp/h, shdlib.cpp/h   |

- Some effects (like "simple", "glossmask", "cubemap") may not have explicit .vsh/.psh files and are implemented in C++ or use fixed-function pipeline logic.
- The "bumpdiff" and "bumpspec" effects have multiple versions (6, 7, 8, ss), likely for different hardware generations or passes.

---

### Step 2: Recreate Shaders as BGFX Shaders

For each effect:
- Create `vs_<effect>.sc` and `fs_<effect>.sc` in `wwshade2bgfx`.
- Use the old `.vsh`/`.psh` as a reference for math, lighting, and texture logic.
- Implement using BGFX conventions for attributes, varyings, and uniforms.
- Use `varying.def.sc` for shared varyings.

**Example mapping:**
- `shd8bumpdiff.vsh/psh` → `vs_bumpdiff.sc` / `fs_bumpdiff.sc`
- `shd8bumpspec.vsh/psh` → `vs_bumpspec.sc` / `fs_bumpspec.sc`
- `shdsimple.cpp/h`      → `vs_simple.sc` / `fs_simple.sc` (logic ported from C++)

---

### Step 3: Compile BGFX Shaders for Windows

Once you have written the `.sc` files:
- Use your `Tools/shaderc.exe` to compile each shader for Windows.

**Example PowerShell command:**

```powershell
# Compile vertex shader
& ..\..\..\Tools\shaderc.exe -f vs_bumpdiff.sc -o vs_bumpdiff.bin --type vertex --platform windows --profile s_5_0 --varyingdef varying.def.sc

# Compile fragment shader
& ..\..\..\Tools\shaderc.exe -f fs_bumpdiff.sc -o fs_bumpdiff.bin --type fragment --platform windows --profile s_5_0 --varyingdef varying.def.sc
```
- Adjust paths as needed for your directory structure.
- Repeat for each shader pair (`vs_*.sc`/`fs_*.sc`).
- The output `.bin` files are what you will load in your engine with BGFX.

---

### Step 4: Integrate and Refactor C++ Classes

- After compiling and validating the new BGFX shaders, refactor the corresponding C++ classes to:
  - Remove all DX8/D3D8 code.
  - Use BGFX API for setting uniforms, binding textures, and submitting draw calls.
  - Reference the new BGFX shader binaries.

---

### Step 5: Test and Iterate

- Test each effect in-game.
- Compare visuals to the original.
- Adjust shaders and C++ integration for parity.

---

**Summary:**
- Inventory all effects and their shaders/classes.
- Write new BGFX shaders for each effect, using the old ones as logic reference.
- Compile all new shaders with `shaderc.exe` for Windows.
- Refactor C++ classes to use the new BGFX shaders and pipeline.
- Test and iterate for visual parity and performance.
