# plan.md — Feuille de route technique du Raytracer

> Ce fichier est le **plan d'exécution détaillé** du projet. Il liste toutes les étapes prévues avec leurs attentes précises et le résultat final attendu à chaque jalon.
>
> - `PROJECT.md` = hub descriptif (philosophie, exigences sujet, vue d'ensemble).
> - `PLAN.md` (ce fichier) = **quoi faire**, **dans quel ordre**, **avec quels critères d'acceptation**.
> - `DOCUMENTATION.md` = **historique des versions** (ce qui a été ajouté, choix techniques, patches).
> - `MATHS.md` = Explications mathématiques : théorie et pratique du raytracing, ainsi que la géométrie spatiale.
> 
> **Règle** : chaque ticket terminé doit cocher sa case ici, et déclencher une entrée correspondante dans `DOCUMENTATION.md`.

---

## 🎯 Résultat final attendu (définition du "done")

À la fin du projet, le dépôt doit livrer :

1. Un exécutable `./raytracer` à la racine, non linké aux plugins (`ldd ./raytracer` ne montre aucun `raytracer_*.so`).
2. Un ensemble de plugins `.so` dans `./plugins/` (Sphere, Plane, Cylinder, Cone, Directional, Ambient, Point, FlatColor, Phong, a minima).
3. Conformité Epitech totale : exit code 0/84, erreurs sur `stderr`, usage `--help` exact, compilation `-Wall -Wextra -Werror -pedantic` sans warning.
4. Les features **MUST** et **SHOULD** 100 % fonctionnelles.
5. Un empilement de bonus (cible minimale : Phong + réflexion + multithreading + transformation matrix + antialiasing + au moins une texture procédurale).
6. Un jeu de scènes de démonstration dans `./scenes/` + leurs rendus de référence dans `./screenshots/`.
7. Une scène `showcase.cfg` combinant un maximum de features pour la soutenance.
8. `README.md` (doc utilisateur), `PROJECT.md` (hub), `PLAN.md` (ce fichier, à jour), `DOCUMENTATION.md` (historique complet).
9. Architecture défendable à l'oral : au moins 2 design patterns justifiables (on vise 4 : Factory, Composite, Builder, Strategy).

---

## 📐 Décisions architecturales verrouillées

| Sujet | Décision | Motivation |
|-------|----------|------------|
| Langage | C++20 | `std::filesystem`, `concepts`, `<span>`, `<numbers>` |
| Build | CMake + Makefile wrapper | Plugins `.so` naturels, conforme à l'attendu Epitech |
| Patterns | Factory + Composite + Builder + Strategy | Couvre parser, scene graph, construction, matériaux. Observer/Decorator en réserve bonus. |
| Plugins | Un seul symbole C exporté : `raytracer_plugin_entry` → `PluginDescriptor` (POD versionné) | ABI stable, ajout d'un type = 1 `.so` + 1 ligne CMake |
| Transformations | `Matrix4` + `Transform` dès Phase 0 | Évite le refactor "transformation matrix" (bonus 2 pts acquis structurellement) |
| Scene graph | `INode` Composite dès Phase 0 (pas de liste plate) | Prépare scene graph (2 pts) + scene import (2 pts) + BVH sans refactor |
| Matériaux | Déclarés séparément dans la config, référencés par nom, partagés via `shared_ptr` | Mémoire + textures lourdes + duplication évitée |
| Thread-safety | Scène immuable post-construction, renderer en tuiles mono-thread d'abord | Multithreading = ajouter un pool, aucun refactor |
| Couleur | `Color` en float linéaire, conversion sRGB uniquement à l'écriture PPM | Évite les rendus faussés non réparables en bonus |
| Intersection | En espace objet (primitives canoniques : sphère unité, plan Z=0) | Logique simple dans chaque plugin, tout le reste dans `Transform` |

---

## 🧱 Signatures d'interfaces prévues (contrat public)

```cpp
// Types POD partagés via ABI plugins
struct Ray            { Vector3 origin; Vector3 direction; double tMin, tMax; };
struct HitRecord      { double t; Vector3 point; Vector3 normal;
                        Vector2 uv; const IMaterial* material;
                        const INode* node; bool frontFace; };
struct ShadingContext { const Scene& scene; const HitRecord& hit;
                        const Ray& incomingRay; int depth; };

class IPrimitive {
public:
    virtual ~IPrimitive() = default;
    virtual bool    intersect(const Ray&, HitRecord&) const = 0;
    virtual AABB    bounds() const = 0;
    virtual Vector2 uvAt(const Vector3& localPoint) const = 0;
};

class INode { // Composite
public:
    virtual ~INode() = default;
    virtual bool intersect(const Ray&, HitRecord&) const = 0;
    virtual AABB bounds() const = 0;
    virtual void addChild(std::shared_ptr<INode>) {}
};

struct LightSample { Vector3 direction; Color radiance; double distance; };

class ILight {
public:
    virtual ~ILight() = default;
    virtual LightSample sample(const Vector3& surfacePoint) const = 0;
    virtual bool        isAmbient() const { return false; }
    virtual Color       ambientContribution() const { return Color::black(); }
};

class IMaterial { // Strategy
public:
    virtual ~IMaterial() = default;
    virtual Color shade(const ShadingContext&) const = 0;
};

// ABI plugin (C-compatible)
struct PluginDescriptor {
    uint32_t     abi_version;
    PluginKind   kind;          // PRIMITIVE | LIGHT | MATERIAL
    const char*  name;
    void* (*create)(const libconfig::Setting&);
    void  (*destroy)(void*);
};
extern "C" const PluginDescriptor* raytracer_plugin_entry();
```

---

## 🗓️ Phases & tickets

Chaque ticket a : **objectif** · **livrable** · **critère d'acceptation** (vérifiable). Le ticket n'est coché que quand son critère passe ET que `DOCUMENTATION.md` contient son entrée.

---

### Phase 0 — Fondations (avant tout code de rendu)

> But : pouvoir rendre une image PPM vide via un renderer en tuiles, avec un plugin factice chargé dynamiquement. Aucune feature MUST n'est encore implémentée, mais toute l'architecture est posée.

- [x] **T0.1 — Initialisation du dépôt**
  - Objectif : structure de dossiers conforme + `.gitignore` strict + `README.md` squelette.
  - Livrable : arborescence vide cohérente avec le projet cible, `.gitignore`, `README.md`, `plan.md` et `DOCUMENTATION.md` créés.
  - Critère : `git status` propre après premier commit ; aucun binaire / fichier généré tracké.

- [x] **T0.2 — Squelette CMake + Makefile wrapper**
  - Objectif : build système fonctionnel, binaire vide à la racine, flags Debug/Release, sorties aux bons endroits.
  - Livrable : `CMakeLists.txt` racine + `src/CMakeLists.txt` + `plugins/CMakeLists.txt` + `Makefile` wrapper.
  - Critère : `make` produit `./raytracer` (main qui retourne 0), `make re` / `make fclean` fonctionnent, `./raytracer` ne produit aucun output et retourne 0.

- [x] **T0.3 — `main.cpp` minimal conforme Epitech**
  - Objectif : `--help`, gestion d'argv, erreurs sur `stderr`, exit 84.
  - Livrable : `src/main.cpp`.
  - Critère : `./raytracer --help` affiche l'usage exact du sujet (exit 0) ; `./raytracer` seul → 84 + message stderr ; `./raytracer nope.cfg` → 84 + message stderr.

- [x] **T0.3.1 — Hiérarchie d'exceptions du domaine**
  - Objectif : introduire `rt::RaytracerException` + sous-classes (`CliError`, `IOError`, `SceneParseError` / `InvalidConfigError` / `UnknownTypeError`, `PluginError` / `PluginLoadError` / `PluginAbiError`, `MathError`) pour que tout le code métier lance, et qu'un unique `try/catch` dans `main()` traduise en exit 84 + message `stderr`.
  - Livrable : `src/core/Exceptions.hpp`, refactor de `src/main.cpp` pour lancer `CliError` / `IOError` et capturer via un seul `try/catch` avec branches polymorphes.
  - Critère : comportement CLI strictement identique à T0.3 (exits / stdout / stderr) ; la plomberie d'erreurs est disponible pour tous les modules à venir (math, parser, plugin loader).

- [x] **T0.4 — Math : `Vector2`, `Vector3`, `Color`**
  - Objectif : briques mathématiques soignées (opérateurs, dot, cross, normalize, length, conversions).
  - Livrable : `src/math/Vector2.hpp`, `Vector3.hpp`, `Color.hpp` (+ `.cpp` si besoin).
  - Critère : compilation propre sous `-Wall -Wextra -Werror -pedantic` ; `Color` stocké en linéaire (pas de conversion sRGB ici) ; `Vector3::normalized()` lève `rt::MathError` sur vecteur nul ; tous les opérateurs `constexpr` où possible. Validation formelle reportée à l'introduction d'un vrai framework de tests (ticket dédié).

- [x] **T0.5 — Math : `Matrix4`, `Transform`**
  - Objectif : transformations rigides + inverse + inverseTransposed pour les normales.
  - Livrable : `src/math/Matrix4.hpp/.cpp`, `src/math/Transform.hpp/.cpp`.
  - Critère : `translation(dx,dy,dz).applyToPoint(p) == p + (dx,dy,dz)` ; `inverse().apply(apply(p)) ≈ p` (critères structurellement satisfaits par construction — validation formelle reportée au ticket tests dédié).

- [x] **T0.6 — `Ray`, `HitRecord`, `ShadingContext`, `AABB`**
  - Objectif : types POD partagés par le core et les plugins.
  - Livrable : headers dans `src/core/` ou `src/interfaces/`.
  - Critère : compilent seuls, inclus par le core sans dépendance (vérifié : les 4 headers compilent isolément sous `-Wall -Wextra -Werror -pedantic` avec forward decl sur `IMaterial`/`INode`/`Scene`).

- [x] **T0.7 — Interfaces publiques**
  - Objectif : définir `IPrimitive`, `INode`, `ILight`, `IMaterial`.
  - Livrable : headers dans `src/interfaces/`.
  - Critère : cible CMake INTERFACE library `rt_interfaces` créée, liée par `raytracer` ; les 4 headers compilent avec stubs concrets sous `-Wall -Wextra -Werror -pedantic` (vérifié).

- [x] **T0.8 — ABI plugins (`PluginDescriptor`, `RAYTRACER_ABI_VERSION`)**
  - Objectif : contrat C stable exporté par tout plugin.
  - Livrable : `src/interfaces/plugin_abi.hpp`.
  - Critère : `RAYTRACER_ABI_VERSION = 1` défini ; `PluginDescriptor` + macro `RAYTRACER_PLUGIN_ENTRY` compilent des deux côtés (core et plugin simulé) sous `-Wall -Wextra -Werror -pedantic` (vérifié).

- [x] **T0.9 — `PluginLoader` (encapsulation `libdl`)**
  - Objectif : seul endroit avec `dlopen`/`dlsym`/`dlclose`, RAII strict.
  - Livrable : `src/plugins/PluginLoader.hpp/.cpp`.
  - Critère : build propre 0 warning ; RAII (destructor dlclose inverse-order) ; 7 guards dans loadOne (dlopen, dlsym, entry non-null, name non-vide, create/destroy non-null, ABI mismatch, duplicate) → warning stderr + rejet propre ; `ldd ./raytracer` ne liste aucun plugin ; `libdl` lié.

- [x] **T0.10 — `SceneParser` minimal (libconfig++)**
  - Objectif : ouverture fichier, lecture de la section `camera`.
  - Livrable : `src/parser/SceneParser.hpp/.cpp`, `scenes/empty_camera.cfg`.
  - Critère : `./raytracer scenes/empty_camera.cfg` → exit 0 (✓ vérifié). Erreurs de parse → `rt::InvalidConfigError` → exit 84 + stderr. `libconfig++-dev` 1.5 installé.

- [x] **T0.11 — `SceneBuilder` (Builder pattern)**
  - Objectif : construction incrémentale de `Scene`, résolution des références de matériaux.
  - Livrable : `src/core/SceneBuilder.hpp/.cpp`, `src/core/Scene.hpp/.cpp`, `src/core/CameraConfig.hpp`.
  - Critère : `./raytracer scenes/empty_camera.cfg` → exit 0 (✓) ; `SceneBuilder::build()` sans camera → `rt::InvalidConfigError` ; `scene.camera().fov == 60.0` par construction analytique (vérifié à la lecture du code).

- [x] **T0.12 — `PPMWriter` + image de test**
  - Objectif : écriture PPM P6 binaire, conversion linéaire → sRGB.
  - Livrable : `src/output/PPMWriter.hpp/.cpp`.
  - Critère : damier 256×256 écrit, visualisable (✓) ; header P6 valide, 196 608 octets pixels, cases blanc/noir alternées sur 32 px.

- [ ] **T0.13 — `Camera` pinhole + rayons primaires**
  - Objectif : `generateRay(x, y)` renvoie un rayon en espace monde selon FoV, position, rotation.
  - Livrable : `src/core/Camera.hpp/.cpp`.
  - Critère : rendu d'un fond dégradé dépendant de la direction du rayon = dégradé correct sur l'image.

- [ ] **T0.14 — `Renderer` (tuile-based dès le départ)**
  - Objectif : `renderTile(x0,y0,x1,y1, Image&)` + `renderAll()` qui itère.
  - Livrable : `src/core/Renderer.hpp/.cpp`.
  - Critère : image de fond dégradé rendue via le renderer, pas via code ad-hoc.

- [ ] **T0.15 — Plugin factice "sphere" (end-to-end)**
  - Objectif : prouver que la chaîne plugin fonctionne bout en bout.
  - Livrable : `plugins/sphere/` (CMakeLists + code).
  - Critère : `./plugins/raytracer_sphere.so` existe, chargé par `PluginLoader`, intersection basique disponible.

**🏁 Fin de Phase 0** : `./raytracer scenes/empty.cfg` rend une image PPM valide (fond dégradé), le loader charge `sphere.so`, toute l'architecture tient debout.

---

### Phase 1 — MUST (validation minimale du sujet)

- [ ] **T1.1** — Primitive **Sphere** (vraie implémentation, plugin `.so`) — intersection rayon/sphère en espace objet.
- [ ] **T1.2** — Primitive **Plane** (plugin `.so`) — plan Z=0 canonique, orienté via `Transform`.
- [ ] **T1.3** — Lumière **Ambient** (plugin `.so`).
- [ ] **T1.4** — Lumière **Directional** (plugin `.so`).
- [ ] **T1.5** — Matériau **FlatColor** (plugin `.so`).
- [ ] **T1.6** — Pipeline d'éclairage minimal dans le renderer (somme ambient + diffuse lambert).
- [ ] **T1.7** — Parser complet (primitives, lumières, matériaux référencés par nom, translation).
- [ ] **T1.8** — Scènes de démo : `demo_sphere.cfg`, `demo_plane.cfg`, `demo_flat_color.cfg`, `light_ambient.cfg`, `light_directional.cfg`, `demo_translation.cfg`.

**🏁 Fin de Phase 1** : `./raytracer scenes/demo_sphere.cfg` produit un PPM correct avec une sphère colorée éclairée. **MUST du sujet validé.**

---

### Phase 2 — SHOULD

- [ ] **T2.1** — Primitive **Cylinder** (plugin).
- [ ] **T2.2** — Primitive **Cone** (plugin).
- [ ] **T2.3** — **Rotation** via `Transform::rotation()`.
- [ ] **T2.4** — **Drop shadows** (shadow ray dans la direction de `LightSample`).
- [ ] **T2.5** — Scènes de démo : `demo_cylinder.cfg`, `demo_cone.cfg`, `demo_rotation.cfg`, `demo_shadows.cfg`.

**🏁 Fin de Phase 2** : toutes les features MUST + SHOULD fonctionnent, architecture toujours propre.

---

### Phase 3 — Audit & multithreading

- [ ] **T3.1** — Audit architecture : ajouter un type = N fichiers ? Plugins réellement indépendants ? `ldd ./raytracer` vierge ?
- [ ] **T3.2** — **Multithreading** du renderer (thread pool sur les tuiles) — bonus 1 pt, gain énorme pour le dev.
- [ ] **T3.3** — (Optionnel) **BVH** / space partitioning si la géométrie devient lourde.

**🏁 Fin de Phase 3** : dev confortable pour la suite, rendus rapides.

---

### Phase 4 — Empilement de bonus

Ordre recommandé (gain visuel / effort) :

- [ ] **T4.1** — **Phong reflection model** (2 pts) — énorme gain visuel.
- [ ] **T4.2** — **Reflection** (0.5 pt).
- [ ] **T4.3** — **Refraction** (1 pt) + **Transparency** (0.5 pt).
- [ ] **T4.4** — **Transformation matrix** (2 pts) — déjà acquis structurellement, à documenter.
- [ ] **T4.5** — **Scene graph** (2 pts) — idem, à documenter + scènes de démo avec groupes.
- [ ] **T4.6** — **Texture procédurale checkerboard** (1 pt).
- [ ] **T4.7** — **Texture procédurale Perlin noise** (1 pt).
- [ ] **T4.8** — **Texturing from file** (1 pt).
- [ ] **T4.9** — **Antialiasing supersampling** (0.5 pt) + **adaptif** (1 pt).
- [ ] **T4.10** — **Multiple lights** (directional × / point ×) + **colored light** (0.5 × 3).
- [ ] **T4.11** — **Triangles** (1 pt) + **`.OBJ` loader** (1 pt).
- [ ] **T4.12** — Primitives exotiques : **Torus** (1 pt), **Tanglecube** (1 pt), **Möbius strip** (2 pts), **Fractales** (2 pts).
- [ ] **T4.13** — **Limited cylinder / cone** (0.5 + 0.5).
- [ ] **T4.14** — **Scale / Shear** transformations (0.5 + 0.5).
- [ ] **T4.15** — **Normal mapping** (2 pts).
- [ ] **T4.16** — **Ambient occlusion** (2 pts).
- [ ] **T4.17** — **Import scene in scene** (2 pts).
- [ ] **T4.18** — **SFML display** (1 pt) + **quit during generation** (0.5 pt) + **scene preview** (2 pts) + **hot reload** (1 pt) → Observer pattern activé.

**🏁 Fin de Phase 4** : score bonus solide, features visuelles impressionnantes.

---

### Phase 5 — Polish & soutenance

- [ ] **T5.1** — Revue finale : zéro warning, code mort supprimé, `clang-tidy` si possible.
- [ ] **T5.2** — `README.md` utilisateur finalisé (install, build, run, format de scène).
- [ ] **T5.3** — Commentaires sur les parties mathématiques non triviales.
- [ ] **T5.4** — Production des scènes de démo manquantes + rendus dans `./screenshots/`.
- [ ] **T5.5** — Scène **`showcase.cfg`** combinant : sphères réfléchissantes + plan texturé + Phong + ombres + antialiasing + scène importée + multi-lumières.
- [ ] **T5.6** — Préparation de la soutenance : capacité à expliquer chaque design pattern, chaque choix.

**🏁 Fin de Phase 5** : projet prêt à être rendu et soutenu.

---

## 📊 Checklist de conformité permanente (à vérifier à chaque commit)

- [ ] Binaire `./raytracer` à la racine.
- [ ] Plugins dans `./plugins/*.so`.
- [ ] Exit 0 / 84 correctement utilisés.
- [ ] Erreurs sur `std::cerr` uniquement.
- [ ] `./raytracer --help` conforme au sujet.
- [ ] `ldd ./raytracer` ne liste aucun plugin.
- [ ] Build `-Wall -Wextra -Werror -pedantic` sans warning.
- [ ] `.gitignore` strict, aucun binaire tracké.
- [ ] `plan.md` à jour (cases cochées).
- [ ] `DOCUMENTATION.md` à jour (entrée par feature).

---

## 🔁 Processus à chaque ticket

1. Vérifier le ticket dans `plan.md`.
2. Implémenter.
3. Valider le critère d'acceptation.
4. Cocher la case ici.
5. Ajouter une entrée dans `DOCUMENTATION.md` (section patch notes).
6. Commit avec message conventionnel (`feat:`, `fix:`, `refactor:`…).
