# Raytracer — G-OOP-400

A modular raytracer written in C++20 for Epitech, built around a dynamic plugin system. Every primitive, light, and material is a separately compiled `.so` loaded at runtime — adding a new shape is one file and one CMake line.

---

## What is a raytracer?

For each pixel of the image, a ray is fired from the camera through that pixel into the scene. The program finds the closest object it hits, then calculates the color based on the material and all light sources. Reflections, refractions, and shadows each fire additional rays recursively. The result is written as a PPM image.

---

## Build

**Requirements:** `g++ ≥ 11` (C++20), `cmake ≥ 3.16`, `libconfig++`, `libdl` (bundled with glibc). Optional: `SFML 3` for live preview.

```bash
make           # Release build (default)
make re        # Full rebuild
make clean     # Remove build artefacts
make fclean    # Also remove binary and plugins
```

Output: `./raytracer` at the root, plugins in `./plugins/*.so`.

---

## Usage

```bash
./raytracer scenes/demo_phong.cfg > output.ppm   # render to file (no window)
./raytracer scenes/demo_phong.cfg                # live SFML preview (if built)
./raytracer --help
```

- Exit `0` on success, `84` on any error.
- All error messages go to `stderr`, the PPM image to `stdout`.
- When SFML is available and stdout is a terminal, a live window shows the render progressing tile by tile. Press **ESC** or close the window to abort.

---

## Scene file format

