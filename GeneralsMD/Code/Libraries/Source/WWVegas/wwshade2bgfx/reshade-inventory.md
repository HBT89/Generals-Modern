### Step 1: Inventory and Analyze All Effects

#### **A. Inventory Table**

| Effect Name      | Vertex Shader(s) (.vsh/.sc)                                   | Pixel/Fragment Shader(s) (.psh/.sc)                                   | C++ Class (.cpp/.h)                        | Notes |
|------------------|---------------------------------------------------------------|-----------------------------------------------------------------------|--------------------------------------------|-------|
| Simple           | (none, likely fixed function or handled in C++)               | (none)                                                                | shdsimple.cpp/h                            |       |
| Bump Diffuse     | shd6bumpdiff.vsh, shd7bumpdiffpass0.vsh, shd7bumpdiffpass1.vsh,<br>shd8bumpdiff.vsh, shd8ssbumpdiff.vsh,<br>vs_bumpdiff.sc | shd8bumpdiff.psh, shd8ssbumpdiff.psh,<br>fs_bumpdiff.sc, fs_bumpdiffpass0.sc, fs_bumpdiffpass1.sc | shd6bumpdiff.cpp/h, shd7bumpdiff.cpp/h,<br>shd8bumpdiff.cpp/h, shdbumpdiff.cpp/h      |       |
| Bump Specular    | shd6bumpspec.vsh, shd7bumpspecpass0.vsh, shd7bumpspecpass1.vsh,<br>shd8bumpspec.vsh, shd8ssbumpspec.vsh,<br>vs_bumpspec.sc | shd8bumpspec.psh, shd8ssbumpspec.psh,<br>fs_bumpspec.sc, fs_bumpspecpass0.sc, fs_bumpspecpass1.sc | shd6bumpspec.cpp/h, shd7bumpspec.cpp/h,<br>shd8bumpspec.cpp/h, shdbumpspec.cpp/h      |       |
| Gloss Mask       | (none found, likely in C++ only)                              | (none found)                                                          | shdglossmask.cpp/h                         |       |
| Cubemap          | (none found, likely in C++ only)                              | (none found)                                                          | shdcubemap.cpp/h                           |       |
| Legacy W3D       | (none, wraps old material system)                             | (none)                                                                | shdlegacyw3d.cpp/h                         | Legacy/fallback wrapper |
| Shader Def       | (none, base/utility)                                          | (none)                                                                | shddef.cpp/h, shddeffactory.cpp/h, shddefmanager.cpp/h | Shader definition, factory, and management |
| Mesh/Material    | (none, mesh/material utility)                                 | (none)                                                                | shdmesh.cpp/h, shdsubmesh.cpp/h            | Mesh/material helpers |
| Loader/Lib       | (none, shader loading/util)                                   | (none)                                                                | shdloader.cpp/h, shdlib.cpp                | Shader loading/utilities |
| Renderer         | (none, renderer utility)                                      | (none)                                                                | shdrenderer.cpp/h                          | Renderer interface |
| Dump/Debug       | (none, debug utility)                                         | (none)                                                                | shddump.h                                  | Debug/dump helpers |
| HW Constants     | (none, constants header)                                      | (none)                                                                | shdhw_constants.h                          | Hardware constants |
| Interface        | (none, interface header)                                      | (none)                                                                | shdinterface.cpp/h                         | Shader interface base |
| Class IDs        | (none, class id header)                                       | (none)                                                                | shdclassids.h                              | Class ID registry |
| Force Links      | (none, linker utility)                                        | (none)                                                                | shdforcelinks.cpp/h                        | Linker helpers |

---

#### **B. C++ Shader Pipeline Explainers**

- **shdbumpdiff.cpp / shdbumpdiff.h**: Implements the C++ class for the Bump Diffuse shader effect. Manages the bump diffuse pipeline, binding shaders, setting uniforms (light, color, bumpiness), and bridging engine/material data to the GPU for per-pixel bump diffuse lighting.

- **shdbumpspec.cpp / shdbumpspec.h**: Implements the C++ class for the Bump Specular shader effect. Manages the bump specular pipeline, including shader binding, uniform setup for light/specular/gloss, and integration with the engine's material and lighting system.

- **shdglossmask.cpp / shdglossmask.h**: C++ class for the Gloss Mask effect. Handles gloss mask pipeline, shader binding, gloss mask textures, and related uniforms for per-pixel gloss modulation.

- **shdcubemap.cpp / shdcubemap.h**: C++ class for Cubemap reflection. Manages cubemap pipeline, shader binding, cubemap textures, and passing view/normal vectors for real-time environment mapping.

- **shdsimple.cpp / shdsimple.h**: C++ class for the Simple effect. Manages the simplest pipeline, binding a basic shader and setting minimal uniforms for unlit/basic lit objects.

- **shdlegacyw3d.cpp / shdlegacyw3d.h**: Implements a legacy wrapper shader class. Its purpose is to wrap the old W3D material system, allowing legacy materials to be used in the new shader pipeline as a fallback. Intended for transition only.

- **shddef.cpp / shddef.h**: Base class for shader definitions. Provides the core interface and data structures for all shader types. Used by all effect-specific shader classes.

- **shddeffactory.cpp / shddeffactory.h**: Implements the factory for creating shader definition objects. Used to instantiate the correct shader class for each material/effect at runtime.

- **shddefmanager.cpp / shddefmanager.h**: Manages the lifecycle and registry of all shader definitions. Handles lookup, caching, and management of shader classes and their resources.

- **shdmesh.cpp / shdmesh.h**: Mesh/material utility classes. Provide helpers for mesh data, material assignment, and geometry setup for the shader pipeline.

- **shdsubmesh.cpp / shdsubmesh.h**: Submesh/material utility classes. Handle submesh partitioning and material assignment for complex objects.

- **shdloader.cpp / shdloader.h**: Shader loading and utility classes. Handle loading, parsing, and management of shader files and resources.

- **shdlib.cpp**: General shader library utilities. Provides shared code and helpers for the shader system.

- **shdrenderer.cpp / shdrenderer.h**: Renderer interface and helpers. Provide the main interface between the engine's renderer and the shader/material system.

- **shddump.h**: Debug/dump helpers for shader data and state. Used for diagnostics and debugging.

- **shdhw_constants.h**: Hardware constants header. Defines constant register indices and values for shaders.

- **shdinterface.cpp / shdinterface.h**: Base interface for all shader classes. Defines the core API for shader/material interaction.

- **shdclassids.h**: Class ID registry for shader types. Used for runtime type identification and factory lookup.

- **shdforcelinks.cpp / shdforcelinks.h**: Linker helpers to ensure all shader classes are linked into the final binary.

---

Each C++ class acts as the interface between the engine/material system and the BGFX shaders, handling all effect-specific state, uniform setup, and resource binding required for the corresponding visual effect or utility role.
