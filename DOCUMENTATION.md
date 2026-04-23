# DOCUMENTATION.md — Historique du projet Raytracer

> Ce document est un **journal technique vivant**. À chaque feature implémentée, à chaque choix notable, à chaque patch, on ajoute une entrée.
>
> - `PROJECT.md` = hub descriptif (philosophie, exigences sujet).
> - `PLAN.md` = feuille de route (tickets, phases, critères d'acceptation).
> - `DOCUMENTATION.md` (ce fichier) = **ce qui a été fait, quand, pourquoi, comment**.
> - `MATHS.md` = Explications mathématiques : théorie et pratique du raytracing, ainsi que la géométrie spatiale.
> 
> **Règle** : on ne coche pas un ticket dans `PLAN.md` sans avoir ajouté son entrée ici.

---

## 📑 Sommaire

- [DOCUMENTATION.md — Historique du projet Raytracer](#documentationmd--historique-du-projet-raytracer)
  - [📑 Sommaire](#-sommaire)
  - [Format d'une entrée](#format-dune-entrée)
  - [Versioning](#versioning)
  - [Changelog](#changelog)
    - [v0.0.30 — 2026-04-23 — BVH (T3.3)](#v0030--2026-04-23--bvh-t33)
    - [v0.0.29 — 2026-04-23 — Multithreading renderer + AABB slab test (T3.1/T3.2)](#v0029--2026-04-23--multithreading-renderer--aabb-slab-test-t31t32)
    - [v0.0.28 — 2026-04-23 — Drop shadows (T2.4)](#v0028--2026-04-23--drop-shadows-t24)
    - [v0.0.27 — 2026-04-23 — Rotation via Transform (T2.3)](#v0027--2026-04-23--rotation-via-transform-t23)
    - [v0.0.26 — 2026-04-23 — Plugin Cone (T2.2)](#v0026--2026-04-23--plugin-cone-t22)
    - [v0.0.25 — 2026-04-23 — Plugin Cylinder (T2.1)](#v0025--2026-04-23--plugin-cylinder-t21)
    - [v0.0.24 — 2026-04-22 — Scènes de démo Phase 1 (T1.8)](#v0024--2026-04-22--scènes-de-démo-phase-1-t18)
    - [v0.0.23 — 2026-04-22 — Parser complet : matériaux + translation (T1.7)](#v0023--2026-04-22--parser-complet--matériaux--translation-t17)
    - [v0.0.22 — 2026-04-22 — Pipeline d'éclairage minimal (T1.6)](#v0022--2026-04-22--pipeline-déclairage-minimal-t16)
    - [v0.0.21 — 2026-04-22 — Plugin FlatColor + parseMaterials (T1.5)](#v0021--2026-04-22--plugin-flatcolor--parsematerials-t15)
    - [v0.0.20 — 2026-04-22 — Plugin DirectionalLight (T1.4)](#v0020--2026-04-22--plugin-directionallight-t14)
    - [v0.0.19 — 2026-04-22 — Plugin AmbientLight + parseLights (T1.3)](#v0019--2026-04-22--plugin-ambientlight--parselights-t13)
    - [v0.0.18 — 2026-04-22 — Plugin Plane, parsePrimitives générique (T1.2)](#v0018--2026-04-22--plugin-plane-parseprimitives-générique-t12)
    - [v0.0.17 — 2026-04-22 — Sphere réelle + PrimitiveNode + GroupNode (T1.1)](#v0017--2026-04-22--sphere-réelle--primitivenode--groupnode-t11)
    - [v0.0.16 — 2026-04-22 — Plugin factice sphere, fin Phase 0 (T0.15)](#v0016--2026-04-22--plugin-factice-sphere-fin-phase-0-t015)
    - [v0.0.15 — 2026-04-22 — Renderer tuile-based + Image (T0.14)](#v0015--2026-04-22--renderer-tuile-based--image-t014)
    - [v0.0.14 — 2026-04-22 — Camera pinhole + rayons primaires (T0.13)](#v0014--2026-04-22--camera-pinhole--rayons-primaires-t013)
    - [v0.0.13 — 2026-04-22 — PPMWriter : sortie P6 binaire, sRGB (T0.12)](#v0013--2026-04-22--ppmwriter--sortie-p6-binaire-srgb-t012)
    - [v0.0.12 — 2026-04-22 — SceneBuilder + Scene immuable (T0.11)](#v0012--2026-04-22--scenebuilder--scene-immuable-t011)
    - [v0.0.11 — 2026-04-22 — SceneParser minimal : libconfig++, section camera (T0.10)](#v0011--2026-04-22--sceneparser-minimal--libconfig-section-camera-t010)
    - [v0.0.10 — 2026-04-22 — PluginLoader : encapsulation dlopen/dlsym/dlclose (T0.9)](#v0010--2026-04-22--pluginloader--encapsulation-dlopendlsymdlclose-t09)
    - [v0.0.9 — 2026-04-22 — ABI plugins : PluginDescriptor, RAYTRACER_ABI_VERSION (T0.8)](#v009--2026-04-22--abi-plugins--plugindescriptor-raytracer_abi_version-t08)
    - [v0.0.8 — 2026-04-22 — Interfaces publiques : IPrimitive, INode, ILight, IMaterial (T0.7)](#v008--2026-04-22--interfaces-publiques--iprimitive-inode-ilight-imaterial-t07)
    - [v0.0.7 — 2026-04-22 — Types POD partagés : Ray, HitRecord, ShadingContext, AABB (T0.6)](#v007--2026-04-22--types-pod-partagés--ray-hitrecord-shadingcontext-aabb-t06)
    - [v0.0.6 — 2026-04-22 — Math : Matrix4 + Transform (T0.5)](#v006--2026-04-22--math--matrix4--transform-t05)
    - [v0.0.5 — 2026-04-22 — Math : Vector2, Vector3, Color (T0.4)](#v005--2026-04-22--math--vector2-vector3-color-t04)
    - [v0.0.4 — 2026-04-21 — Hiérarchie d'exceptions du domaine (T0.3.1)](#v004--2026-04-21--hiérarchie-dexceptions-du-domaine-t031)
    - [v0.0.3 — 2026-04-21 — CLI conforme Epitech (T0.3)](#v003--2026-04-21--cli-conforme-epitech-t03)
    - [v0.0.2 — 2026-04-21 — Squelette CMake + Makefile wrapper (T0.2)](#v002--2026-04-21--squelette-cmake--makefile-wrapper-t02)
    - [v0.0.1 — 2026-04-21 — Initialisation du dépôt (T0.1)](#v001--2026-04-21--initialisation-du-dépôt-t01)
  - [Architecture \& choix techniques](#architecture--choix-techniques)
    - [Design patterns retenus (v0.0.1)](#design-patterns-retenus-v001)
    - [Choix techniques clés (v0.0.1)](#choix-techniques-clés-v001)
    - [Gestion d'erreurs (v0.0.4)](#gestion-derreurs-v004)
    - [Math : conventions (v0.0.5)](#math--conventions-v005)
    - [Types POD d'échange core/plugins (v0.0.7)](#types-pod-déchange-coreplugins-v007)
  - [Features documentées](#features-documentées)
    - [Primitives](#primitives)
    - [Lumières](#lumières)
    - [Matériaux](#matériaux)
    - [Transformations](#transformations)
    - [Bonus](#bonus)
  - [Patches / bug fixes](#patches--bug-fixes)
    - [2026-04-22 — Suppression de `math_sanity_check()` et de `src/math/sanity.hpp`](#2026-04-22--suppression-de-math_sanity_check-et-de-srcmathsanityhpp)
    - [2026-04-21 — Suppression du bruit `gmake[N]: Entering/Leaving directory`](#2026-04-21--suppression-du-bruit-gmaken-enteringleaving-directory)
  - [Known issues](#known-issues)

---

## Format d'une entrée

Chaque entrée suit ce canevas :

```markdown
### vX.Y.Z — YYYY-MM-DD — <Titre court>

**Type** : feature | refactor | fix | docs | infra | perf | chore
**Ticket** : T0.X (référence vers plan.md)
**Phase** : 0 (Fondations) | 1 (MUST) | 2 (SHOULD) | 3 (Audit) | 4 (Bonus) | 5 (Polish)

**Résumé**
Une phrase qui dit ce qui change.

**Contexte / motivation**
Pourquoi on fait ça maintenant, quelle contrainte ou quelle feature ça débloque.

**Détails techniques**
- Fichiers ajoutés / modifiés / supprimés
- Interfaces ou signatures impactées
- Dépendances introduites
- Décisions notables (et alternatives rejetées)

**Impact**
- Ce qui devient possible
- Ce qui n'est PAS couvert (volontairement)
- Risques / dettes techniques introduites

**Validation**
- Commande(s) permettant de vérifier que ça marche
- Scène de démo associée (si applicable)
```

---

## Versioning

Schéma **MAJOR.MINOR.PATCH** interne au projet (pas de release publique) :

- **MAJOR** : passe d'une phase à la suivante (0 → 1 → 2 → … → 5).
- **MINOR** : nouvelle feature complète (une primitive, une lumière, un matériau, un bonus).
- **PATCH** : bug fix, refactor, amélioration de doc, ajout de scène de démo.

Exemples :
- `v0.1.0` = Phase 0 démarrée, première feature d'infrastructure livrée.
- `v1.0.0` = MUST complet.
- `v2.0.0` = MUST + SHOULD complets.
- `v4.5.0` = empilement de bonus, 5ᵉ bonus livré en Phase 4.

---

## Changelog

### v0.0.30 — 2026-04-23 — BVH (T3.3)

**Type** : performance
**Ticket** : T3.3
**Phase** : 3

**Ce qui a changé** :
- `src/core/BVHNode.hpp` (header-only) : `BVHNode::build(nodes)` construit un arbre BVH par split centroid-median sur l'axe le plus long (O(N log N)), `intersect()` utilise `AABB::intersects()` pour rejeter les sous-arbres entiers avant de descendre, et réduit `tMax` après le premier enfant touché pour élaguer le second.
- `src/interfaces/AABB.hpp` : ajout de `isFinite()` — retourne false si un des 6 composants est ±inf ou NaN. Utilisé pour séparer les primitives non-bornées.
- `src/core/SceneBuilder.cpp` : `build()` partitionne les primitives en `finite` (AABB bornée → BVH) et `infinite` (plans, sky domes → `GroupNode` linéaire). La racine de scène est soit le BVH seul, soit un `GroupNode(BVH, infiniteGroup)`. Évite le NaN centroid qui causait UB dans `std::sort`.
- `src/core/SceneBuilder.hpp` : inclusion de `GroupNode.hpp` ajoutée.

**Bug résolu** : le plan plugin retournait `AABB{-∞,-∞,-∞, +∞,+∞,+∞}`. Le centroid `(-∞ + +∞) × 0.5 = NaN` rendait le comparateur de sort UB, produisant un arbre BVH incorrect (pixels différents). La partition finite/infinite corrige cela.

**Critère validé** : `cmp /tmp/mt1.ppm /tmp/bvh_fixed.ppm` → PIXEL IDENTICAL, 24/24 scènes exit 0.

---

### v0.0.29 — 2026-04-23 — Multithreading renderer + AABB slab test (T3.1/T3.2)

**Type** : performance
**Tickets** : T3.1, T3.2
**Phase** : 3

**Ce qui a changé** :
- `src/interfaces/AABB.hpp` : `bool intersects(const Ray&) const noexcept` — slab test Kay-Kajiya branchless par axe. Prérequis pour BVH (T3.3).
- `src/core/Renderer.cpp` : `renderAll()` parallélisé via thread pool minimaliste. Toutes les tuiles sont indexées dans un `std::vector<Tile>`, un `std::atomic<int>` distribue les indices sans mutex. N threads = `std::thread::hardware_concurrency()`. Aucun état partagé mutable entre threads : `Scene` const, `Camera` const, pixels de tuiles disjoints.
- Gain mesuré sur `demo_grand.cfg` (1280×720, 12 cœurs) : **1.03s → 0.26s (×4)**, 692% CPU, sortie binaire identique d'un run à l'autre.

**Critère validé** : 24/24 scènes exit 0, output déterministe (`cmp` entre deux runs identiques), 0 warnings.

---

### v0.0.28 — 2026-04-23 — Drop shadows (T2.4)

**Type** : feature
**Ticket** : T2.4 + T2.5
**Phase** : 2 (SHOULD)

**Ce qui a changé** :
- `src/core/Scene.hpp/.cpp` : `bool Scene::occluded(const Ray& shadowRay) const` — délègue à `root_->intersect()` avec dummy HitRecord, court-circuite dès le premier hit.
- `src/core/Renderer.cpp` : dans la boucle de lumières, avant d'accumuler la contribution Lambert, un shadow ray est émis. Origin = `rec.point + rec.normal × kRayEpsilon` (biais normal pour éviter l'auto-intersection). `tMax = ls.distance − kRayEpsilon` (bloque avant la source pour les point lights ; ∞ pour les directionnels). Si `occluded()` retourne true : `continue` (contribution ignorée).
- `scenes/test_shadows.cfg` (320×240) : sphère rouge sur sol gris, lumière directionnelle depuis haut-gauche. Critère : 1 888 pixels ombragés (sRGB≈97) + 72 486 pixels éclairés (sRGB≈233).
- `scenes/demo_shadows.cfg` (800×600) : 6 sphères + 1 cylindre + 1 cône, lumière très contrastée (ambient=0.06), 9 752 pixels ombragés visibles.

**Critère validé** : `test_shadows.cfg` → ombre nette (1 888 px sRGB≈97) vs sol éclairé (72 486 px sRGB≈233), exit 0, 0 warnings.

---

### v0.0.27 — 2026-04-23 — Rotation via Transform (T2.3)

**Type** : feature
**Ticket** : T2.3
**Phase** : 2 (SHOULD)

**Ce qui a changé** :
- `src/core/PrimitiveNode.hpp` : `Transform transform_` + `Transform inverseTransform_` (cached) remplacent l'ancien `Vector3 translate_`. Le rayon est transformé en espace objet via l'inverse, le point/normale de hit sont transformés en espace monde via la forward transform. Règle normale : `(M⁻¹)ᵀ·n` via `transform_.applyToNormal()`.
- `src/core/SceneBuilder.hpp/.cpp` : `addPrimitive(prim, mat, Transform)` — `Transform::identity()` par défaut (rétrocompatible).
- `src/parser/SceneParser.cpp` : lecture du champ optionnel `rotate = { x; y; z; }` (degrés), composition X→Y→Z (intrinsèque) ; `translation * rotation` passé à `addPrimitive`.
- `scenes/test_rotation.cfg` (320×240) : cylindre cobalt horizontal (rotZ=90°) + cône amber incliné (rotX=30°).
- `scenes/demo_rotation.cfg` (800×600) : showcase rotation riche.

**Critère validé** : cylindre cobalt horizontal (2 194 px bleus, x:112..207) + cône amber incliné (542 px ambrés, x:93..112) dans `test_rotation.cfg`, exit 0, 0 warnings.

---

### v0.0.26 — 2026-04-23 — Plugin Cone (T2.2)

**Type** : feature
**Ticket** : T2.2
**Phase** : 2 (SHOULD)

**Ce qui a changé** :
- `plugins/cone/ConePlugin.cpp` : `Cone(center, radius, height)` — cône fini, apex en haut (y=+halfH), base en bas (y=-halfH).
  - Surface implicite : `f = x²+z² − k²·(halfH−y)²` où `k = radius/height`.
  - Quadratique : `a = Dx²+Dz²−k²Dy²`, `half_b = Ox·Dx+Oz·Dz+k²·α·Dy`, `c = Ox²+Oz²−k²·α²` (α = halfH−Oy).
  - Deux racines triées (min,max) — première valide dans `[-halfH, halfH]` retenue (gère `a<0` correctement).
  - Normale corps : `normalize(hx, k²·(halfH−hy), hz)` (gradient de f).
  - Cap base unique en y=−halfH : disque radius, normale (0,−1,0).
  - UV body : azimuth + hauteur normalisée. UV cap : projection XZ.
- `plugins/cone/CMakeLists.txt` + `plugins/CMakeLists.txt`.
- `src/parser/SceneParser.cpp` : `loadList("cones", "cone")`.
- `scenes/test_cone.cfg` (320×240) + `scenes/demo_cone.cfg` (800×600, cônes + cylindres + sphères).

**Critère validé** : corps orange Lambert (G réduit flanc vs cap), base disk visible, exit 0, 0 warnings.

---

### v0.0.25 — 2026-04-23 — Plugin Cylinder (T2.1)

**Type** : feature
**Ticket** : T2.1
**Phase** : 2 (SHOULD)

**Ce qui a changé** :
- `plugins/cylinder/CylinderPlugin.cpp` : `Cylinder(center, radius, height)` — cylindre fini axé sur Y, ±halfH du centre.
  - Body : quadratique en xz (`a = Dx²+Dz²`, `half_b = Ox*Dx+Oz*Dz`, `c = Ox²+Oz²−R²`), clamp `[-halfH, +halfH]`, racine proche en premier.
  - Caps : `t = (±halfH − Oy)/Dy`, test `hx²+hz² ≤ R²`.
  - UV body : `u = atan2(hz,hx)/(2π)+0.5`, `v = (hy+halfH)/(2*halfH)`. UV caps : projection XZ.
  - Normal body : `(hx/R, 0, hz/R)`. Normal caps : `(0, ±1, 0)`.
  - AABB : `center ± (R, halfH, R)`.
- `plugins/cylinder/CMakeLists.txt` : `raytracer_cylinder.so`.
- `plugins/CMakeLists.txt` : `add_subdirectory(cylinder)`.
- `src/parser/SceneParser.cpp` : `loadList("cylinders", "cylinder")`.
- `scenes/test_cylinder.cfg` (320×240) + `scenes/demo_cylinder.cfg` (800×600).

**Critère validé** : cap teal G=171 (normal up, Lambert fort), body G=147 (normal sideways, Lambert réduit). 0 warnings.

---

### v0.0.24 — 2026-04-22 — Scènes de démo Phase 1 (T1.8)

**Type** : docs / chore
**Ticket** : T1.8
**Phase** : 1 (MUST)

**Ce qui a changé** :
- Résolution augmentée : toutes les démo passent à 800×600 minimum (les test_* restent en 320×240).
- `scenes/demo_sphere.cfg` (800×600) : sphère rouge dramatique sur sol nacré, lumière chaude en haut-gauche.
- `scenes/demo_plane.cfg` (800×600) : plan gris à faible angle de caméra, sphère cobalt en ancrage.
- `scenes/demo_flat_color.cfg` (800×600) : 6 sphères en arc — rouge, orange, jaune, vert, bleu, violet.
- `scenes/light_ambient.cfg` (800×600) : ambient seul (intensity=0.85), surfaces plates sans gradient.
- `scenes/light_directional.cfg` (800×600) : ambient minimal (0.04) + directional fort, gradient Lambert maximal.
- `scenes/demo_translation.cfg` (800×600) : 6 sphères via `translate` uniquement, grille 3×2.
- `scenes/demo_showcase.cfg` (1280×720) : 12 sphères + 2 plans, 10 matériaux, 2 lumières directionnelles (key + fill), mix direct/translate.
- `screenshots/` : tous les rendus sauvegardés.

**Critère validé** : 7 scènes → exit 0, rendus sauvegardés dans `screenshots/`. Showcase 2.6 MB = 1280×720×3 octets (✓).

---

### v0.0.23 — 2026-04-22 — Parser complet : matériaux + translation (T1.7)

**Type** : feature
**Ticket** : T1.7
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `src/core/PrimitiveNode.hpp` : ajout de `material_` (const IMaterial*, emprunt non-owning) et `translate_` (Vector3). `intersect()` décale `ray.origin` de -translate_ avant, remplace `rec.point` de +translate_ après, et écrit `rec.material = material_` si non-null.
- `src/core/SceneBuilder.hpp/.cpp` : `addPrimitive()` reçoit `(prim, mat=nullptr, translate={})`. Ajout de `findMaterial(string_view)` → retourne le raw pointer depuis `materials_` (parseMaterials doit être appelé avant parsePrimitives).
- `src/parser/SceneParser.cpp` : lambda `loadList` lit le champ optionnel `material` (name → `builder.findMaterial()`), lit le champ optionnel `translate {x,y,z}`, puis appelle `builder.addPrimitive(prim, mat, translate)`.
- `scenes/test_translation.cfg` : sphère rouge et sphère verte, centre canonique `{0,0,0}`, positionnées via `translate`, sol bleu.

**Critère validé** : sphère rouge R=255 G≈130 ; plan bleu R≈124 G≈171 B=248 dans `test_flat_color.cfg`. Dans `test_translation.cfg` : sphère rouge à gauche (x≈100-130), sphère verte à droite (x≈200-230). 0 warnings.

---

### v0.0.22 — 2026-04-22 — Pipeline d'éclairage minimal (T1.6)

**Type** : feature
**Ticket** : T1.6
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `src/core/Renderer.cpp` : `shade()` remplace la visu de normales par le pipeline physique :
  1. Miss → sky gradient (inchangé)
  2. Hit → albedo via `rec.material->shade(ShadingContext)` ou `Color::white()` si pas de matériau (T1.7 l'assignera)
  3. Accumulation ambient : somme des `light->ambientContribution()` sur les lumières `isAmbient()`
  4. Accumulation diffuse Lambert : pour chaque lumière non-ambiante, `ls = light->sample(rec.point)`, contribution = `ls.radiance × max(0, normal·ls.direction)`
  5. Résultat = `albedo × (ambient + diffuse)`

**Critère validé** : sol (255,255,248) = Lambert fort + ambient ; ciel bleu (223,237,255) ; 0 warnings.

---

### v0.0.21 — 2026-04-22 — Plugin FlatColor + parseMaterials (T1.5)

**Type** : feature
**Ticket** : T1.5
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `plugins/flat_color/FlatColorPlugin.cpp` : `FlatColor(Color albedo)`, `shade()` retourne l'albedo directement (sans éclairage — appliqué en T1.6 par le Renderer). Config : `color {r, g, b}`.
- `plugins/flat_color/CMakeLists.txt` : `raytracer_flat_color.so`, lié à `rt_interfaces config++`.
- `plugins/CMakeLists.txt` : `add_subdirectory(flat_color)`.
- `src/parser/SceneParser.hpp/.cpp` : `parseMaterials(builder, loader)` — itère le groupe `materials`, lit le champ `type` pour dispatcher vers le plugin `(Material, type)`, appelle `builder.registerMaterial(name, ...)`.
- `src/main.cpp` : `parser.parseMaterials(builder, loader)` appelé avant `parsePrimitives`.
- `scenes/test_flat_color.cfg` : deux matériaux (red, blue), une sphère et un plan référençant chacun un matériau, ambient + directional.

**Critère validé** : `raytracer_flat_color.so` chargé, `parseMaterials` enregistre les matériaux sans erreur, `test_flat_color.cfg` → exit 0, 0 warnings.

---

### v0.0.20 — 2026-04-22 — Plugin DirectionalLight (T1.4)

**Type** : feature
**Ticket** : T1.4
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `plugins/directional/DirectionalPlugin.cpp` : `DirectionalLight(toLight, radiance)`. Config `direction` = direction vers la scène → negated+normalized pour `toLight_`. `sample()` retourne `LightSample{toLight_, radiance_, +inf}`. Pas de `isAmbient` override (false par défaut).
- `plugins/directional/CMakeLists.txt` + `plugins/CMakeLists.txt`.
- `scenes/test_directional.cfg` : sphère + plan + ambient + directional (effet en T1.6).

**Critère validé** : plugin chargé, exit 0, 0 warnings.

---

### v0.0.19 — 2026-04-22 — Plugin AmbientLight + parseLights (T1.3)

**Type** : feature
**Ticket** : T1.3
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `plugins/ambient/AmbientPlugin.cpp` : `AmbientLight(Color contribution)`, `isAmbient()=true`, `ambientContribution()=contribution_`, `sample()` dummy. Config : `color {r,g,b}` × `intensity`.
- `plugins/ambient/CMakeLists.txt` : `raytracer_ambient.so`.
- `plugins/CMakeLists.txt` : `add_subdirectory(ambient)`.
- `src/parser/SceneParser.hpp/.cpp` : `parseLights(builder, loader)` — lambda `loadList` symétrique à `parsePrimitives`. Supporte `ambient` (T1.3) et `directional` (T1.4 à venir).
- `src/main.cpp` : appel `parser.parseLights(builder, loader)`.
- `scenes/test_ambient.cfg` : sphère + plan + lumière ambiante (effet visible en T1.6).

**Critère validé** : plugin chargé, exit 0, 0 warnings sur `test_ambient.cfg` et toutes les scènes précédentes.

---

### v0.0.18 — 2026-04-22 — Plugin Plane, parsePrimitives générique (T1.2)

**Type** : feature
**Ticket** : T1.2
**Phase** : 1 (MUST)

**Ce qui a changé** :
- `plugins/plane/PlanePlugin.cpp` : `Plane(normal, distance)` — intersection `t=(d−dot(N,O))/dot(N,D)`, deux faces, tangente/bitangente précalculées pour UV, AABB infinie. Config : `normal {x,y,z}`, `distance`.
- `plugins/plane/CMakeLists.txt` : `raytracer_plane.so`.
- `plugins/CMakeLists.txt` : `add_subdirectory(plane)`.
- `src/parser/SceneParser.cpp` : `parsePrimitives` refactorisé avec lambda `loadList(key, kind)` — extensible à tout nouveau type de primitive en une ligne.
- `scenes/test_plane.cfg` : plan Y=0, caméra à l'horizon (ciel haut, sol bas).
- `scenes/test_sphere_on_plane.cfg` : sphère T1.1 posée sur plan T1.2.

**Critère validé** : sol (188,255,188) normal (0,1,0), ciel (216,233,255), sphère (188,254,202) distinct des deux. Exit 0, 0 warnings.

---

### v0.0.17 — 2026-04-22 — Sphere réelle + PrimitiveNode + GroupNode (T1.1)

**Type** : feature
**Ticket** : T1.1
**Phase** : 1 (MUST)

**Contexte** : Le plugin sphere était un stub. Il faut l'intersection réelle rayon/sphère pour pouvoir rendre de la géométrie.

**Ce qui a changé** :
- `plugins/sphere/SpherePlugin.cpp` : `Sphere(center, radius)`, intersection demi-discriminant (`half_b²−a·c`), normal outward/inward via `frontFace`, UV sphérique (θ=acos(−y), φ=atan2(−z,x)+π). Config lue via libconfig (`center.x/y/z`, `radius`).
- `plugins/sphere/CMakeLists.txt` : `config++` ajouté.
- `src/core/PrimitiveNode.hpp` : adapter INode→IPrimitive (header-only).
- `src/core/GroupNode.hpp` : groupe composite INode, tMax shrinking pour le closest-hit (header-only).
- `src/core/SceneBuilder.hpp/.cpp` : `addPrimitive(shared_ptr<IPrimitive>)` → crée PrimitiveNode, ajoute au GroupNode interne. `build()` utilise `root_` sinon `group_`.
- `src/parser/SceneParser.hpp/.cpp` : `parsePrimitives(builder, loader)` lit `primitives.spheres[]`, instancie via plugin factory + deleter custom.
- `src/main.cpp` : appel `parser.parsePrimitives(builder, loader)`.
- `src/core/Renderer.cpp` : `shade()` visualise la normale en RGB ([-1,1]→[0,1]) jusqu'à T1.5.
- `scenes/test_sphere.cfg` : scène de validation (sphère unité à l'origine).

**Critère validé** : pixel centre = (188,187,255) ≈ normal (0,0,1) → caméra ; coin = (210,230,255) ciel. Exit 0, 0 warnings.

---

### v0.0.16 — 2026-04-22 — Plugin factice sphere, fin Phase 0 (T0.15)

**Type** : feature
**Ticket** : T0.15
**Phase** : 0 (Fondations) — dernier ticket

**Contexte** : Valider la chaîne dlopen → ABI → registre bout en bout avant d'implémenter la vraie géométrie en Phase 1.

**Ce qui a changé** :
- `plugins/sphere/SpherePlugin.cpp` : `DummySphere` (IPrimitive, `intersect` retourne toujours false, `bounds` = AABB [-1,1]³). Fonctions `create`/`destroy` sans throw, macro `RAYTRACER_PLUGIN_ENTRY`. Pas de dépendance à libconfig++ (paramètre ignoré avec forward-decl).
- `plugins/sphere/CMakeLists.txt` : `add_library(raytracer_sphere SHARED ...)`, `PREFIX ""` → `raytracer_sphere.so`.
- `plugins/CMakeLists.txt` : `add_subdirectory(sphere)` ajouté.

**Critère validé** : `./plugins/raytracer_sphere.so` (16.7K), chargé par `PluginLoader` sans warning stderr, `./raytracer scenes/empty_camera.cfg` → exit 0.

**🏁 Phase 0 complète** : toute l'architecture (math, interfaces, ABI, PluginLoader, SceneParser, SceneBuilder, Camera, Renderer, PPMWriter) tient debout. Le binaire rend un dégradé ciel 800×600 et charge le plugin sphere.

---

### v0.0.15 — 2026-04-22 — Renderer tuile-based + Image (T0.14)

**Type** : feature
**Ticket** : T0.14
**Phase** : 0 (Fondations)

**Contexte** : Le code ad-hoc T0.13 calculait les pixels dans `runScene()`. Il faut l'encapsuler dans un `Renderer` réutilisable, partitionné en tuiles pour préparer la parallélisation (T3.2).

**Ce qui a changé** :
- `src/core/Image.hpp` : buffer pixel row-major `vector<Color>`, accesseurs `at(x,y)`, `pixels()`, `width()`, `height()` (header-only).
- `src/core/Renderer.hpp/.cpp` : `Renderer(Scene&, Camera&)`, `renderTile(x0,y0,x1,y1, Image&)`, `renderAll(Image&)` (tuiles 64×64). `shade(ray)` : appel `scene_.intersect()` → gris #808080 si hit (placeholder T0.15), sinon dégradé ciel.
- `main.cpp` : `runScene()` simplifié à 4 lignes : `Camera` → `Image` → `Renderer::renderAll` → `PPMWriter::write`. Suppression du `[[maybe_unused]]`.
- `src/CMakeLists.txt` : `core/Renderer.cpp` ajouté.

**Critère validé** : image 800×600 identique au dégradé T0.13, aucune couture aux frontières de tuile (pixels (63,0) et (64,0) identiques).

---

### v0.0.14 — 2026-04-22 — Camera pinhole + rayons primaires (T0.13)

**Type** : feature
**Ticket** : T0.13
**Phase** : 0 (Fondations)

**Contexte** : La scène parsée fournit un `CameraConfig` (position, lookAt, up, FoV, résolution). Il faut en dériver des rayons monde pour chaque pixel de l'image.

**Ce qui a changé** :
- `src/core/Camera.hpp/.cpp` : modèle sténopé. Le constructeur précalcule la base orthonormée `(u, v, w)` et les vecteurs d'écran `lowerLeft_`, `horizontal_`, `vertical_`; `generateRay(px, py)` est un hot path arithmétique pur (sans throw).
  - Base : `w = normalize(position − lookAt)`, `u = normalize(up × w)`, `v = w × u`.
  - Demi-extents : `halfH = tan(fov * π / 360)`, `halfW = halfH × (width / height)`.
  - Convention pixels : `(0,0)` = coin haut-gauche, `y` croît vers le bas.
  - Normalisation safe dans `generateRay` : fallback `-z` si direction nulle (ne se produit pas avec une config valide).
- `main.cpp` : `runScene()` remplace le damier T0.12 par un rendu dégradé ciel (bleu-azur au zénith, blanc à l'horizon) via les rayons primaires.
- `src/CMakeLists.txt` : `core/Camera.cpp` ajouté.

**Critère validé** : image 800×600, canal R monotone croissant top→bot (207→225→240), zénith (207, 228, 255), horizon (240, 246, 255).

---

### v0.0.13 — 2026-04-22 — PPMWriter : sortie P6 binaire, sRGB (T0.12)

**Type** : feature
**Ticket** : T0.12
**Phase** : 0 (Fondations)

**Contexte** : Le pipeline de rendu produit des couleurs en espace linéaire (`Color`, floats). Il faut un module pour les encoder en PPM P6 (format portable, lisible par tout viewer) avec conversion gamma sRGB.

**Ce qui a changé** :
- `src/output/PPMWriter.hpp/.cpp` : `PPMWriter::write(ostream, w, h, pixels)` et `writeFile(path, ...)`.
- Conversion IEC 61966-2-1 : linéaire ≤ 0.0031308 → ×12.92, sinon 1.055 × x^(1/2.4) − 0.055. Clampage implicite (négatif → 0, > 1 → 255).
- Header P6 binaire (`P6\n<w> <h>\n255\n`), données pixels 3 octets/pixel sans padding.
- `main.cpp` : `runScene()` génère un damier 256×256 (cases de 32 px) sur stdout — remplacé par le vrai renderer en T0.14.
- `src/CMakeLists.txt` : `output/PPMWriter.cpp` ajouté.

**Critère validé** : `./raytracer scenes/empty_camera.cfg > /tmp/check.ppm` → 196 623 octets (15 header + 196 608 pixels), header P6 valide, cases (0,0) = blanc, (32,0) = noir.

---

### v0.0.12 — 2026-04-22 — SceneBuilder + Scene immuable (T0.11)

**Type** : feature
**Ticket** : T0.11
**Phase** : 0 (Fondations)

**Résumé**
Introduction du Builder pattern pour la construction de scène : `rt::SceneBuilder` accumule caméra, nœuds, lumières et matériaux nommés, puis construit une `rt::Scene` **immuable** via `build()`. `CameraConfig` est extrait de `SceneParser.hpp` dans `core/CameraConfig.hpp` pour éviter une dépendance core → parser. `runScene()` est désormais complet : `PluginLoader → SceneParser → SceneBuilder → Scene`.

**Contexte / motivation**
La `Scene` est l'état global du rendu : chaque thread de rendu (T3.2) la lit en lecture seule. L'immuabilité post-construction est la propriété qui permet un multithreading sans verrous. Le Builder sépare la phase d'accumulation (mutable, séquentielle) de la phase de lecture (immuable, parallèle).

La résolution des matériaux par nom est aussi centralisée ici : le `SceneParser` (T1.7) lit `material = "red"` dans le `.cfg`, le `SceneBuilder` a enregistré `"red"` → un `shared_ptr<IMaterial>`, et la `Scene` expose `findMaterial(name) → const IMaterial*` utilisée par les plugins lors du shade.

**Détails techniques**

- **`src/core/CameraConfig.hpp`** — POD extrait de `SceneParser.hpp`.
  - Raison : `Scene.hpp` a besoin de `CameraConfig` ; si ce type vivait dans `SceneParser.hpp`, `Scene.hpp → SceneParser.hpp → libconfig.h++` créerait une dépendance core → parser → lib tierce. Extraire `CameraConfig` dans core coupe cette chaîne.
  - `SceneParser.hpp` inclut désormais `core/CameraConfig.hpp` (parser → core, sens correct).
- **`src/core/Scene.hpp`** — snapshot immuable.
  - Non-copiable (`= delete`), movable (`= default`) pour le retour par valeur depuis `build()` (NRVO).
  - Destructeur déclaré dans `.hpp`, défini dans `.cpp` (où `IMaterial.hpp` est inclus) pour éviter la dépendance circulaire `Scene.hpp → IMaterial.hpp → ShadingContext.hpp → Scene.hpp`.
  - `class IMaterial;` forward-declared dans `Scene.hpp` ; `shared_ptr<IMaterial>` est valide avec un type incomplet pour le stockage (le destructeur nécessite le type complet → dans `.cpp`).
  - Accesseurs : `camera()`, `lights()`, `findMaterial(string_view)`, `intersect(Ray, HitRecord&)`.
  - `intersect()` : délègue à `root_->intersect()` si root non nul, sinon false (scène vide → tous les rayons manquent → couleur de fond).
  - Constructeur privé + `friend class SceneBuilder` : seul le builder peut construire une Scene.
- **`src/core/Scene.cpp`** — implémentations.
  - `~Scene() = default` défini ici (type `IMaterial` complet requis pour `~shared_ptr<IMaterial>`).
  - `findMaterial` : `O(log n)` via `std::map::find`.
- **`src/core/SceneBuilder.hpp/.cpp`** — Builder pattern.
  - API fluente : chaque setter retourne `*this`.
  - `std::optional<CameraConfig> camera_` : absent → `build()` throw `rt::InvalidConfigError`.
  - `setRoot(shared_ptr<INode>)`, `addLight(shared_ptr<ILight>)`, `registerMaterial(name, shared_ptr<IMaterial>)` — toutes les données sont moved dans la Scene lors de `build()`.
  - Registre matériaux : `map<string, shared_ptr<IMaterial>>` — "last registration wins" (cohérent avec libconfig++ où la dernière définition écrase la précédente).
- **`src/main.cpp` — `runScene()` finalisé pour Phase 0** :
  ```cpp
  rt::PluginLoader  loader("./plugins");
  rt::SceneParser   parser(scenePath);
  rt::SceneBuilder  builder;
  builder.setCamera(parser.parseCamera());
  rt::Scene scene = builder.build();
  // T0.12+: renderer
  ```
- **`src/CMakeLists.txt`** : ajout de `core/Scene.cpp` et `core/SceneBuilder.cpp`.

**Alternatives rejetées**
- **`Scene` copiable** : la scène peut contenir des centaines de milliers de nœuds. Une copie serait onéreuse et sémantiquement erronée (deux threads ne devraient pas chacun avoir leur propre copie — ils lisent la même). Move-only + référence partagée via `const Scene&` dans les `ShadingContext` est la politique correcte.
- **Constructeur public de Scene** : permettrait de bypasser le builder et de construire une scène sans caméra → undefined behaviour dans le renderer. Le constructeur privé + `friend SceneBuilder` verrouille le point d'entrée.
- **`Scene` avec méthodes mutables (`addNode`, etc.)** : briserait l'immuabilité, empêcherait le multithreading lock-free.
- **`map<string, IMaterial*>` au lieu de `shared_ptr`** : gestion manuelle de la durée de vie → impossible à sécuriser correctement. `shared_ptr` garantit que le matériau survit aussi longtemps que la scène.
- **`unordered_map` pour les matériaux** : O(1) vs O(log n) sur quelques dizaines de matériaux → différence négligeable, `std::map` évite de définir un hash.
- **Inclure `IMaterial.hpp` dans `Scene.hpp`** : créerait la dépendance circulaire `Scene → IMaterial → ShadingContext → Scene`. La forward-declaration + destructor-in-cpp est le pattern standard C++ pour ce cas.
- **`GroupNode` intégré dans T0.11** : ajouterait une implémentation concrète de `INode` à ce ticket. T0.11 reste délibérément abstrait — les implémentations concrètes arrivent en T0.15 et Phase 1. La `Scene` accepte un root nul ("scène vide").

**Impact**
- **`ShadingContext::scene`** est désormais utilisable en entier : `ctx.scene.findMaterial(...)`, `ctx.scene.intersect(...)`, `ctx.scene.lights()` sont accessibles depuis `IMaterial::shade()`.
- **T0.12** (PPMWriter) : la scène est construite, la résolution image est connue (`scene.camera().width`, `.height`).
- **T0.13** (Camera) : `Camera(scene.camera())` construit la caméra depuis le config.
- **T1.1+** (primitives) : `SceneBuilder::setRoot(node)` est prêt ; `SceneBuilder::registerMaterial(name, mat)` est prêt.
- **T3.2** (multithreading) : la scène étant immuable, les threads peuvent lire `scene.intersect()` en parallèle sans synchronisation.
- **Régression** : `SceneParser.hpp` a perdu la définition locale de `CameraConfig` au profit d'un `#include "core/CameraConfig.hpp"`. Vérifier que les fichiers qui incluaient `SceneParser.hpp` et utilisaient `rt::CameraConfig` compilent toujours — oui (`SceneBuilder.cpp`, `main.cpp`).

**Non couvert volontairement**
- `Scene::root()` comme accesseur public — le renderer accède à la scène via `intersect()` ; exposer le root permettrait des downcasts dangereux.
- Validation de `CameraConfig` (fov > 0, width > 0…) → T0.13 (Camera vérifiera lors de sa construction).
- `GroupNode` (nœud Composite concret) → T0.15 ou T1.1 quand les premières primitives arriveront.

**Validation**
```bash
make re
./raytracer scenes/empty_camera.cfg; echo $?     # 0 ✓
./raytracer; echo $?                              # 84 (CliError)
./raytracer nope.cfg; echo $?                     # 84 (IOError)
ldd ./raytracer | grep raytracer_                 # vide
```

---

### v0.0.11 — 2026-04-22 — SceneParser minimal : libconfig++, section camera (T0.10)

**Type** : feature
**Ticket** : T0.10
**Phase** : 0 (Fondations)

**Résumé**
`rt::SceneParser` — classe qui ouvre un fichier `.cfg` avec libconfig++, parse la section obligatoire `camera` et en retourne un `CameraConfig`. `runScene()` dans `main.cpp` instancie désormais un `PluginLoader` puis un `SceneParser`  — la chaîne end-to-end parse est opérationnelle. Première scène de démo `scenes/empty_camera.cfg` créée.

**Contexte / motivation**
T0.9 a posé le loader de plugins, T0.10 pose la lecture du fichier de scène. La brique manquante pour que `./raytracer scenes/empty_camera.cfg` retourne 0 sans crash est le parser libconfig++. La section `camera` est le minimum vital : sans elle, ni la `Camera` (T0.13) ni le `Renderer` (T0.14) ne peuvent être instanciés. Les sections primitives/lights/materials arriveront en T0.11 (SceneBuilder) et T1.7 (parser complet).

**Détails techniques**

- **`src/parser/SceneParser.hpp`** — interface publique.
  - `CameraConfig { position, lookAt, up (Vector3) ; fov (double, degrés) ; width, height (int) }`.
  - `SceneParser(fs::path)` : ouvre + parse. Throws `rt::IOError` (fichier illisible) ou `rt::InvalidConfigError` (erreur de syntaxe avec fichier + numéro de ligne).
  - `parseCamera() → CameraConfig` : lit le groupe `camera` de façon typée. Throws `rt::InvalidConfigError` sur champ absent ou mauvais type.
  - Pas de `<dlfcn.h>` ni de dépendance interne hors `<libconfig.h++>` et `math/Vector3.hpp`.
- **`src/parser/SceneParser.cpp`** — implémentation.
  - Constructeur : `config_.readFile(path.c_str())` ; les exceptions libconfig (`FileIOException`, `ParseException`) sont attrapées et converties en `rt::IOError` / `rt::InvalidConfigError`.
  - `readVec3(Setting)` : helper privé statique qui lit `{x, y, z}` ; gère les cas `TypeInt` (ex: `0` sans point décimal dans le `.cfg`) via double cast explicite.
  - `parseCamera()` : `config_.lookup("camera")` → parse chaque champ avec `static_cast` explicite ; attrape `SettingNotFoundException` et `SettingTypeException`.
- **`scenes/empty_camera.cfg`** — scène minimale.
  ```
  camera: { position, lookAt, up en {x,y,z} ; fov=60 ; width=800 ; height=600 }
  ```
  Sert de test de non-régression permanent : `./raytracer scenes/empty_camera.cfg` doit toujours retourner 0.
- **`src/main.cpp` — `runScene()` mis à jour** :
  ```cpp
  rt::PluginLoader loader("./plugins");   // T0.9
  rt::SceneParser  parser(scenePath);     // T0.10
  [[maybe_unused]] auto cam = parser.parseCamera();
  // T0.11+: SceneBuilder → Scene → Renderer
  ```
  Silencieux en cas de succès (stdout vide) — conforme aux exigences Epitech.
- **`src/CMakeLists.txt`** : ajout de `parser/SceneParser.cpp` + `target_link_libraries(... config++)`.
- **Dépendance système** : `libconfig++-dev` version 1.5 (Ubuntu/Debian). À documenter dans `README.md` (T5.2).

**Alternatives rejetées**
- **Écrire un parser maison** : libconfig++ est la dépendance attendue par le sujet. Un parser maison serait des centaines de lignes non testées.
- **`lookupValue` (API non-jetante)** pour lire les champs : plus verbeux (`if (!cam.lookupValue("fov", cfg.fov)) throw ...`). La variante jetante est plus concise pour des champs obligatoires.
- **`double d = s["fov"]` sans cast explicite** : libconfig++ distingue `TypeFloat` et `TypeInt`. Un `fov = 60` (sans point) dans le `.cfg` serait de type `TypeInt` — la conversion implicite vers `double` peut générer une `SettingTypeException` sur certaines versions. Le cast explicite `static_cast<double>(static_cast<int>(s))` est robuste pour les deux.
- **`CameraConfig` avec `std::optional` sur les champs** : tous les champs caméra sont obligatoires pour le rendu ; pas de valeur par défaut raisonnable pour `fov` ou la résolution. Lever une erreur explicite est plus clair.
- **Séparer `SceneParser` en plusieurs classes** (une par section) : prématuré pour T0.10 ; le parser sera étendu en T0.11 et T1.7 quand les sections primitives/lights/materials arriveront.

**Impact**
- **T0.11** (SceneBuilder) : `SceneParser` sera étendu pour lire les sections `primitives`, `lights`, `materials`. Le `SceneBuilder` prend un `SceneParser&` ou les configs brutes.
- **T0.13** (Camera) : `CameraConfig` alimentera directement le constructeur de `Camera`.
- **T1.7** (parser complet) : `parseCamera()` rester intacte ; seules les méthodes `parsePrimitives()` etc. seront ajoutées.
- **`ldd ./raytracer`** : `libconfig++.so` apparaît désormais dans les dépendances du binaire. Ce n'est pas un plugin raytracer — conforme.
- **Dette technique** : `scenes/empty_camera.cfg` doit être mis à jour si le format camera évolue. Le test de régression `./raytracer scenes/empty_camera.cfg → exit 0` est à garder dans la checklist de commit.

**Non couvert volontairement**
- Sections `primitives`, `lights`, `materials`, `import` → T0.11 / T1.7.
- Validation de `fov` (doit être > 0 et < 180), `width`/`height` (> 0) → T0.13 (Camera sait ce qui est valide pour elle).
- Format `.json` ou `.yaml` alternatif → hors scope sujet.

**Validation**
```bash
make re
./raytracer scenes/empty_camera.cfg; echo $?     # 0 ← critère T0.10 ✓
./raytracer --help; echo $?                       # 0
./raytracer; echo $?                              # 84
./raytracer nope.cfg; echo $?                     # 84 (IOError)
ldd ./raytracer | grep raytracer_                 # vide
```

---

### v0.0.10 — 2026-04-22 — PluginLoader : encapsulation dlopen/dlsym/dlclose (T0.9)

**Type** : feature
**Ticket** : T0.9
**Phase** : 0 (Fondations)

**Résumé**
`rt::PluginLoader` — classe RAII qui scanne `./plugins/*.so`, résout `raytracer_plugin_entry` via `dlsym`, valide le `PluginDescriptor` (7 guards séquentiels), et indexe les plugins valides par `(PluginKind, name)`. Seul fichier du projet autorisé à appeler `dlopen`/`dlsym`/`dlclose`. `libdl` ajouté au linkage.

**Contexte / motivation**
Le critère T0.9 impose : "seul endroit avec `dlopen`/`dlsym`/`dlclose`". Concentrer ces appels ici garantit qu'aucun autre module ne peut fuir un handle ou oublier un `dlclose`. La logique de validation est également centralisée : un plugin mal formé (ABI mismatch, descripteur nul, entrée manquante) est rejeté proprement avec un warning lisible sur `stderr`, sans jamais crasher le processus.

**Détails techniques**

- **`src/plugins/PluginLoader.hpp`** — interface publique, sans `<dlfcn.h>`.
  - Constructeur `explicit PluginLoader(fs::path pluginDir = "./plugins")` : scanne et charge.
  - Destructeur déclaré (défini dans `.cpp` pour garder `<dlfcn.h>` hors du header).
  - Non-copiable (handles uniques), movable (ownership transféré, ancien cleared).
  - `find(kind, name_view) → const PluginDescriptor*` : O(log n) via `std::map`.
  - `count() → size_t` : nombre de plugins valides chargés.
  - Membres privés : `std::vector<void*> handles_` + `std::map<pair<PluginKind, string>, const PluginDescriptor*> registry_`.
- **`src/plugins/PluginLoader.cpp`** — seul fichier qui `#include <dlfcn.h>`.
  - **`sym_to_fn<FnPtr>(void*)`** (template local anonyme) : workaround POSIX pour caster `void*` → pointeur de fonction sans violer `-Wpedantic`. Utilise `std::memcpy` + `static_assert(sizeof(FnPtr) == sizeof(void*))`. La norme ISO C++ interdit le `reinterpret_cast` entre data et function pointer ; `memcpy` contourne l'UB formellement.
  - **`loadOne(path)` — 7 guards séquentiels** :
    1. `dlopen(RTLD_NOW | RTLD_LOCAL)` → warning + return sur échec. `RTLD_NOW` : résolution immédiate (erreur détectée au load, pas au premier appel). `RTLD_LOCAL` : symboles du plugin isolés (pas de pollution globale).
    2. `dlsym("raytracer_plugin_entry")` → warning + `dlclose` + return si absent.
    3. Appel de `entry()` → warning + `dlclose` si retourne nullptr.
    4. `desc->name` non vide → warning + `dlclose` si vide.
    5. `desc->create` et `desc->destroy` non nuls → warning + `dlclose`.
    6. `desc->abi_version == RAYTRACER_ABI_VERSION` → warning avec versions attendues/reçues + `dlclose`.
    7. Pas de doublon `(kind, name)` dans le registry → warning "first wins" + `dlclose`.
    - **Handle enregistré uniquement si tous les guards passent** → pas de fuite si un guard échoue en milieu de chaîne.
  - **Destructeur** : parcourt `handles_` en ordre **inverse** (LIFO — miroir de l'ordre de load, respecte les dépendances éventuelles entre plugins).
  - **Move** : `operator=` ferme les handles courants avant le move ; source est cleared (handles nullifiés pour que son destructeur ne double-close pas).
- **`src/CMakeLists.txt`** : ajout de `plugins/PluginLoader.cpp` à l'exécutable + `target_link_libraries(... dl)`.
- **Chargement du répertoire** : utilise `std::filesystem::is_directory` avec `std::error_code` (no-throw) avant `directory_iterator`. Si `./plugins/` n'existe pas → silencieux (attendu en Phase 0 avant T0.15).

**Alternatives rejetées**
- **`reinterpret_cast<FnPtr>(sym)` pour dlsym** : interdit par ISO C++ (violé par `-Wpedantic -Werror`). `memcpy` est l'alternative idiomatique documentée dans la FAQ POSIX.
- **`RTLD_LAZY` au lieu de `RTLD_NOW`** : les symboles non résolus au load resteraient invisibles jusqu'au premier appel → crash en production au lieu d'une erreur lisible au démarrage.
- **`RTLD_GLOBAL` au lieu de `RTLD_LOCAL`** : les symboles du plugin entrent dans l'espace global → risque de collision si deux plugins définissent une fonction portant le même nom. `RTLD_LOCAL` est plus sûr.
- **`std::unique_ptr<void, decltype(&dlclose)>` comme handle** : `unique_ptr<void>` n'est pas légal (`void` n'est pas un type complet, `delete` sur `void*` est UB). Un wrapper struct ou un raw `void*` + boucle dans le destructeur sont les vraies options. Le raw `void*` + vector est plus lisible.
- **Map par `std::string` seulement** : deux plugins de types différents avec le même nom (e.g. une `Primitive "sphere"` et une `Light "sphere"`) coexistent légitimement. La clé est bien `(kind, name)`.
- **Singleton** : le loader est instancié et tenu en vie par `runScene()` / futur `main` — pas de singleton global. Testable, destructible proprement.
- **Exceptions dans loadOne** : un plugin cassé ne doit pas couper court au scan des autres. Warnings sur `stderr` + continue est la politique correcte.
- **`std::unordered_map` avec hash pour `pair<PluginKind, string>`** : O(1) vs O(log n) sur quelques dizaines de plugins → différence négligeable. `std::map` évite d'écrire un hash custom.

**Impact**
- **T0.10+** : `runScene()` dans `main.cpp` pourra instancier un `PluginLoader`, puis transmettre sa référence au `SceneBuilder` pour résoudre les types de plugins depuis le fichier `.cfg`.
- **T0.15** (plugin factice sphère) : `PluginLoader("./plugins").find(Primitive, "sphere")` renvoie le descripteur après que `raytracer_sphere.so` a été chargé.
- **`ldd ./raytracer`** : ne liste aucun `raytracer_*.so` → conforme Epitech. `libdl` est intégré à `libc` sur Ubuntu 22+ (pas visible dans `ldd` mais le link `-ldl` est présent et utile sur les systèmes plus anciens).
- **Dette technique** : les plugins ne sont pas encore déchargés dans un ordre dépendant des scènes (ordre inverse de chargement est une heuristique). Si un plugin A alloue un objet de type défini par le plugin B, le destructeur de A doit tourner avant que B ne soit déchargé. Problème potentiel uniquement si on ajoute des plugins qui dépendent les uns des autres — hors scope pour l'instant.

**Non couvert volontairement**
- **Hot-reload** (bonus T4.18) : `dlclose` + `dlopen` sans redémarrer l'executable → ajouté quand SFML/preview sera implémenté.
- **Chargement depuis des chemins configurables** (flag CLI `--plugin-dir`) → si besoin Phase 4.
- **API d'énumération des plugins chargés** (`listAll()`) → pas encore nécessaire ; le `SceneBuilder` cherchera par `find()`.

**Validation**
```bash
make re                                # 0 warning, libdl lié
./raytracer --help; echo $?            # exit 0
./raytracer; echo $?                   # exit 84
ldd ./raytracer | grep raytracer_      # vide — aucun plugin linké en dur
# Test du loader avec un faux .so :
mkdir -p /tmp/fakepl && echo "garbage" > /tmp/fakepl/raytracer_fake.so
./raytracer /tmp/valid.cfg 2>&1 | head  # warning dlopen lisible (si PluginLoader appelé)
```
Build 0 warning confirmé. RAII (dlclose LIFO) et 7 guards vérifiés par lecture de code. Regression CLI : aucune.

---

### v0.0.9 — 2026-04-22 — ABI plugins : PluginDescriptor, RAYTRACER_ABI_VERSION (T0.8)

**Type** : feature
**Ticket** : T0.8
**Phase** : 0 (Fondations)

**Résumé**
Introduction de `src/interfaces/plugin_abi.hpp` : le contrat C-linkage que tout plugin `.so` exporte. Définit `RAYTRACER_ABI_VERSION`, l'enum `PluginKind`, le POD `PluginDescriptor`, la signature `extern "C" raytracer_plugin_entry()` et la macro `RAYTRACER_PLUGIN_ENTRY` pour simplifier l'écriture d'un plugin.

**Contexte / motivation**
Le PluginLoader (T0.9) chargera des `.so` avec `dlopen`/`dlsym`. Pour trouver le code du plugin, il cherche le symbole `raytracer_plugin_entry` — un pointeur de fonction C (pas de mangling) qui renvoie un `PluginDescriptor`. Ce descriptor est le **seul point d'entrée** du plugin ; il contient tout ce dont le core a besoin : type de plugin, nom, fabrique, destructeur.

Plusieurs contraintes à concilier :
- **Stabilité ABI** : si le format de `PluginDescriptor` change un jour, les anciens `.so` ne doivent pas crasher — ils doivent être rejetés proprement. D'où `abi_version` vérifié à load-time par le PluginLoader.
- **Pas d'exceptions inter-boundary** : une exception qui traverserait `dlopen` provoquerait un comportement indéfini (stack unwinding incompatible). La convention est que `create`/`destroy` capturent tout en interne.
- **Légèreté** : `plugin_abi.hpp` est le seul header du core que les plugins doivent obligatoirement inclure. On ne veut pas y tirer toute la STL. Forward-declaration de `libconfig::Setting` suffit.

**Détails techniques**

- **`RAYTRACER_ABI_VERSION = 1`** : `inline constexpr uint32_t` dans `namespace rt`. Le PluginLoader (T0.9) compare `desc->abi_version == RAYTRACER_ABI_VERSION` ; mismatch → warning stderr + rejet propre (pas de `std::terminate`).
- **`PluginKind : uint32_t`** : enum class à underlying type fixe — taille garantie (`sizeof == 4`) quel que soit le compilateur ou la plateforme. Valeurs : `Primitive = 0`, `Light = 1`, `Material = 2`. Ajout futur (e.g. `PostProcess = 3`) = breaking change → bump ABI.
- **`PluginDescriptor`** : POD agrégat.
  - `abi_version` : rempli par la macro, jamais hardcodé à la main par le plugin.
  - `kind` : le PluginLoader l'utilise pour caster `create()` → `IPrimitive*` / `ILight*` / `IMaterial*`.
  - `name` : `const char*` litéral (duration statique). Clé de lookup dans le registre (kind, name) du PluginLoader et dans le fichier `.cfg`. Convention : lowercase ASCII, underscore pour les espaces (e.g. `"flat_color"`).
  - `create(const libconfig::Setting&) → void*` : fabrique héap. Le bloc `libconfig::Setting` vient directement du parser (T0.10) — le plugin lit ses paramètres depuis la config sans que le core sache ce qu'il lit.
  - `destroy(void*)` : appelé à la destruction de la Scene. Ne doit pas throw.
- **Forward-decl `libconfig::Setting`** : `namespace libconfig { class Setting; }` dans le header — le type est utilisé dans une signature de pointeur de fonction, ce qui ne nécessite pas la définition complète. Évite de tirer `<libconfig.h++>` dans le header partagé.
- **`extern "C" raytracer_plugin_entry()`** : déclaré hors de `namespace rt` (les symboles `extern "C"` vivent dans l'espace global). Le `extern "C"` supprime le mangling → `dlsym("raytracer_plugin_entry")` trouve le symbole indépendamment du compilateur.
- **Macro `RAYTRACER_PLUGIN_ENTRY(kind, name, create, destroy)`** : génère la définition complète de `raytracer_plugin_entry()` avec `abi_version` rempli automatiquement. Usage typique dans un plugin :
  ```cpp
  RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "sphere",
                         &SpherePlugin::create, &SpherePlugin::destroy)
  ```
  Avantage : le plugin auteur ne peut pas oublier de mettre à jour `abi_version` manuellement.
- **Aucune modification CMake** : `plugin_abi.hpp` est dans `src/interfaces/`, déjà exposé par `rt_interfaces` (T0.7). Les futurs plugins lieront simplement `rt_interfaces` pour l'obtenir.

**Alternatives rejetées**
- **Utiliser `std::string_view` pour `name`** : `std::string_view` a un layout défini par l'ABI C++ (dépend du compilateur/stdlib), et pointer dans une string_view vers un litéral statique introduit une dépendance subtile. `const char*` litéral est la convention C portable et universellement comprise.
- **Faire `PluginKind` un `enum` (non scoped)** : `enum class` avec underlying type fixe garantit la taille. Un `enum` non-scoped aurait un underlying type dépendant du compilateur.
- **Mettre le `name` dans un `std::string` membre** : POD non trivial, pas copiable bit-à-bit → hors spec ABI stable.
- **Un symbole exporté différent par type** (`raytracer_primitive_entry`, `raytracer_light_entry`…) : multiplie les points d'entrée ; le PluginLoader doit tenter plusieurs `dlsym`. Un seul `raytracer_plugin_entry` est plus simple.
- **Inclure `<libconfig.h++>` directement** : alourdit le header partagé, force tous les plugins à compiler avec les headers libconfig installés au bon endroit. La forward-declaration suffit pour déclarer le pointeur de fonction.
- **`std::function<void*(const libconfig::Setting&)>` au lieu de `void*(*)(...)` raw** : `std::function` n'est pas POD, a un layout dépendant de l'implémentation, et introduit un overhead de type erasure inutile.
- **Exceptions dans `create`/`destroy`** : traverser la frontière `dlopen` avec une exception = UB en C++ (deux bibliothèques C++ potentiellement différentes, stacks de déwinding incompatibles). `nullptr` + log stderr est la convention.
- **Pas de macro `RAYTRACER_PLUGIN_ENTRY`** : sans la macro, chaque plugin hardcoderait `abi_version = 1`. Quand la version bumpe à `2`, tous les plugins compilés sans recompilation renverraient `1` → rejet correct par le loader. Mais l'auteur qui oublie de recompiler voit son plugin silencieusement rejeté sans savoir pourquoi. La macro compile `RAYTRACER_ABI_VERSION` dans le `.so` → la version dans le binaire reflète toujours celle des headers avec lesquels il a été compilé.

**Impact**
- **T0.9** (PluginLoader) : `dlsym("raytracer_plugin_entry")` retourne `const rt::PluginDescriptor*`. Le loader caste vers le bon type selon `kind`, vérifie `abi_version`.
- **T0.15** (plugin factice sphère) : `RAYTRACER_PLUGIN_ENTRY(Primitive, "sphere", ...)` — 5 lignes pour définir l'entry point.
- **Toute la Phase 1** : chaque plugin de la Phase 1 est structuré autour de ce contrat. L'ajouter en Phase 0 garantit qu'il n'y aura pas de refactor.
- **`ldd ./raytracer`** : le binary ne lie pas `libconfig++` encore — le loader l'utilisera en T0.9. Pas de régression.

**Non couvert volontairement**
- **ABI version check** (rejet à load-time) → T0.9 (PluginLoader).
- **`PluginKind::Node`** pour des nœuds de scene graph exportés par plugin → hors scope sujet. Le Composite pattern vit dans le core.
- **Chargement de plusieurs descriptors depuis un seul `.so`** (un `.so` qui fournit plusieurs primitives) → non supporté. Un `.so` = un `raytracer_plugin_entry` = un type. Architecture plus simple.
- **Hot-reload** → bonus T4.18. L'ABI est stable pour le permettre, mais le PluginLoader ne l'implémente pas encore.

**Validation**
```bash
make re                                # rebuild Release, 0 warning
./raytracer --help; echo $?            # exit 0
./raytracer; echo $?                   # exit 84
ldd ./raytracer | grep raytracer_      # vide → aucun plugin linké
```
`plugin_abi.hpp` compilé avec un plugin simulé (macro + create/destroy) ET du côté core (include avec les 4 interfaces) sous `-Wall -Wextra -Werror -pedantic -std=c++20` — 0 warning. `static_assert(rt::RAYTRACER_ABI_VERSION == 1)` vérifié.

---

### v0.0.8 — 2026-04-22 — Interfaces publiques : IPrimitive, INode, ILight, IMaterial (T0.7)

**Type** : feature
**Ticket** : T0.7
**Phase** : 0 (Fondations)

**Résumé**
Définition des quatre interfaces polymorphes centrales du pipeline : `rt::IPrimitive` (géométrie + UV), `rt::INode` (Composite, nœud de scene graph), `rt::ILight` + `rt::LightSample` (lumière + résultat d'échantillonnage), `rt::IMaterial` (Strategy, calcul de couleur). Introduction d'une cible CMake `rt_interfaces` (INTERFACE library) qui expose `src/` comme include root pour le core **et** les futurs plugins `.so`.

**Contexte / motivation**
T0.6 a posé les types POD partagés (`Ray`, `HitRecord`, `ShadingContext`, `AABB`). T0.7 pose le **contrat comportemental** au-dessus : les classes abstraites que le renderer utilise pour demander "intersecte-toi avec ce rayon" ou "calcule ta couleur". Ces interfaces sont la **frontière stable** entre le core et les plugins dynamiques — un plugin `.so` implémente `IPrimitive` ou `IMaterial` et n'a besoin de connaître que cette interface.

Le `CMakeLists.txt` de `src/interfaces/` crée une `INTERFACE library` CMake (`rt_interfaces`) qui ne compile rien mais propage l'include path `src/`. Tout futur plugin écrira juste `target_link_libraries(raytracer_sphere PRIVATE rt_interfaces)` et aura accès à tous les types partagés sans redéfinir d'include path.

**Détails techniques**

- **`src/interfaces/IPrimitive.hpp`** — interface géométrique.
  - `intersect(Ray, HitRecord&) → bool` : remplie `rec` sur hit (t, point, normal, uv, frontFace). Renvoie false sur miss sans modifier rec.
  - `bounds() → AABB` : en **espace objet** (avant le Transform du nœud parent). Utilisé par le BVH.
  - `uvAt(Vector3 localPoint) → Vector2` : paramétrisation de la surface. Séparé d'intersect pour ne pas calculer les UV quand le matériau n'en a pas besoin.
- **`src/interfaces/INode.hpp`** — nœud de scene graph (Composite).
  - `intersect(Ray, HitRecord&) → bool` : parcourt le sous-arbre, renvoie le hit **le plus proche** en espace monde.
  - `bounds() → AABB` : en **espace monde** (post-transform). Le renderer et le BVH ne parlent qu'en espace monde.
  - `addChild(shared_ptr<INode>)` : no-op par défaut (overridé par GroupNode). Permet au SceneBuilder d'ajouter des enfants sans downcast.
- **`src/interfaces/ILight.hpp`** — lumière + `LightSample`.
  - `LightSample { direction, radiance, distance }` : direction unitaire vers la lumière, radiance HDR incidente, distance pour borner les shadow rays (`tMax = distance`). Directional → `distance = +∞`.
  - `sample(surfacePoint) → LightSample` : pure virtual.
  - `isAmbient()` / `ambientContribution()` : défauts `false` / `black()`. Overridés par les lumières ambiantes seulement. Le renderer teste `isAmbient()` et appelle `ambientContribution()` une fois par point, séparément des lumières directionnelles.
- **`src/interfaces/IMaterial.hpp`** — matériau (Strategy).
  - `shade(ShadingContext) → Color` : pure virtual. Reçoit la scène entière (pour lancer des rayons secondaires), le hit record, le rayon entrant et le compteur de rebonds.
  - Contrat de performance : allocation-free sur le stack (pas de heap dans le chemin shade).
- **`src/interfaces/CMakeLists.txt`** — cible CMake INTERFACE.
  - `add_library(rt_interfaces INTERFACE)` + `target_include_directories(rt_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)` (= `src/`).
  - Consommé par `target_link_libraries(raytracer PRIVATE rt_interfaces)` dans `src/CMakeLists.txt`.
  - Les plugins l'utiliseront de la même façon (`target_link_libraries(raytracer_sphere PRIVATE rt_interfaces)` en T0.15).
- **`src/CMakeLists.txt`** : ajout de `add_subdirectory(interfaces)` et `target_link_libraries(raytracer PRIVATE rt_interfaces)`. Le `target_include_directories(raytracer PRIVATE src/)` explicite peut maintenant être retiré puisque `rt_interfaces` expose la même path — maintenu implicitement via le lien INTERFACE.

**Alternatives rejetées**
- **Tout mettre dans un seul header `interfaces.hpp`** : les includes circulaires deviendraient inévitables rapidement (IPrimitive utilise AABB, INode utilise INode, etc.). Un header par classe = dépendances explicites et tracées.
- **`IPrimitive` et `INode` fusionnés** : architecturalement erroné. Un IPrimitive est une géométrie pure (sphère unité, plan Z=0, cylindre canonique) ; un INode est une entité de scene graph avec un Transform, potentiellement enfants. Les séparer permet d'avoir plusieurs `IPrimitive`s partagées entre des `INode`s différents.
- **Inclure `<memory>` partout** : `<memory>` n'est inclus que dans `INode.hpp` où `shared_ptr<INode>` est utilisé. Les autres headers n'en ont pas besoin.
- **Rendre `addChild` pure virtual** : forcerait chaque implementer à déclarer une méthode vide. Le no-op default est plus ergonomique pour les leaf nodes, et la sémantique est documentée.
- **Stocker `LightSample.direction` comme normalisation garantie par l'appelant** : c'est bien la convention retenue. Les plugins sont responsables de renvoyer une direction unitaire. Le renderer n'appelle pas `normalized()` sur le résultat (économie de sqrt dans les hot paths).
- **Mettre `LightSample` dans un header séparé** : POD à 3 champs, utilisé uniquement avec `ILight`. Le grouper dans `ILight.hpp` évite un header de 10 lignes.
- **CMake `add_library(rt_interfaces STATIC)` au lieu de `INTERFACE`** : aucun `.cpp` dans les interfaces → un STATIC target vide est une erreur CMake. L'INTERFACE library est exactement le bon outil pour les header-only dependencies.
- **Propager l'include path via `target_include_directories` global** : anti-pattern CMake moderne. Chaque cible devrait déclarer ses dépendances explicitement via `target_link_libraries`.

**Impact**
- **T0.8** (ABI plugins) : `PluginDescriptor` peut maintenant référencer `IPrimitive`, `INode`, `ILight`, `IMaterial` par pointeur opaque — les types existent.
- **T0.11** (SceneBuilder + Scene) : `Scene` peut stocker des `shared_ptr<INode>`, `shared_ptr<ILight>`, `shared_ptr<IMaterial>`.
- **T0.15** (plugin factice sphère) : le plugin `raytracer_sphere.so` implémente `IPrimitive`, lie `rt_interfaces`, le core le charge et l'utilise via l'interface.
- **T4.1** (Phong) : `IMaterial::shade()` reçoit `ShadingContext` qui contient la scène → les rayons secondaires (réflexion, ombres) sont lancés depuis shade.
- **Dette technique** : aucune. La coupure core/plugin est propre et versionnée via `rt_interfaces`. Ajouter une méthode à une interface est un breaking change ABI → sera géré via `RAYTRACER_ABI_VERSION` (T0.8).

**Non couvert volontairement**
- **Implémentations concrètes** (`SphereNode`, `FlatColorMaterial`, etc.) → Phase 1.
- **`Scene`** (pour que `ShadingContext::scene` soit utilisable) → T0.11.
- **`LightSample::pdf`** (densité de probabilité pour Monte Carlo) → hors scope sujet, éventuel bonus avancé.
- **`IEnvironment`** (lumière d'arrière-plan / sky) → éventuel bonus.

**Validation**
```bash
make re                                # rebuild Release, 0 warning
./raytracer --help; echo $?            # exit 0 — CLI intact
./raytracer; echo $?                   # exit 84 — CliError intact
./raytracer nope.cfg; echo $?          # exit 84 — IOError intact
ldd ./raytracer | grep raytracer_      # vide → aucun plugin linké
```
Les 4 headers compilent avec stubs concrets sous `-Wall -Wextra -Werror -pedantic -std=c++20` (vérifié via fichier .cpp jetable — 0 warning). `rt_interfaces` apparaît dans le graphe CMake (`cmake --graphviz`). Aucune régression CLI.

---

### v0.0.7 — 2026-04-22 — Types POD partagés : Ray, HitRecord, ShadingContext, AABB (T0.6)

**Type** : feature
**Ticket** : T0.6
**Phase** : 0 (Fondations)

**Résumé**
Introduction des quatre types POD qui forment le **vocabulaire commun** du pipeline de rendu — utilisés identiquement par le core et par les plugins : `rt::Ray` (rayon paramétrique), `rt::HitRecord` (résultat d'intersection), `rt::ShadingContext` (bundle remis à `IMaterial::shade`), `rt::AABB` (bounding box axis-aligned). Tous résident dans `src/interfaces/` pour signifier leur statut d'**interface d'échange** entre les modules.

**Contexte / motivation**
Le pipeline de rendu s'articule autour d'un enchaînement standard : un rayon traverse la scène → chaque primitive rapporte son intersection éventuelle via `intersect(Ray, HitRecord&)` → la plus proche des hits est sélectionnée → le matériau du hit est interrogé via `shade(ShadingContext)` pour produire une couleur. Avant d'écrire les interfaces `IPrimitive`/`INode`/`ILight`/`IMaterial` (T0.7), ces quatre types doivent exister sous forme **stable, POD, minimale** :
- **POD** : ils traversent la frontière ABI core ↔ plugins sans gymnastique (pas de méthodes virtuelles, pas de layout dépendant du compilateur).
- **Headers self-contained** : chaque type doit pouvoir être inclus isolément sans tirer des dépendances cycliques. `IMaterial`, `INode`, `Scene` arrivent en T0.7/T0.11 → forward declarations.
- **Placement dans `src/interfaces/`** : ce dossier est la **seule surface d'API publique** vue par les plugins ; le placer ici verrouille sa stabilité.

`Ray` embarque déjà l'intervalle paramétrique `[tMin, tMax]` (utilisé par le BVH bonus T3.3 et par les shadow rays) et un epsilon partagé pour éviter la self-intersection. `HitRecord` porte les pointeurs non-propriétaires vers le matériau et le nœud, qui sont owned par la Scene (convention verrouillée dès T0.1). `AABB` adopte le sentinel inversé (+∞ / −∞) qui rend `expanded()` et `merged()` triviaux. `ShadingContext` est bâti sur la pile à chaque appel — aucune copie, aucune ownership, uniquement des références const.

**Détails techniques**

- **`src/interfaces/Ray.hpp`** — POD rayon paramétrique.
  - Champs : `origin` (Vector3), `direction` (Vector3, **unit attendu** — non vérifié par le type ; les constructeurs de rayons dans `Camera`/shadow/reflexion se chargent de la normalisation).
  - Intervalle : `tMin = kRayEpsilon` (petit offset positif qui évite qu'un rayon réfléchi/réfracté ré-intersecte immédiatement sa surface d'origine), `tMax = +∞`.
  - Constante partagée : `inline constexpr double kRayEpsilon = 1e-4;`. Le choix de `1e-4` est un compromis : assez grand pour absorber les erreurs d'arrondi double sur des scènes d'échelle unitaire, assez petit pour ne pas créer de "gap" visible quand deux surfaces sont tangentes.
  - Helper `at(t)` : `constexpr noexcept` — `origin + t * direction`.
- **`src/interfaces/HitRecord.hpp`** — résultat POD d'intersection.
  - Champs : `t` (paramètre du rayon au hit), `point` (position monde), `normal` (unitaire, espace monde), `uv` (paramétrisation surface pour texturing), `material` (`const IMaterial*`, non-propriétaire), `node` (`const INode*`, non-propriétaire), `frontFace` (bool — face extérieure ou intérieure, rempli par la primitive).
  - **Forward declarations** : `class IMaterial;` et `class INode;` — leurs définitions arrivent en T0.7. Le header reste self-contained.
  - **Ownership explicite** : le commentaire en tête mentionne "raw pointers NON-OWNING into the Scene" pour écarter toute ambiguïté (pas de `delete` à faire).
- **`src/interfaces/ShadingContext.hpp`** — bundle remis au matériau.
  - Champs : `const Scene&`, `const HitRecord&`, `const Ray& incomingRay`, `int depth = 0`.
  - **Forward declaration** : `class Scene;` — sa définition arrive en T0.11.
  - Construit **toujours sur la pile** au site d'appel de `shade()`, jamais stocké → pas de constructeur par défaut, pas de copie.
  - `depth` compte les rebonds récursifs (0 pour un rayon primaire, 1 pour la première réflexion/réfraction…). Permet aux matériaux de stopper la récursion (cf. T4.1/T4.2).
- **`src/interfaces/AABB.hpp`** — bounding box axis-aligned.
  - Champs : `Vector3 min`, `Vector3 max`.
  - **Constructeur par défaut = boîte VIDE sentinel** : `min = (+∞, +∞, +∞)`, `max = (−∞, −∞, −∞)`. Cette convention rend `expanded(p)` correct dès le premier appel dans une boucle d'expansion (`for (p : points) b = b.expanded(p);` donne toujours une boîte finie et bien ordonnée à la sortie).
  - Constructeur explicite `AABB(min, max)` pour construire directement une boîte connue.
  - `isEmpty()` : `true` ssi `min.x > max.x || min.y > max.y || min.z > max.z` — le sentinel le satisfait sur les trois axes.
  - `expanded(Vector3)` : retourne une nouvelle AABB étendue pour inclure `p` (composantwise `min`/`max`).
  - `merged(AABB)` : union de deux boîtes.
  - Toutes les méthodes sont `constexpr noexcept`.
- **Pas de `.cpp`** : les quatre types sont header-only (POD + ops `constexpr`).
- **Pas de modif CMake** : l'include path `src/` existant couvre `src/interfaces/`. Aucun symbole à exporter, pas de lib dédiée (ce sera T0.7 pour la "`interfaces` INTERFACE library").
- **Vérification de compilation** : chaque header a été compilé isolément sous `-Wall -Wextra -Werror -pedantic -std=c++20` (petit fichier jetable incluant les quatre + instanciant chaque type, hors repo) — 0 warning. Les forward declarations suffisent pour un `Ray`/`HitRecord`/`ShadingContext`/`AABB` standalone ; les vraies définitions de `IMaterial`/`INode`/`Scene` seront nécessaires pour **utiliser** ces pointeurs (mais pas pour compiler les POD).

**Alternatives rejetées**
- **Placer ces types dans `src/core/`** : sémantique erronée. `src/core/` = runtime interne (Scene, SceneBuilder, Renderer, Camera). `src/interfaces/` = contrat public partagé avec les plugins. Ces quatre types sont consommés par les plugins (intersect renvoie `HitRecord`, shade reçoit `ShadingContext`, bounds renvoie `AABB`) → ils appartiennent à l'interface.
- **Faire de `HitRecord` / `ShadingContext` des classes avec constructeurs / invariants** : inutile. Ce sont des bundles construits et remplis sur la pile, jamais stockés ; aucun invariant à protéger. Un `struct` POD avec champs publics est le design idiomatique pour un DTO de hot path (cf. `std::pair`, `std::optional`).
- **Intégrer `material` et `node` par `shared_ptr` dans `HitRecord`** : surcoût prohibitif (atomic increment/decrement sur chaque intersection). Les primitives et les nœuds sont owned par la `Scene` (elle-même immutable après construction) → les raw pointers sont sûrs tant que le `HitRecord` ne survit pas à la Scene. La documentation en tête de header rend cette convention explicite.
- **Stocker l'ownership de `material` dans `HitRecord` via `weak_ptr`** : idem — pattern non justifié quand la Scene garantit la durée de vie.
- **Stocker `Scene&` par pointeur dans `ShadingContext`** : les références expriment mieux le non-null invariant à la construction. `ShadingContext` n'est jamais reconstruit ; une référence est strictement supérieure à un pointeur ici.
- **Remplacer `tMin`/`tMax` par un `Interval` séparé** : overkill pour 2 `double`. L'abstraction viendrait polluer tout le code de ray spawning sans gain.
- **Initialiser `frontFace` à `false`** : convention arbitraire ; `true` est cohérent avec la majorité des rayons primaires (on hit la face avant d'un objet). Les primitives écrasent cette valeur de toute façon, donc la valeur par défaut n'a qu'un rôle de doc.
- **Stocker `Color emission` ou `double roughness` dans `HitRecord`** : ces données appartiennent au matériau (accessible via `material->...`). `HitRecord` reste minimal — c'est ce que le **rayon** a rapporté, pas ce que la **surface** stocke.
- **Utiliser un bitset pour des flags futurs (inside/outside, emissive)** : spéculation. On ajoutera un flag quand un use-case concret l'exigera.
- **`AABB` empty = `{0,0,0}, {0,0,0}`** : piège classique — `expanded((1,1,1))` donnerait `{{0,0,0},{1,1,1}}`, mais si tous les points sont négatifs, `min` resterait à `(0,0,0)` (faux). Le sentinel `(+∞,−∞)` est universellement correct.

**Impact**
- **T0.7** peut maintenant définir `IPrimitive::intersect(const Ray&, HitRecord&) const` et `IMaterial::shade(const ShadingContext&) const` sans définir de nouveau type au passage.
- **T0.8** (ABI plugins) : ces POD sont les seuls types "hors primitive C" qui peuvent traverser la frontière `extern "C"` de façon stable — leur simplicité garantit l'ABI.
- **T1.1+** (primitives) : chaque plugin remplit un `HitRecord` et retourne une `AABB` — le vocabulaire est prêt.
- **T3.3** (BVH bonus) : `AABB::merged` est l'opération centrale du build récursif.
- **Dette technique** : aucune. Les types sont minimaux, non-propriétaires, et leurs conventions sont documentées en tête de chaque header.

**Non couvert volontairement**
- **Intersection `AABB ↔ Ray`** (slab method) : utile pour le BVH (T3.3). Sera ajouté comme méthode `AABB::intersect(Ray, double& tHit)` quand le BVH arrivera — garder le type minimal jusque-là évite les anticipations inutiles.
- **`HitRecord::isHit()`** / champ `bool hit` : inutile — la signature `intersect(Ray, HitRecord&) -> bool` porte déjà le statut. Le `HitRecord` n'a de sens que si `true` est retourné.
- **Tracking du médium traversé** (indice de réfraction courant, volumes participatifs) : hors scope Phase 0. Sera ajouté si/quand les bonus refraction/volumes le demandent.
- **Validation automatique** (compilation isolée via CTest, asserts d'invariants sur `AABB`) : reportée au ticket framework de tests. La compilation isolée a été vérifiée manuellement via un fichier jetable (non commité).

**Validation**
```bash
make re                               # rebuild Release, 0 warning
./raytracer --help; echo $?           # exit 0 — CLI intact
./raytracer; echo $?                  # exit 84 — CliError intact
./raytracer nope.cfg; echo $?         # exit 84 — IOError intact
ldd ./raytracer | grep raytracer_     # vide → aucun plugin linké
```
Les quatre headers compilent seuls sous `-Wall -Wextra -Werror -pedantic -std=c++20` (vérifié via un fichier .cpp jetable hors repo, incluant chacun en isolation puis les quatre ensemble — 0 warning). Aucune régression CLI. Critère d'acceptation T0.6 (compilent seuls, inclus sans dépendance cyclique grâce aux forward decls) **satisfait structurellement**.

---

### v0.0.6 — 2026-04-22 — Math : Matrix4 + Transform (T0.5)

**Type** : feature
**Ticket** : T0.5
**Phase** : 0 (Fondations)

**Résumé**
Introduction de `rt::Matrix4` (matrice 4x4 row-major, `double`) et `rt::Transform` (wrapper qui cache la matrice + son inverse pré-calculé). Factories analytiques pour les transformations usuelles (translation, scaling, rotations autour des axes), inverse numérique général via Gauss-Jordan avec pivot partiel, et application correcte aux normales via `(M⁻¹)ᵀ`.

**Contexte / motivation**
Les primitives du raytracer seront définies en **espace objet canonique** (sphère unité, plan Z=0, etc.) et positionnées dans la scène via un `Transform`. Chaque intersection rayon/primitive demande donc de transformer le rayon en espace objet (multiplication par l'inverse du Transform) — une opération appelée des millions de fois par frame. Conséquences architecturales :
- **Il faut pré-calculer l'inverse** : recalculer une Gauss-Jordan dans la boucle de rendu serait catastrophique.
- **Il faut gérer correctement les normales** : sous un scale non uniforme, les normales ne transforment PAS comme les directions ; la règle correcte est `n' = (M⁻¹)ᵀ · n`. Avoir l'inverse déjà en mémoire rend `applyToNormal()` gratuit.
- **Les factories doivent construire l'inverse analytiquement** quand on le connaît (translation → négation, rotation → transposée, scaling → reciproque) : c'est plus rapide ET plus précis que d'inverser numériquement.

Le ticket prépare également T4.4 (bonus "transformation matrix" = 2 pts) et T4.5 (scene graph = 2 pts) : ces deux bonus sont désormais **structurellement acquis**, il ne restera qu'à les documenter/exposer.

**Détails techniques**

- **`src/math/Matrix4.hpp`** — classe 4x4 row-major.
  - Stockage : `std::array<double, 16>` (flat, contigu, cache-friendly ; row-major → élément `(r,c)` à `r*4+c`).
  - **Default constructor → identité** (convention PBRT / GLM ; neutre pour la multiplication).
  - Constructeur explicite 16 valeurs (row-major).
  - Accès par `operator()(row, col)` (lecture + écriture).
  - Factories `constexpr` : `identity()`, `translation(dx,dy,dz)`, `scaling(sx,sy,sz)`. Rotations non-`constexpr` (std::cos/sin ne le sont pas en C++20 ; ce sera résolu en C++23).
  - Opérations `constexpr` : `operator*` (multiplication triple boucle), `transposed()`, `operator==` (défaultée, bit-exact), `applyToPoint` (w=1, ligne translation incluse), `applyToDirection` (w=0, ligne translation ignorée).
  - `inverse()` → implémentation dans `.cpp`, lève `rt::MathError` si singulière.
- **`src/math/Matrix4.cpp`** — `inverse()` via Gauss-Jordan avec pivot partiel :
  - Matrice augmentée `[A | I]` de 4×8, réduite en `[I | A⁻¹]`.
  - **Pivot partiel** : à chaque colonne, on échange la ligne courante avec celle ayant le plus grand `|a[r][col]|` → stabilité numérique sur les scales extrêmes.
  - Seuil de singularité `kSingularityThreshold = 1e-12` (bien au-dessus de l'epsilon double pour rejeter les matrices dégénérées au lieu de produire du garbage).
  - Lève `rt::MathError("Matrix4::inverse(): matrix is singular")` → cohérent avec la frontière d'erreurs posée en T0.3.1.
- **`src/math/Transform.hpp`** — wrapper tenant **matrice + inverse cachée**.
  - Default constructor → identité (les deux membres par défaut = identité).
  - Constructeur explicite `Transform(const Matrix4&)` : calcule l'inverse via `Matrix4::inverse()` (peut lever `MathError`).
  - Constructeur **direct** `Transform(matrix, inverse)` : pas de vérification (le caller s'engage que `matrix * inverse == I`). C'est ce qu'utilisent les factories analytiques.
  - Factories analytiques :
    - `translation(dx,dy,dz)` → inverse = translation(-dx,-dy,-dz).
    - `scaling(sx,sy,sz)` → lève `MathError` sur 0 ; inverse = scaling(1/sx, 1/sy, 1/sz).
    - `rotationX|Y|Z(θ)` → inverse = rotationX|Y|Z(-θ) (les rotations sont orthogonales : inverse == transposée).
  - Application : `applyToPoint`, `applyToDirection` délèguent à la matrice ; `applyToNormal` utilise la 3x3 supérieure-gauche de l'inverse, transposée implicitement (accès `inv(k, i)` au lieu de `inv(i, k)`). **Ne renormalise pas** — décision du caller (économie de sqrt quand pas nécessaire).
  - `operator*` (composition) : `this * other` = "appliquer `other` d'abord, puis `this`" (convention math classique). Inverse du résultat = `other.inverse_ * inverse_` (ordre inverse).
  - `inverse()` : **zéro FLOP** — juste un échange des deux membres stockés.
- **`src/math/Transform.cpp`** — implémentations des factories ci-dessus.
- **`src/CMakeLists.txt`** — ajout de `math/Matrix4.cpp` et `math/Transform.cpp` à la cible `raytracer`.
- **Aucune modification de `main.cpp`** : les types math ne sont pas (encore) consommés par le pipeline, leur utilisation commencera en T0.6 (`Ray`) et T0.13 (`Camera`).

**Alternatives rejetées**
- **Ne stocker que la matrice, recalculer l'inverse à chaque usage** : rédhibitoire en perf (des millions de Gauss-Jordan par frame). La mémoire supplémentaire (128 octets par Transform) est négligeable face au gain CPU.
- **Utiliser la formule analytique 4x4 des cofacteurs** (style GLM `inverse()`) : ~300 lignes unrolled, plus rapide sur le papier, mais `inverse()` n'est sur aucun hot path (appelé uniquement en construction de Transform). Gauss-Jordan est 50 lignes, lisible, numériquement robuste.
- **Pas de pivot partiel (Gauss-Jordan naïf)** : instable numériquement dès qu'une ligne a un `a[col][col]` proche de zéro alors qu'une autre ligne offre un meilleur pivot. 10 lignes de plus pour une robustesse critique.
- **`std::array<std::array<double, 4>, 4>` au lieu de flat `std::array<double, 16>`** : même taille mémoire (les compilateurs déplient), mais moins intuitif pour l'ordre row-major et casse certaines optimisations vectorielles. Le flat + `operator()` est la convention des libs modernes.
- **Stocker column-major (convention OpenGL)** : pas de raison pour un raytracer CPU ; row-major est plus intuitif dans du code C++ et matche la notation mathématique standard.
- **Factoriser `Transform` en abstraction polymorphe** (`class ITransform`) : Transform est un **value type**, pas une entité polymorphe. Comme pour Vector3, une interface virtuelle ici = 100 % coût, 0 % bénéfice (cf. discussion T0.4).
- **Perspective divide dans `applyToPoint`** : on ne fait pas de projection perspective via Matrix4 pour l'instant. Quand ce sera nécessaire (si on ajoute un bonus de type caméra orthographique avec vraie projection 4D), ce helper sera revisité. Pour le pinhole actuel (T0.13), la projection se fait directement dans `Camera::generateRay()`.
- **Normaliser automatiquement dans `applyToNormal`** : couteux (sqrt) et pas toujours souhaité (certains algos travaillent avec la longueur). Le caller décide.
- **Lever `MathError` dès la construction d'un scaling(0,…)** : choix cohérent avec la frontière d'erreurs posée en T0.3.1 — le scaling singulier ne peut pas être représenté comme Transform car son inverse n'existe pas.
- **Sanity check / assertions embarquées** : non (politique validée le 2026-04-22). La validation automatique des critères d'acceptation viendra avec le framework de tests.

**Impact**
- **T0.6** (`Ray`, `HitRecord`) : le type `Transform` est disponible pour transformer rayons et normales entre espace monde et espace objet.
- **T0.13** (`Camera`) : la caméra pinhole utilisera `Transform` pour sa position + orientation dans le monde.
- **T1.x** (primitives) : chaque primitive portera un `Transform` et intersectera en espace objet canonique. Le bonus "transformation matrix" (T4.4, 2 pts) est **acquis** dès que ces primitives l'utilisent.
- **T4.5** (scene graph) : `INode` composite pourra composer des `Transform` naturellement via `operator*`. Bonus acquis structurellement.
- **Dette technique** : aucune. Les APIs sont stables, les responsabilités bien séparées (Matrix4 = pure math, Transform = politique de cache + erreurs).

**Non couvert volontairement**
- **`AABB`** (bounding box) → T0.6.
- **Projection perspective** dans `applyToPoint` (w != 1) → éventuel bonus caméra avancée.
- **Transformations Scale/Shear bonus** (T4.14) : la matrice supporte déjà le shear via construction manuelle ; il restera à exposer une factory dédiée et à l'accepter dans le parser.
- **Quaternions** pour les rotations : les rotations autour des axes X/Y/Z suffisent pour le sujet et l'ensemble des bonus de rotation. Un ajout quaternion serait du refactor inutile.
- **Validation automatique** (critères PLAN.md `translation(d).applyToPoint(p) == p + d` et `inverse().apply(apply(p)) ≈ p`) : les critères sont satisfaits **par construction** (les factories sont analytiques, pas numériques), et la validation formelle sera faite via GoogleTest + CTest quand le ticket framework sera ouvert.

**Validation**
```bash
make re                               # rebuild Release, 0 warning
make debug                            # rebuild Debug (ASan+UBSan), 0 warning
./raytracer --help; echo $?           # exit 0 — CLI intact
./raytracer; echo $?                  # exit 84 — CliError intact
./raytracer nope.cfg; echo $?         # exit 84 — IOError intact
ldd ./raytracer | grep raytracer_     # vide → aucun plugin linké
```
Les types math compilent proprement sous `-Wall -Wextra -Werror -pedantic` (Release + Debug+sanitizers). Aucune régression sur la CLI. Les critères du ticket sont satisfaits par construction (factories analytiques, inverse cachée), validation formelle reportée.

---

### v0.0.5 — 2026-04-22 — Math : Vector2, Vector3, Color (T0.4)

**Type** : feature
**Ticket** : T0.4
**Phase** : 0 (Fondations)

**Résumé**
Introduction des briques mathématiques de base du raytracer : `rt::Vector2` (UV / espace écran), `rt::Vector3` (points, directions, normales en double précision) et `rt::Color` (RGB flottant en espace linéaire). Toutes les opérations usuelles sont fournies en `constexpr` où possible.

> **Patch 2026-04-22** : le header `src/math/sanity.hpp` et l'appel `rt::math_sanity_check()` depuis `main()` ont été supprimés juste après la validation du ticket. Décision : pas de tests ad-hoc dans le code de prod — la validation formelle des types math sera faite plus tard via un vrai framework (GoogleTest + CTest, ticket dédié). Voir la section "Patches / bug fixes" ci-dessous.

**Contexte / motivation**
Avant d'attaquer `Matrix4`/`Transform` (T0.5), `Ray`/`HitRecord` (T0.6) et les interfaces (T0.7), on pose les types numériques fondamentaux. Contraintes verrouillées en Phase 0 :
- `Vector3` en **double** : précision nécessaire pour de longues chaînes de transformations et d'intersections ; les erreurs s'accumulent vite en float sur de gros vecteurs de scène.
- `Color` en **float linéaire** : la conversion sRGB + clamp se fait **exclusivement** à l'écriture PPM (T0.12). Stocker en linéaire permet l'empilement HDR (réflexion, lumières multiples, Phong) sans quantification prématurée.
- **Immuabilité** : tous les opérateurs retournent une nouvelle valeur. Aucun `+=`/`-=` exposé pour éviter les mutations silencieuses (cf. `rules/common/coding-style.md`).
- **Exceptions aux frontières uniquement** : `normalized()` lève `rt::MathError` sur vecteur nul — acceptable car les normalisations de scène (camera.up, direction de lumière) se font à la construction, pas dans la boucle de rendu. Les hot-paths devront vérifier `lengthSquared() > 0` eux-mêmes.

**Détails techniques**
- **`src/math/Vector2.hpp`** — POD double x, y.
  - Opérateurs : `+`, `-`, `-unary`, `*double`, `/double`, `==` (`= default`).
  - Multiplication scalaire symétrique : `double * Vector2` via `operator*` libre.
  - `dot`, `lengthSquared`, `length`, `normalized()` (lève `MathError` sur longueur nulle).
  - Tout est `constexpr` sauf `length()` et `normalized()` (dépendent de `std::sqrt`).
- **`src/math/Vector3.hpp`** — POD double x, y, z.
  - Même surface que Vector2, plus `cross()` (convention main droite : ex×ey=ez).
  - `normalized()` lève `rt::MathError` — ce qui justifie a posteriori le raffinement `MathError` introduit en T0.3.1.
- **`src/math/Color.hpp`** — POD float r, g, b.
  - Constructeurs + factories `Color::black()`, `Color::white()` (`static constexpr`).
  - Opérateurs : `+`, `-` (accumulation additive), `*float` / `/float` (intensité), `*Color` (produit **composante par composante** = albedo de surface × radiance incidente).
  - Comparaison `==` défaultée.
  - **Pas de clamp, pas de conversion sRGB ici** : c'est le rôle de `PPMWriter` (T0.12). Les valeurs > 1.0 sont légales (HDR).
- **Aucun `.cpp`** ajouté : tout est header-only (inline / constexpr). Pas de nouvelle cible CMake ; `target_include_directories(raytracer PRIVATE src/)` existant suffit.
- **Validation** : aucune suite de tests embarquée dans le code. La validation formelle des types math passera par un vrai framework (GoogleTest + CTest) dans un ticket dédié, jamais par des asserts manuels dans le source.

**Alternatives rejetées**
- **Stocker `Vector3` en `float`** : gain mémoire marginal, perte de précision critique sur des scènes avec des grandes échelles (e.g. coordonnées astronomiques de bonus exotiques). La plupart des raytracers de référence (PBRT, Mitsuba) utilisent double pour la géométrie.
- **Templater `Vector<T, N>` ou dériver de `std::array`** : sur-ingénierie pour deux types (Vector2, Vector3). Les signatures explicites sont plus lisibles, le compilateur optimise de toute façon.
- **Ajouter `operator+=`, `-=`, `*=`, `/=`** : violent l'immuabilité imposée par les règles du projet. Les renderers alloueront des `const Vector3` intermédiaires ; la RVO gère la perf.
- **Ajouter un `epsilon()` par défaut dans `operator==`** : risqué. Le `==` défaultée est strict (bit-exact) → prédictible. Les comparaisons approximées se font via `approx(a, b, eps)` explicites dans la logique métier.
- **`Color` en 4 canaux (RGBA)** : l'alpha n'est pas au programme du sujet, et le pipeline Epitech est opaque. Ajouter A serait une dette non justifiée.
- **`Color::clamped()` / `Color::toSRGB8()`** : hors scope T0.4. Ces conversions vivront dans `PPMWriter` (T0.12) où leur contexte est clair (formats de pixel, endianness).
- **Asserts manuels dans le code de prod** (`math_sanity_check()`-style) : prototypé dans une version intermédiaire puis **retiré** — pas de tests ad-hoc dans le source. La validation automatique passera par GoogleTest + CTest dans un ticket dédié.
- **Lever une exception dans les hot paths** : catégoriquement non. `normalized()` lève car appelé aux frontières (construction de la camera, parsing) ; les boucles de tuiles/rayons doivent éviter `normalized()` sur un vecteur potentiellement nul ou le gérer manuellement.

**Impact**
- T0.5 (`Matrix4`, `Transform`) peut désormais composer des opérations sur Vector3 directement.
- T0.6 (`Ray`, `HitRecord`, `ShadingContext`) réutilise Vector2/Vector3/Color sans adaptation.
- T0.12 (`PPMWriter`) aura le type `Color` en linéaire prêt à être converti en sRGB + clampé → octets.
- **Dette technique** : pas de validation automatique pour l'instant. Le ticket dédié GoogleTest + CTest (à planifier) couvrira les types math + core.

**Non couvert volontairement**
- `Matrix4` / `Transform` → T0.5 (ticket séparé).
- `AABB`, `Ray`, `HitRecord` → T0.6.
- Clamp / conversion sRGB / PPM → T0.12.
- Tests unitaires formalisés (GoogleTest + CTest) → sera introduit quand la surface math + core le justifie.
- Quaternions / rotations Euler → hors scope sujet, éventuellement en bonus Phase 4.

**Validation**
```bash
make re                               # rebuild Release, 0 warning
./raytracer --help; echo $?           # exit 0
./raytracer; echo $?                  # exit 84 (CliError)
./raytracer nope.cfg; echo $?         # exit 84 (IOError)
ldd ./raytracer | grep raytracer_     # vide → aucun plugin linké
```
Tous les checks passent. Les types math compilent proprement sous `-Wall -Wextra -Werror -pedantic`, `Vector3::normalized()` lève bien `rt::MathError` sur vecteur nul (contrat vérifiable manuellement en attendant le ticket tests). Aucune validation automatique embarquée dans le code — c'est un choix explicite (voir patch 2026-04-22).

---

### v0.0.4 — 2026-04-21 — Hiérarchie d'exceptions du domaine (T0.3.1)

**Type** : refactor
**Ticket** : T0.3.1
**Phase** : 0 (Fondations)

**Résumé**
Introduction d'une hiérarchie d'exceptions personnalisées (`rt::RaytracerException` et ses sous-classes) et refactor de `main.cpp` pour que toute erreur du domaine remonte à un unique `try/catch` dans `main()`. Plus de `if (error) return 84;` disséminés : chaque module du projet à venir lancera une exception typée, `main()` la traduira en exit 84 + message sur `stderr`.

**Contexte / motivation**
À la fin de T0.3, la gestion d'erreur CLI fonctionnait mais reposait sur un helper `fail()` qui écrivait sur `stderr` puis retournait 84. Ce schéma ne passe pas à l'échelle : dès T0.4+ (math), T0.9 (plugin loader) et T0.10 (parser), chaque module aurait dû propager son erreur manuellement (code de retour, `std::optional`, `std::expected`). En C++ idiomatique, ce rôle revient aux exceptions ; elles offrent en plus :
- **catégorisation claire** : un `PluginAbiError` ≠ un `SceneParseError` ≠ une `CliError` ; `main()` (ou un futur observateur) peut réagir différemment sans sniffer le message.
- **contexte riche** : chaque exception porte son propre `what()` (chemin du fichier, ligne libconfig, nom du plugin, ABI attendue vs trouvée…).
- **polymorphisme** : un seul `catch (const rt::RaytracerException&)` attrape tout le domaine métier, et des `catch` plus spécifiques peuvent être ajoutés sans toucher aux lanceurs.
- **sécurité** : un `std::exception` inattendu ne remonterait plus à `std::terminate` — un filet de dernier recours exit 84 proprement.

La décision a été actée en passation : "T0.3bis / v0.0.4 — on pose la plomberie d'erreurs maintenant, avant T0.4, pour que les classes math (ex: `Vector3::normalize()` sur vecteur nul) puissent jeter `MathError` proprement dès leur écriture."

**Détails techniques**
- **Nouveau fichier** : `src/core/Exceptions.hpp`.
  - Racine : `rt::RaytracerException : public std::runtime_error` — hérite de la stdlib pour bénéficier de `what()` + ergonomie des constructeurs (`using std::runtime_error::runtime_error;`).
  - Catégories (toutes réutilisant les constructeurs parents via `using`) :
    - `CliError` — argv invalide (manque argument, trop d'arguments, flag inconnu).
    - `IOError` — fichier inexistant / pas régulier / illisible.
    - `SceneParseError` (base) avec deux raffinements :
      - `InvalidConfigError` — syntaxe / type / valeur hors bornes libconfig.
      - `UnknownTypeError` — scène référence un nom non fourni par un plugin chargé.
    - `PluginError` (base) avec deux raffinements :
      - `PluginLoadError` — `dlopen`/`dlsym` échoue, `.so` absent.
      - `PluginAbiError` — `RAYTRACER_ABI_VERSION` incompatible.
    - `MathError` — `Vector3::normalize()` vecteur nul, matrice singulière, composition invalide.
  - Header-only, pas de `.cpp` (tout est `inline` via héritage et constructeurs délégués).
  - Namespace `rt::` choisi pour toutes les classes publiques du projet (court, dédié, pas de collision avec `std::`).
- **`src/main.cpp` refactoré** :
  - Ancien helper `fail()` supprimé.
  - Nouvelle fonction `validateScenePath()` : lève `rt::IOError` plutôt que retourner un code.
  - Nouvelle fonction `parseArguments()` : lève `rt::CliError` pour toute erreur d'argv ; renvoie un `std::filesystem::path` vide comme sentinel "`--help` déjà géré, exit 0".
  - `runScene()` devient `void` (ne peut plus échouer par code de retour).
  - `main()` : unique `try { … } catch` avec 3 branches polymorphes :
    - `rt::CliError` → message sur `stderr` **+ usage** + exit 84 (un CLI invalide mérite l'usage).
    - `rt::RaytracerException` → message sur `stderr` + exit 84 (pas d'usage pour une erreur IO/parse/plugin/math).
    - `std::exception` → filet de dernier recours : "unexpected error: …" + exit 84, jamais `std::terminate`.
  - `#include "core/Exceptions.hpp"` ajouté (chemin relatif au `target_include_directories(PRIVATE src/)` existant).

**Alternatives rejetées**
- **Rester en codes de retour (`int fail(msg)`)** : ne passe pas à l'échelle sur N modules ; impose de re-propager manuellement à chaque niveau. Anti-idiomatique en C++ moderne où RAII + exceptions = modèle standard aux frontières.
- **`std::expected<T, ErrorCategory>` (C++23)** : toolchain sujet garantit C++20 uniquement. Aussi : `expected` est mieux adapté aux retours métier (intersect, shade) qu'aux erreurs catastrophiques de démarrage. On en gardera l'idée pour les retours de hot-path plus tard.
- **Une seule classe `RaytracerException` sans sous-classes** : fonctionnel mais on perd la catégorisation. `e.what()` deviendrait la seule info typée, ce qui obligerait à parser des chaînes pour réagir.
- **Faire hériter directement de `std::exception`** : priverait des constructeurs `(const std::string&)` et `(const char*)` de `std::runtime_error`. Zéro bénéfice, verbosité accrue.
- **Exceptions dans les hot paths (`IPrimitive::intersect`, `IMaterial::shade`)** : catégoriquement non. Les exceptions C++ ont un coût non trivial sur le chemin levé ; sur une boucle de millions de rayons, c'est inacceptable. Les hot paths restent en valeur de retour (`bool`, `std::optional<HitRecord>`).
- **Laisser les exceptions traverser l'ABI plugin (`.so`)** : interdit — une exception ne peut pas traverser la frontière `extern "C"` en toute sécurité (linker différent, ABI divergente). Les plugins devront capturer en interne et retourner un code d'erreur que le core reconvertit en exception. Ce point sera câblé en T0.9 / T0.15.

**Impact**
- Dès T0.4 (math), les classes `Vector3`, `Matrix4`, `Transform` peuvent lever `rt::MathError` naturellement.
- T0.9 (PluginLoader) aura `PluginLoadError` et `PluginAbiError` prêts à l'emploi.
- T0.10 (SceneParser) aura `InvalidConfigError` et `UnknownTypeError` prêts à l'emploi.
- Le code de `main()` devient stable : toute nouvelle source d'erreur s'intègre sans modifier `main()` (ouvert à l'extension, fermé à la modification).
- **Filet de sécurité** : un `std::bad_alloc`, une `std::filesystem::filesystem_error` non convertie, etc. ne provoquent plus `std::terminate` — exit 84 avec diagnostic.
- **Dette technique introduite** : les plugins devront implémenter une conversion code d'erreur → exception au bord core/plugin (documenté pour T0.9). Aucun autre emprunt.

**Non couvert volontairement**
- Enrichissement des exceptions avec des **champs typés** (`.file_name()`, `.line_number()`, `.plugin_name()`) : on l'ajoutera au cas par cas quand le parser/loader les produiront (T0.9, T0.10). Aujourd'hui, le message textuel suffit.
- Tests unitaires des exceptions : le framework de tests (GoogleTest + CTest) n'est pas encore en place ; il sera introduit en même temps que les classes math (T0.4) où le volume de code justifie une cible `test`.

**Validation**
```bash
make re                               # rebuild clean, 0 warning
./raytracer --help; echo $?           # usage sur stdout, exit 0
./raytracer; echo $?                  # CliError: usage sur stderr, exit 84
./raytracer nope.cfg; echo $?         # IOError: pas d'usage, exit 84
./raytracer src; echo $?              # IOError (dir): pas d'usage, exit 84
./raytracer a b c; echo $?            # CliError: usage sur stderr, exit 84
./raytracer /tmp/valid.cfg; echo $?   # accusé de réception, exit 0
```
Tous les checks passent. Comportement CLI strictement identique à T0.3 (zéro régression observable de l'extérieur), mais la plomberie d'erreurs est désormais prête pour le reste du projet.

---

### v0.0.3 — 2026-04-21 — CLI conforme Epitech (T0.3)

**Type** : feature
**Ticket** : T0.3
**Phase** : 0 (Fondations)

**Résumé**
Implémentation de l'entrée CLI conforme au sujet : gestion d'`argv`, flag `--help` (alias `-h`), messages d'erreur sur `std::cerr`, codes de sortie `0` / `84`, validation du chemin de scène (existence + fichier régulier).

**Contexte / motivation**
Le sujet Epitech est strict sur :
- l'usage exact affiché par `--help`,
- les erreurs sur `stderr` et **pas** sur `stdout`,
- le code de retour `84` sur **toute** erreur (argument invalide, fichier introuvable, etc.).
La moulinette Epitech échoue instantanément si l'un de ces points est faux. On verrouille ça dès la Phase 0 avant tout code de rendu.

**Détails techniques**
- Fichier modifié : `src/main.cpp` (remplace le placeholder de T0.2).
- Constantes d'exit codes en `constexpr` (`kExitSuccess = 0`, `kExitFailure = 84`).
- Usage centralisé dans une `std::string_view` constante (`kUsage`) pour éviter la divergence stdout/stderr.
- Helper `fail(message)` : écrit sur `std::cerr` avec préfixe `"raytracer: "`, affiche l'usage, retourne `kExitFailure`.
- Helper `isHelpFlag(arg)` : accepte `--help` et `-h` (alias courant, non exigé mais zéro coût).
- Validation du fichier de scène :
  - `std::filesystem::exists` avec `std::error_code` pour éviter les exceptions.
  - `std::filesystem::is_regular_file` pour rejeter les dossiers et les devices.
- `runScene()` : stub qui accuse réception du fichier et informe que le pipeline de rendu n'est pas encore câblé (sera remplacé en T0.10+).
- `#include <string_view>` pour comparer les arguments sans allocation.

**Alternatives rejetées**
- **Une mini-classe `Cli`** : over-engineering pour 40 lignes. On introduira une classe si le CLI grossit (bonus multi-flags, options de rendu, etc.).
- **`fprintf(stderr, …)`** : `std::cerr` est plus idiomatique en C++ moderne et cohérent avec le reste du projet.
- **Ne pas afficher l'usage après l'erreur** : on le fait parce que c'est plus user-friendly et c'est conforme à l'esprit des CLI Epitech.
- **Bail out sans message propre** : interdit par le sujet (messages d'erreur obligatoires).

**Impact**
- La conformité Epitech CLI est verrouillée : tout futur changement qui casserait le code 84 ou qui polluerait `stdout` sera détectable immédiatement via les tests manuels documentés ici.
- `runScene()` est le **point de branchement** pour T0.10 (parser) et T0.14 (renderer) : il sera remplacé par la chaîne complète sans toucher à la gestion d'argv.

**Non couvert volontairement**
- Parsing du fichier `.cfg` → T0.10.
- Flags additionnels (`--output`, `--threads`, `--verbose`) → éventuellement en Phase 4 quand les bonus l'exigeront.

**Validation**
```bash
./raytracer --help                 # USAGE sur stdout, exit 0
./raytracer -h                     # idem (alias)
./raytracer                        # "missing argument" sur stderr, exit 84
./raytracer nope.cfg               # "scene file not found" sur stderr, exit 84
./raytracer src                    # "not a regular file" sur stderr, exit 84
./raytracer a b c                  # "too many arguments" sur stderr, exit 84
./raytracer /tmp/valid_scene.cfg   # accusé de réception, exit 0
```
Tous les checks passent. `stdout` reste strictement vide en cas d'erreur.

---

### v0.0.2 — 2026-04-21 — Squelette CMake + Makefile wrapper (T0.2)

**Type** : infra
**Ticket** : T0.2
**Phase** : 0 (Fondations)

**Résumé**
Mise en place du build system : `CMakeLists.txt` racine, sous-projets `src/` et `plugins/`, `Makefile` wrapper. Le binaire `raytracer` est désormais produit à la racine, vide mais fonctionnel (retourne 0).

**Contexte / motivation**
Avant d'écrire le moindre code moteur, on a besoin d'un build system déterministe qui respecte les contraintes Epitech :
- binaire `./raytracer` à la racine (pas dans `build/`),
- plugins `.so` dans `./plugins/`,
- règles `all`, `re`, `clean`, `fclean` accessibles via un simple `make`,
- flags `-Wall -Wextra -Werror -pedantic` par défaut,
- séparation Debug (sanitizers) / Release (`-O3`, critique pour un raytracer).

**Détails techniques**
- **`CMakeLists.txt` racine** : projet `raytracer` en C++20, `CMAKE_POSITION_INDEPENDENT_CODE ON` (requis pour les plugins `.so`), sorties :
  - `CMAKE_RUNTIME_OUTPUT_DIRECTORY` = racine du repo → produit `./raytracer`.
  - `CMAKE_LIBRARY_OUTPUT_DIRECTORY` = `./plugins/` → produit `./plugins/raytracer_*.so`.
  - `CMAKE_ARCHIVE_OUTPUT_DIRECTORY` = `build/archive` pour les `.a` éventuelles (hors du repo tracké).
- Flags par configuration (générateurs d'expressions `$<CONFIG:...>`) :
  - **Debug** : `-g -O0 -fsanitize=address,undefined` (compile + link).
  - **Release** : `-O3 -DNDEBUG` (par défaut si aucun build type spécifié).
- **`src/CMakeLists.txt`** : cible `raytracer` (executable) avec `main.cpp`. Aucun linkage à des plugins. `libdl` et `libconfig++` seront ajoutés quand les modules concernés arriveront (T0.9, T0.10).
- **`plugins/CMakeLists.txt`** : vide pour l'instant, documenté. Chaque plugin sera ajouté via `add_subdirectory(<nom>)` au fur et à mesure (T0.15 pour la sphère factice, puis Phase 1).
- **`src/main.cpp`** : `int main(int, char*[]) { return 0; }`. Le CLI complet (`--help`, exit 84, messages sur `stderr`) est implémenté en T0.3 pour rester fidèle au découpage en tickets atomiques.
- **`Makefile` wrapper** :
  - `all` → configure + build (si `CMakeCache.txt` absent, configure d'abord).
  - `re` → `fclean` + `all`.
  - `clean` → `cmake --build build --target clean`.
  - `fclean` → supprime `build/`, `raytracer`, `plugins/*.so|.dylib|.dll`.
  - Bonus : `debug`, `release`, `help`, variables `BUILD_DIR`, `BUILD_TYPE`, `JOBS` surchargeables.

**Alternatives rejetées**
- **Makefile pur sans CMake** : écrit et maintenu à la main, lourd dès qu'on aura N plugins `.so`. CMake le fait nativement.
- **Mettre les plugins dans `build/plugins/`** : non conforme au sujet, qui exige `./plugins/` à la racine.
- **Linker le binaire à un plugin** : casse le modèle plugin (et la règle `ldd ./raytracer` ne doit lister aucun `raytracer_*.so`).

**Impact**
- On peut à présent ajouter incrémentalement des modules (core libs, interfaces, plugins) sans retoucher le build system de base.
- Contrainte de conformité verrouillée : toute régression (binaire hors racine, plugin linké en dur) serait visible immédiatement via `ldd`.
- `make debug` active ASan/UBSan dès la Phase 0 — tout bug mémoire dans le math ou le loader sera détecté tôt.

**Non couvert volontairement**
- `--help`, exit 84, erreurs sur `stderr` → T0.3.
- `libdl`, `libconfig++` → T0.9 / T0.10.
- Cibles plugin (`raytracer_sphere`, etc.) → T0.15 et Phase 1.

**Validation**
```bash
make                       # build OK
./raytracer; echo $?       # 0
./raytracer                # aucun output (stdout et stderr vides)
ldd ./raytracer            # aucun raytracer_*.so linké
make fclean                # raytracer et build/ supprimés
make re                    # reconstruit ./raytracer
make debug                 # build avec sanitizers
make help                  # affiche les cibles disponibles
```
Tous les checks passent.

---

### v0.0.1 — 2026-04-21 — Initialisation du dépôt (T0.1)

**Type** : infra
**Ticket** : T0.1
**Phase** : 0 (Fondations)

**Résumé**
Premier squelette du dépôt : structure de dossiers conforme à la cible architecturale, `.gitignore` strict, `README.md` squelette, `PLAN.md` et `DOCUMENTATION.md` créés.

**Contexte / motivation**
Le dépôt était vide (seul `PROJECT.md`). Avant d'écrire la moindre ligne de code, on pose la structure pour que tous les futurs ajouts tombent au bon endroit et pour éviter tout commit accidentel de binaires / artefacts de build.

**Détails techniques**
- Dossiers créés (vides avec `.gitkeep`) :
  - `src/core/`, `src/math/`, `src/interfaces/`, `src/primitives/`, `src/lights/`, `src/materials/`, `src/parser/`, `src/plugins/`, `src/output/`
  - `plugins/` (sortie des `.so`)
  - `scenes/` (fichiers `.cfg` de démo)
  - `screenshots/` (rendus de référence `.ppm`)
  - `tests/` (tests unitaires à venir)
  - `bonus/` (fichiers bonus séparables)
- Fichiers ajoutés :
  - `.gitignore` : ignore `build/`, artefacts C/C++ (`*.o`, `*.so`, `*.a`, `*.d`), le binaire `raytracer` à la racine, les `.ppm` générés (sauf `screenshots/*.ppm` conservés comme références), caches IDE (`.vscode/`, `.idea/`, `.cache/`, `.clangd/`, `compile_commands.json`), swap (`*.swp`, `*.swo`), macOS (`.DS_Store`).
  - `README.md` : squelette utilisateur (build, run, scène, structure).
  - `plan.md` : feuille de route détaillée (ce qui doit être fait, dans quel ordre, avec critères d'acceptation).
  - `DOCUMENTATION.md` : ce fichier.

**Impact**
- Tous les futurs ajouts ont un emplacement prédéfini.
- Risque zéro de commit de binaire ou de dossier `build/`.
- Le triptyque de docs est en place : `PROJECT.md` (hub) / `PLAN.md` (roadmap) / `DOCUMENTATION.md` (journal).

**Validation**
- `git status` propre après le premier commit.
- `find . -name .gitkeep` liste tous les dossiers créés.
- Aucun fichier binaire ou généré tracké.

---

<!-- Ajouter les futures entrées ci-dessus cette ligne, en ordre antéchronologique. -->

---

## Architecture & choix techniques

Cette section consolide les **décisions architecturales** (WHY plus que WHAT). Chaque décision doit pointer vers la version où elle a été introduite.

### Design patterns retenus (v0.0.1)

- **Factory** — instanciation de primitives / lumières / matériaux depuis le fichier de scène, via les plugins. Chaque plugin exporte un symbole C `raytracer_plugin_entry` pointant vers un `PluginDescriptor` qui contient la factory.
- **Composite** — scene graph `INode` (`LeafNode` + `GroupNode`), même interface pour primitives isolées et groupes. Prépare scene graph et scene import sans refactor.
- **Builder** — `SceneBuilder` qui accumule caméra, lumières, matériaux, nœuds, puis construit une `Scene` immuable.
- **Strategy** — `IMaterial::shade(ShadingContext)` : chaque matériau (FlatColor, Phong, Reflective…) est une stratégie interchangeable.

En réserve bonus : Observer (hot-reload), Decorator (empilement d'effets sur matériau), State (modes renderer).

### Choix techniques clés (v0.0.1)

- **C++20** : `std::filesystem`, `concepts`, `<span>`, `<numbers>`.
- **CMake** + Makefile wrapper : conforme, naturel pour plugins `.so`.
- **`Matrix4` dès le départ** : évite le refactor "transformation matrix".
- **ABI plugins versionnée** (`RAYTRACER_ABI_VERSION`) avec POD C stable.
- **Scène immuable post-construction** : multithreading sans refactor.
- **`Color` en float linéaire**, conversion sRGB uniquement à l'écriture PPM.
- **Intersection en espace objet** (primitives canoniques).
- **Matériaux référencés par nom, partagés via `shared_ptr`**.

### Gestion d'erreurs (v0.0.4)

- **Exceptions aux frontières uniquement** : CLI, parser, plugin loader, IO. Jamais dans les hot paths (`intersect`, `shade`, boucle de tuiles) — ces derniers restent en valeur de retour (`bool`, `std::optional`).
- **Racine unique** : toutes les erreurs du domaine héritent de `rt::RaytracerException : public std::runtime_error` (voir `src/core/Exceptions.hpp`).
- **Catégories** : `CliError`, `IOError`, `SceneParseError` (→ `InvalidConfigError`, `UnknownTypeError`), `PluginError` (→ `PluginLoadError`, `PluginAbiError`), `MathError`.
- **Unique `try/catch` dans `main()`** : traduit chaque catégorie en exit 84 + message `stderr`. Un `catch (const std::exception&)` de dernier recours évite `std::terminate` sur toute erreur inattendue.
- **Frontière plugin** : les exceptions **ne traversent pas** `extern "C"` ; chaque plugin capture en interne et renvoie un code d'erreur que le core reconvertit en `PluginError`. Câblage à faire en T0.9 / T0.15.

### Math : conventions (v0.0.5)

- **Précision** : `Vector2` / `Vector3` stockent en `double` (précision géométrique sur scènes à grande échelle). `Color` stocke en `float` (RGB linéaire, HDR autorisé > 1.0).
- **Espace colorimétrique** : toute la math colorimétrique (éclairage, réflexion, atténuation) se fait en **linéaire**. La conversion sRGB + clamp est réservée à `PPMWriter` (T0.12).
- **Immuabilité** : tous les opérateurs renvoient de nouveaux objets. Aucun compound assignment (`+=`, `-=`, `*=`, `/=`) exposé — les règles du projet interdisent la mutation.
- **Base vectorielle** : main droite. `ex.cross(ey) == ez` (vérifié par `math_sanity_check()`).
- **Exceptions math aux frontières** : `normalized()` lève `rt::MathError` sur vecteur nul. Les hot paths (`intersect`, `shade`, boucle de tuiles) doivent vérifier `lengthSquared() > 0` manuellement plutôt que d'attraper une exception.
- **Validation runtime** : aucune validation automatique embarquée dans le code. Les tests manuels introduits en v0.0.5 (`src/math/sanity.hpp`) ont été retirés : la validation formelle passera par GoogleTest + CTest dans un ticket dédié, pas par des asserts ad-hoc dans le source.
- **Matrices** : `Matrix4` row-major, stockée flat en `std::array<double, 16>`, default = identité. Accès par `M(row, col)`. Multiplication, transposée, inversion (Gauss-Jordan + pivot partiel, lève `MathError` sur singulière).
- **Transform** : tient matrice + **inverse pré-calculé**. Factories analytiques (`translation`/`scaling`/`rotation*`) construisent l'inverse exactement (négation, réciproque, transposée) ; le constructeur générique `Transform(Matrix4)` appelle `Matrix4::inverse()` numérique. `applyToNormal` utilise `(M⁻¹)ᵀ` pour gérer correctement les scales non uniformes. `Transform::inverse()` est gratuit (swap des deux matrices stockées).

### Types POD d'échange core/plugins (v0.0.7)

- **Emplacement** : `src/interfaces/` (surface d'API publique partagée avec les plugins, par opposition à `src/core/` qui est le runtime interne).
- **`Ray`** : `{origin, direction, tMin, tMax}`, `direction` unitaire par convention (non vérifié par le type). `tMin` par défaut = `kRayEpsilon = 1e-4` pour éviter la self-intersection des rayons réfléchis/réfractés. Helper `at(t) = origin + t*direction`.
- **`HitRecord`** : `{t, point, normal, uv, material*, node*, frontFace}`. POD copiable librement. Les pointeurs `IMaterial*` et `INode*` sont **non-propriétaires** — la Scene owne leurs cibles via `shared_ptr`. `frontFace` est rempli par la primitive en comparant `dot(normal, ray.dir)`.
- **`ShadingContext`** : `{const Scene&, const HitRecord&, const Ray&, int depth}`. Construit sur la pile au site d'appel de `shade()`, jamais stocké. `depth` borne la récursion (réflexion/réfraction).
- **`AABB`** : `{Vector3 min, Vector3 max}`. **Empty sentinel** : default-constructed = `min = (+∞,+∞,+∞)`, `max = (−∞,−∞,−∞)` → `expanded()` et `merged()` sont corrects par construction, même sur la première itération d'une boucle d'expansion.
- **Règles de compilation** : headers self-contained — `IMaterial`, `INode`, `Scene` en **forward declaration** car définis plus tard (T0.7, T0.11).
- **Pas d'exceptions dans ces types** : ils sont utilisés dans les hot paths (intersect, shade). La gestion d'erreurs reste aux frontières (T0.3.1).

---

## Features documentées

Chaque feature (primitive, lumière, matériau, bonus) aura sa propre sous-section, remplie au fur et à mesure.

### Primitives

_Aucune primitive implémentée pour l'instant._

### Lumières

_Aucune lumière implémentée pour l'instant._

### Matériaux

_Aucun matériau implémenté pour l'instant._

### Transformations

_Aucune transformation exposée à la config pour l'instant._

### Bonus

_Aucun bonus implémenté pour l'instant._

---

## Patches / bug fixes

### 2026-04-22 — Suppression de `math_sanity_check()` et de `src/math/sanity.hpp`

**Fichiers** : `src/math/sanity.hpp` (supprimé), `src/main.cpp` (include + appel retirés)
**Type** : refactor / DX

**Problème**
Le ticket T0.4 avait introduit `rt::math_sanity_check()` dans `src/math/sanity.hpp` pour valider à la volée Vector2, Vector3, Color via des `assert()` appelés depuis `main()` en Debug. Ces checks « à la main » pollueraient le code de prod et ne remplacent pas un vrai framework de tests.

**Fix**
- Suppression de `src/math/sanity.hpp`.
- `#include "math/sanity.hpp"` et appel `rt::math_sanity_check()` retirés de `src/main.cpp`.
- Critère d'acceptation de T0.4 dans `PLAN.md` reformulé : compilation `-Wall -Wextra -Werror -pedantic` propre + contraintes structurelles (linéaire, `MathError` sur vecteur nul, `constexpr` où possible). Validation formelle reportée à l'introduction d'un vrai framework.

**Politique retenue**
Pas de tests ad-hoc dans le code de prod. Quand une validation automatique sera nécessaire, elle passera par GoogleTest + CTest dans un ticket dédié (dossier `tests/`, cible CMake séparée, lancée via `ctest`).

**Résultat**
- `src/math/` ne contient plus que `Vector2.hpp`, `Vector3.hpp`, `Color.hpp` (+ `.gitkeep`).
- `main.cpp` revient à sa forme post-T0.3.1 : un `try/catch` qui n'exécute rien avant `parseArguments()`.
- Build Release : OK, 0 warning. Tous les exits / chemins CLI inchangés.

---

### 2026-04-21 — Suppression du bruit `gmake[N]: Entering/Leaving directory`

**Fichier** : `Makefile`
**Type** : chore / DX

**Problème**
Le wrapper délègue à CMake, qui utilise le générateur "Unix Makefiles" produisant des sous-Makefiles imbriqués (`build/Makefile` → `build/CMakeFiles/Makefile2` → `build/src/CMakeFiles/raytracer.dir/build.make`). À chaque niveau, GNU make loggait `gmake[N]: Entering directory '…'` / `Leaving directory '…'`, ce qui polluait la sortie à chaque `make` / `make re`.

**Fix**
- Ajout de `MAKEFLAGS += --no-print-directory` dans le Makefile racine — propagé à toutes les invocations récursives.
- Passage explicite de `-- --no-print-directory` derrière `cmake --build` pour les versions où `MAKEFLAGS` ne passe pas au travers de CMake.

**Résultat**
Sortie de `make re` réduite aux étapes CMake + compilation ; plus aucun message `gmake[N]:`.

---

## Known issues

_Aucun problème connu pour l'instant._