Scenes use [libconfig++](http://hyperrealm.github.io/libconfig/) syntax. All fields shown below are mandatory unless marked *optional*.

```cfg
camera:
{
    position = { x = 0.0; y = 3.0; z = 8.0; };
    lookAt   = { x = 0.0; y = 0.5; z = 0.0; };
    up       = { x = 0.0; y = 1.0; z = 0.0; };
    fov    = 50.0;    // degrees
    width  = 800;
    height = 600;
};

// Optional antialiasing
antialiasing:
{
    samples   = 3;     // samples per side: 1=off, 2=4spp, 3=9spp …
    adaptive  = false; // only refine high-variance (edge) pixels
    threshold = 0.05;  // variance trigger for adaptive mode [0,1]
};

// Named materials — reference them by name in primitives
materials:
{
    red_phong: {
        type = "phong";
        color = { r = 0.8; g = 0.1; b = 0.1; };
        ka = 0.12; kd = 0.70; ks = 0.40; shininess = 80.0;
    };
    mirror_mat: {
        type = "mirror";
        color = { r = 0.95; g = 0.95; b = 0.95; };
        reflectivity = 0.92;
        ka = 0.05; kd = 0.10; ks = 0.80; shininess = 256.0;
    };
};

primitives:
{
    spheres = (
        { center = { x = 0.0; y = 0.5; z = 0.0; }; radius = 0.5;
          material = "red_phong";
          translate = { x = 1.0; y = 0.0; z = 0.0; };  // optional
          rotate    = { x = 0.0; y = 45.0; z = 0.0; }; // optional, degrees
        }
    );
    planes = (
        { normal = { x = 0.0; y = 1.0; z = 0.0; }; distance = 0.0;
          material = "red_phong"; }
    );
    // Also: cylinders, cones — same translate/rotate/material fields
};

// Optional groups — shared transform applied to all children
groups = (
    {
        translate = { x = 3.0; y = 0.0; z = 0.0; };
        rotate    = { x = 0.0; y = 45.0; z = 0.0; };
        children:
        {
            spheres = ( { center = { x=0; y=0.5; z=0; }; radius=0.5;
                          material="red_phong"; } );
        };
    }
);

lights:
{
    ambient = (
        { color = { r = 1.0; g = 1.0; b = 1.0; }; intensity = 0.15; }
    );
    directional = (
        { direction = { x = -1.0; y = -2.0; z = -1.0; };
          color = { r = 1.0; g = 0.96; b = 0.88; }; intensity = 0.85; }
    );
    point = (
        { position  = { x = 2.0; y = 4.0; z = 2.0; };
          color     = { r = 1.0; g = 0.85; b = 0.6; };
          intensity = 4.0;
          constant  = 1.0; linear = 0.14; quadratic = 0.07; } // optional
    );
};
```

---

## Primitives

| Type | Config key | Required fields |
|------|-----------|-----------------|
| Sphere | `spheres` | `center {x,y,z}`, `radius` |
| Plane | `planes` | `normal {x,y,z}`, `distance` |
| Cylinder | `cylinders` | `center {x,y,z}`, `radius`, `height` |
| Cone | `cones` | `center {x,y,z}`, `radius`, `height` |

All primitives accept optional `translate`, `rotate`, and `material` fields.

---

## Materials

| Type | Description | Key fields |
|------|-------------|------------|
| `flat_color` | Uniform color, no shading | `color {r,g,b}` |
| `phong` | Diffuse + specular highlights | `color`, `ka`, `kd`, `ks`, `shininess` |
| `mirror` | Reflective surface | `color`, `reflectivity` (0–1), phong params |
| `glass` | Refractive transparent | `transparency` (0–1), `ior` (1.0–3.0), `color` |
| `checkerboard` | Procedural checkerboard | `color1`, `color2`, `scale`, phong params |
| `perlin` | Procedural Perlin noise | `color1`, `color2`, `scale`, `octaves`, `mode` (`noise`/`turbulence`/`marble`) |
| `texture` | Image from PPM file | `file` (path), `scale_u`, `scale_v`, phong params |

All materials except `flat_color` support `ka`, `kd`, `ks`, `shininess` for Phong shading.

---

## Lights

| Type | Description | Key fields |
|------|-------------|------------|
| `ambient` | Uniform global light | `color`, `intensity` |
| `directional` | Parallel light from infinite distance | `direction {x,y,z}`, `color`, `intensity` |
| `point` | Positional light with optional falloff | `position {x,y,z}`, `color`, `intensity`, `constant`, `linear`, `quadratic` |

Multiple lights of any type can be combined in each scene.

---

## How it works — step by step

**0 — Architecture (foundations)**
The core has no geometry — it only defines abstract interfaces (`IPrimitive`, `ILight`, `IMaterial`, `INode`) and a dynamic plugin loader. Every shape and material is a `.so` compiled separately. `libdl` loads them at startup, `libconfig++` reads the scene file.

**1 — Camera and rays**
A pinhole camera model converts each pixel `(x, y)` into a 3D ray using the camera position, look-at point, and field of view. The ray is `origin + t * direction`.

**2 — Intersection**
For each ray, the scene finds the closest hit across all objects. Each primitive solves its own geometric equation (e.g., sphere: quadratic formula, plane: linear, cylinder: quadratic on XZ, cone: quadratic with varying radius).

**3 — Lighting (Lambert + Phong)**
At the hit point, the renderer accumulates light contributions. Ambient light is constant. For each directional or point light: a shadow ray checks occlusion, then Lambert diffuse `(N·L)` and Phong specular `(R·V)^shininess` are computed. Point lights attenuate with distance: `1 / (c + l·d + q·d²)`.

**4 — Transforms**
Objects live in canonical local space (sphere centered at origin, etc.). A cached 4×4 transform matrix moves rays to local space before intersection and results back to world space. Normals use the transpose of the inverse: `(M⁻¹)ᵀ · n`.

**5 — BVH acceleration**
Objects are sorted into a binary tree by their bounding boxes. Each ray first tests the bounding box of a subtree — if it misses, the entire subtree is skipped. Reduces intersection work from O(N) to O(log N).

**6 — Multi-threaded rendering**
The image is split into 64×64 tiles. An atomic index distributes tiles to worker threads with no mutex. The scene is immutable after construction, so threads never conflict.

**7 — Reflections**
When a mirror material is hit, a reflected ray `r = d − 2(d·N)N` is fired. The returned color blends with the direct color according to `reflectivity`. Maximum 8 bounces.

**8 — Refractions**
Transparent materials (glass, water) deflect rays using Snell's law. The Schlick approximation handles the Fresnel blend between reflection and refraction near grazing angles. Total internal reflection is handled when the refraction angle exceeds 90°.

**9 — Procedural textures**
Checkerboard uses `floor(x) + floor(y) + floor(z)` parity in world space. Perlin noise uses Ken Perlin's improved lattice noise (2002) with a smoothstep interpolation; turbulence sums multiple octaves of |noise|; marble wraps turbulence inside a sine wave.

**10 — Antialiasing**
Uniform supersampling fires N×N rays per pixel on a stratified grid and averages them. Adaptive mode starts with 2×2; if the four samples differ by more than a threshold, it upgrades to N×N. Smooth areas stay at 4 samples, edges get full quality.

**11 — SFML live preview**
When stdout is a terminal and SFML 3 is available, a window opens and tiles appear as they're computed. ESC or closing the window cancels the render. When piping to a file, the window is suppressed.

---

## Architecture overview

```
raytracer (binary)
├── src/core/        Camera, Renderer (tiles, threads, AA), Scene, SceneBuilder (Builder pattern)
├── src/interfaces/  IPrimitive, INode (Composite), ILight, IMaterial (Strategy), plugin ABI
├── src/math/        Vector2/3, Color, Matrix4, Transform (cached inverse)
├── src/parser/      SceneParser — libconfig++ → SceneBuilder
├── src/plugins/     PluginLoader — dlopen/dlsym encapsulation
└── src/output/      PPMWriter, SFMLDisplay

plugins/ (loaded at runtime, never linked to the binary)
├── sphere, plane, cylinder, cone          — primitives
├── ambient, directional, point_light      — lights
├── flat_color, phong, mirror, glass       — materials
└── checkerboard, perlin, texture          — procedural/file textures
```

Design patterns used: **Factory** (plugin system), **Composite** (scene graph `INode`/`GroupNode`), **Builder** (`SceneBuilder`), **Strategy** (`IMaterial::shade`).

---

## Authors

- Léo Batilliot
- Gabin Wilson
- Mathis Korejezak
