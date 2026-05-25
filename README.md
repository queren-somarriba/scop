# scop

A 3D model viewer built with OpenGL, supporting `.obj` and `.mtl` files, Phong lighting, textures, and interactive rotation.

https://github.com/user-attachments/assets/8fb05f97-07eb-4662-b011-62e7bc82da0a

## Dependencies

- OpenGL 3.3+
- GLFW
- GLAD

## Build

```bash
make
```

## Usage

```bash
./scop <file.obj>
```

## Controls

| Key | Action |
|---|---|
| `W` / `S` | Move forward / backward |
| `A` / `D` | Move left / right |
| `↑` / `↓` | Move up / down |
| `Left click + mouse` | Rotate the model |
| `Scroll` | Adjust specular shininess |
| `Space` | Toggle automatic rotation |
| `T` | Toggle texture / random face colors |
| `←` / `→` | Adjust Z clipping plane (Ztrunc) |
| `Escape` | Quit |

## Features

- Custom `.obj` and `.mtl` parser (no external library)
- Multi-mesh and multi-material support
- Phong lighting (ambient + diffuse + specular)
- UV texturing or automatic triplanar mapping for models without UV coordinates
- Smooth transition between textured and per-face color modes
- Automatic Y-axis rotation
- Automatic model normalization (centering + scaling)
- Automatic normal generation if absent from the `.obj` file
- Interactive Z clipping plane

## Structure

```
.
├── src/
│   ├── main.cpp              # Entry point
│   ├── parser.cpp            # OBJ / MTL parsing
│   ├── GeometryProcessor.cpp # VBO/EBO construction, triangulation
│   ├── Renderer.cpp          # OpenGL render loop
│   ├── InputHandler.cpp      # Keyboard, mouse, scroll
│   └── utils.cpp             # Utilities
├── shaders/
│   ├── Texture.vs            # Vertex shader
│   └── Texture.fs            # Fragment shader (Phong + triplanar)
└── assets/
    └── models/
        └── textures/         # BMP textures
```

## Supported OBJ Format

- Vertices (`v`), normals (`vn`), UV coordinates (`vt`)
- Triangle and polygon faces (automatic fan triangulation)
- Groups (`o`, `g`), materials (`usemtl`, `mtllib`)
- MTL properties: `Ka`, `Kd`, `Ks`, `Ke`, `Ns`, `illum`, `map_Ka`, `map_Ks`, `map_Kd`

  https://github.com/user-attachments/assets/c7afa5ad-39c3-4333-98ba-15a5a6076cf1



https://github.com/user-attachments/assets/d848c459-fa7f-4b43-8e91-85ab90399358



https://github.com/user-attachments/assets/0f6d058a-75fb-4081-8a34-14aeabeaac0e



https://github.com/user-attachments/assets/b47a1292-ba81-44a4-8b3d-b945d2dc924f


