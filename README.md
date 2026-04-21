# Raytracer

Moteur de raytracing C++ modulaire (projet Epitech **G-OOP-400-LIL-4-1-raytracer-16**), architecturé autour d'un cœur minimal et d'un système de plugins dynamiques (`libdl`).

> 📚 **Documentation du projet**
>
> - [`PROJECT.md`](./PROJECT.md) — hub descriptif : philosophie, exigences du sujet, vue d'ensemble.
> - [`PLAN.md`](./PLAN.md) — feuille de route technique : phases, tickets, critères d'acceptation.
> - [`DOCUMENTATION.md`](./DOCUMENTATION.md) — journal technique : historique des versions, choix, patches.

---

## 🚀 Build

### Prérequis

- Compilateur C++20 (`g++ ≥ 11` ou `clang++ ≥ 13`)
- `cmake ≥ 3.16`
- `libconfig++`
- `libdl` (fourni par la libc)
- *(optionnel, bonus)* `SFML` pour l'affichage temps réel

### Compilation

```bash
make           # Build Release par défaut (via CMake)
make re        # Rebuild complet
make clean     # Supprime les artefacts de compilation
make fclean    # Supprime aussi le binaire et les plugins
```

Ou directement avec CMake :

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

Après compilation :

```
./raytracer          # exécutable à la racine
./plugins/*.so       # plugins chargés dynamiquement au runtime
```

---

## 📖 Usage

```bash
./raytracer --help
USAGE: ./raytracer <SCENE_FILE>
  SCENE_FILE: scene configuration
```

Exemple :

```bash
./raytracer scenes/demo_sphere.cfg > output.ppm
```

Le raytracer écrit l'image au format **PPM (P6 binaire)** sur la sortie standard, ou dans un fichier selon le mode d'utilisation prévu.

Codes de retour :

- `0` — succès
- `84` — erreur (argument invalide, fichier introuvable, erreur de parsing, etc.)

---

## 🗂️ Structure du dépôt

```
.
├── CMakeLists.txt         # build system (racine)
├── Makefile               # wrapper vers CMake
├── raytracer              # binaire (généré)
├── plugins/               # plugins .so (générés)
├── src/
│   ├── main.cpp
│   ├── core/              # Scene, Renderer, Camera, SceneBuilder
│   ├── math/              # Vector3, Matrix4, Color, Transform
│   ├── interfaces/        # IPrimitive, ILight, IMaterial, plugin ABI
│   ├── primitives/        # (code commun aux plugins de primitives)
│   ├── lights/            # (code commun aux plugins de lumières)
│   ├── materials/         # (code commun aux plugins de matériaux)
│   ├── parser/            # SceneParser (libconfig++)
│   ├── plugins/           # PluginLoader (libdl)
│   └── output/            # PPMWriter, SFMLDisplay (bonus)
├── scenes/                # scènes de démo (.cfg)
├── screenshots/           # rendus de référence (.ppm)
├── tests/                 # tests unitaires
├── bonus/                 # code bonus séparable
├── PROJECT.md              # hub descriptif du projet
├── PLAN.md                # roadmap détaillée
└── DOCUMENTATION.md       # historique des versions
```

---

## 🎨 Format des scènes

Les scènes utilisent la syntaxe `libconfig++`. Exemple minimal :

```cfg
camera:
{
    resolution = { width = 1920; height = 1080; };
    position = { x = 0; y = -100; z = 20; };
    rotation = { x = 0; y = 0; z = 0; };
    fieldOfView = 72.0;
};

materials = (
    { name = "red_matte"; type = "flat"; color = { r = 255; g = 64; b = 64; }; }
);

primitives:
{
    spheres = (
        { x = 0; y = 0; z = 0; r = 20; material = "red_matte"; }
    );
};

lights:
{
    ambient = 0.3;
    directional = (
        { direction = { x = -1; y = -1; z = -1; }; color = { r = 255; g = 255; b = 255; }; }
    );
};
```

Les scènes disponibles sont listées dans [`scenes/`](./scenes/). Leurs rendus de référence sont dans [`screenshots/`](./screenshots/).

---

## 🧩 Architecture

Le moteur repose sur :

- **Interfaces C++ pures** : `IPrimitive`, `ILight`, `IMaterial`, `INode`.
- **Plugins dynamiques** : chaque primitive / lumière / matériau est compilé en `.so` chargé au runtime par un `PluginLoader` qui encapsule `libdl`.
- **Scene graph Composite** : les objets forment un arbre (`GroupNode` + `LeafNode`), même interface pour les cas simples et complexes.
- **Design patterns** : Factory (plugins), Composite (scene graph), Builder (parser), Strategy (matériaux).

L'exécutable `raytracer` **n'est pas linké** aux plugins : `ldd ./raytracer` ne doit jamais lister de `raytracer_*.so`.

Voir [`PROJECT.md`](./PROJECT.md) et [`PLAN.md`](./PLAN.md) pour les détails.

---

## 👤 Auteurs

- **Léo Batilliot**
- **Gabin Wilson**
- **Mathis Korejezak**
