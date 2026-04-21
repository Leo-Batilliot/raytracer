# PROJECT.md — Projet Raytracer (Epitech G-OOP-400)

Ce document est la **source de vérité** pour le projet Raytracer. Il doit être consulté à chaque session de travail et mis à jour quand des décisions architecturales évoluent. Toutes les exigences du sujet officiel y sont consignées, ainsi que la philosophie de développement que nous adoptons.

---

## 🎯 Philosophie du projet

Notre objectif **n'est pas** simplement de rendre un projet qui "marche". Notre objectif est de produire un moteur de raytracing **propre, professionnel, extensible et maintenable**, capable de servir de base à des extensions illimitées une fois les fonctionnalités obligatoires validées.

Concrètement, cela signifie :

- **Priorité absolue à l'architecture** avant d'écrire la moindre ligne de logique de rendu. Un raytracer mal architecturé devient impossible à étendre après quelques semaines.
- **Chaque ligne de code doit être justifiable.** Si tu ne peux pas expliquer pourquoi une classe existe ou pourquoi une fonction est dans tel fichier, c'est que l'architecture est à retravailler.
- **Tests et scènes de démo continues.** Chaque fonctionnalité ajoutée doit venir avec une scène de démonstration dans `./scenes/` et un rendu de référence dans `./screenshots/`.
- **Les bonus sont le véritable cœur du projet.** Le "must" et le "should" ne sont qu'un point de départ. Notre but est d'empiler un maximum de bonus et de fonctionnalités créatives par-dessus une base solide.
- **Pas de code mort, pas de copier-coller, pas de raccourcis.** Si une fonctionnalité est implémentée deux fois, c'est qu'il manque une abstraction.

**Règle d'or** : avant d'ajouter une feature, se demander "est-ce que l'architecture actuelle peut l'accueillir proprement, ou est-ce que je m'apprête à la plaquer avec du scotch ?". Si c'est la deuxième option, on refactor d'abord.

---

## 📋 Préliminaires (obligatoires)

- **Nom du binaire** : `raytracer` (à la racine du dépôt)
- **Langage** : C++ (on part sur C++ — cette décision est posée)
- **Compilation** : via `Makefile` avec les règles `all`, `re`, `clean`, `fclean` — OU via `CMake`
- **Code de sortie** :
  - `0` si tout va bien
  - `84` en cas d'erreur
- **Messages d'erreur** : obligatoirement sur la **sortie d'erreur** (`std::cerr`), jamais sur `stdout`
- **Usage attendu** :
  ```
  $ ./raytracer --help
  USAGE: ./raytracer <SCENE_FILE>
    SCENE_FILE: scene configuration
  ```
- **Livraison** :
  - Tous les fichiers sources sauf les fichiers inutiles (binaires, fichiers temporaires, `.o`, etc.)
  - Les fichiers de bonus (y compris un éventuel Makefile spécifique) dans un dossier `bonus/`

---

## ✅ Fonctionnalités MUST (obligatoires — doivent être 100% fonctionnelles)

### Primitives
- [ ] **Sphere**
- [ ] **Plane** (plan infini)

### Transformations
- [ ] **Translation**

### Lumières
- [ ] **Directional light** (lumière directionnelle)
- [ ] **Ambient light** (lumière ambiante)

### Matériaux
- [ ] **Flat color** (couleur unie)

### Configuration de scène
- [ ] Ajouter des primitives à la scène
- [ ] Configurer l'éclairage
- [ ] Configurer la caméra

### Interface
- [ ] **Pas de GUI**, sortie dans un **fichier PPM**

> ⚠️ Cette liste ne couvre **que les features**. Les exigences d'architecture (interfaces, design patterns, etc.) sont décrites plus bas et sont tout aussi obligatoires.

---

## 🟡 Fonctionnalités SHOULD (à faire après que le MUST soit parfaitement fonctionnel)

### Primitives
- [ ] **Cylinder**
- [ ] **Cone**

### Transformation
- [ ] **Rotation**

### Lumière
- [ ] **Drop shadows** (ombres portées)

---

## 🎁 Fonctionnalités COULD (bonus — points entre parenthèses)

Ces fonctionnalités sont celles que nous allons **empiler autant que possible** une fois le MUST et le SHOULD validés. Le nombre entre parenthèses indique la valorisation officielle dans le barème.

### Primitives
- [ ] Limited cylinder *(0.5)*
- [ ] Limited cone *(0.5)*
- [ ] Torus *(1)*
- [ ] Tanglecube *(1)*
- [ ] Triangles *(1)*
- [ ] `.OBJ` file loader *(1)*
- [ ] Fractals *(2)*
- [ ] Möbius strip *(2)*

### Transformations
- [ ] Scale *(0.5)*
- [ ] Shear *(0.5)*
- [ ] **Transformation matrix** *(2)* — à prioriser, refactor propre de toutes les transformations
- [ ] **Scene graph** *(2)* — idem, unifie la hiérarchie d'objets

### Lumière
- [ ] Multiple directional lights *(0.5)*
- [ ] Multiple point lights *(0.5)*
- [ ] Colored light *(0.5)*
- [ ] **Phong reflection model** *(2)* — gros gain visuel pour un effort raisonnable
- [ ] Ambient occlusion *(2)*

### Matériaux
- [ ] Transparency *(0.5)*
- [ ] Refraction *(1)*
- [ ] Reflection *(0.5)*
- [ ] Texturing from file *(1)*
- [ ] Texturing from procedural generation of chessboard *(1)*
- [ ] Texturing from procedural generation of Perlin noise *(1)*
- [ ] Normal mapping *(2)*

### Configuration de scène
- [ ] Importer une scène dans une scène *(2)*
- [ ] Antialiasing par supersampling *(0.5)*
- [ ] Antialiasing par supersampling adaptatif *(1)*

### Optimisations
- [ ] Space partitioning (BVH, k-d tree, octree…) *(2)*
- [ ] **Multithreading** *(1)* — à faire tôt, transforme l'expérience de développement
- [ ] Clustering (rendu réparti sur plusieurs machines via réseau) *(3)*

### Interface
- [ ] Afficher l'image pendant et après la génération *(1)* — SFML autorisé
- [ ] Quitter pendant ou après la génération *(0.5)*
- [ ] Scene preview avec un renderer rapide *(2)*
- [ ] Rechargement automatique de la scène au changement du fichier *(1)*

> 💡 **Bonus illimités** : le sujet précise explicitement qu'un nombre infini de bonus est possible. On peut ajouter ce qu'on veut tant que MUST + SHOULD sont complets. Idées hors-liste à envisager : motion blur, depth of field, path tracing / global illumination, volumetric rendering, HDR, tone mapping, bump mapping, caustics, subsurface scattering, environnement maps (skybox)…

---

## 🏛️ Architecture (EXIGENCES OBLIGATOIRES)

L'architecture fait partie intégrante de la note. Ces points ne sont **pas** négociables.

### Interfaces (obligatoire)

En C++, **on doit utiliser des interfaces (classes abstraites pures)**, au minimum pour :
- Les **primitives** (une interface `IPrimitive` ou équivalent)
- Les **lumières** (une interface `ILight` ou équivalent)

Concrètement, chaque primitive (Sphere, Plane, Cylinder, etc.) hérite d'une même interface définissant au minimum :
- Une méthode d'intersection rayon/objet
- Une méthode pour obtenir la normale en un point
- Un accès à son matériau / sa transformation

### Plugins (non-obligatoire mais fortement recommandé)

Le moteur doit être extensible. Le sujet propose (et valorise fortement) un **système de plugins** via bibliothèques dynamiques (`.so`) chargées au runtime avec `libdl`.

- L'exécutable `raytracer` **ne doit PAS être linké** à ces plugins
- Les plugins doivent être stockés dans `./plugins/` à la racine du dépôt
- Une bonne **encapsulation de `libdl`** est requise (pas de `dlopen`/`dlsym` qui traîne partout — on wrap proprement dans une classe `PluginLoader` ou équivalent)

Exemples de ce qui peut être mis en plugin :
- Primitives
- Lumières
- Scene loaders (différents formats de scène)
- Interface graphique
- Core renderers (rasterizer rapide vs raytracer complet vs path tracer)
- Effets optiques

**Décision de projet** : on part sur un système de plugins dès le début. C'est plus de travail initial mais ça dicte une architecture saine et ça ouvre beaucoup de bonus.

### Design patterns (OBLIGATOIRE : au moins 2)

On **doit** utiliser au minimum **2 design patterns** parmi :
- **Factory** — pour instancier des primitives/lumières depuis la config
- **Builder** — pour construire la scène étape par étape
- **Composite** — pour le scene graph (groupes d'objets traités comme un seul)
- **Decorator** — pour empiler des effets sur un matériau ou un objet
- **Observer** — pour le rechargement auto, le reporting de progrès
- **State** — pour les modes du renderer (preview / full render / paused)
- **Mediator** — pour coordonner les composants du moteur

Les choix seront **discutés lors de la soutenance** — donc ils doivent être justifiés.

**Recommandation de départ** :
1. **Factory** pour instancier primitives et lumières depuis le fichier de config (naturel, utile, bien visible)
2. **Composite** pour le scene graph (permet plus tard le bonus scene graph sans refactor)
3. **Observer** ou **Decorator** en troisième pattern selon les features qu'on empile

---

## 📦 Bibliothèques autorisées (C++)

**UNIQUEMENT** :
- La stdlib C (pour `libdl`) et C++
- `libconfig++` pour parser le fichier de scène
- `SFML` pour l'affichage (uniquement si on fait le bonus de display)

Toute autre dépendance doit être demandée à l'équipe pédagogique. **Pas de `boost`, pas de `glm`, pas de `nlohmann/json`, rien d'autre.**

> Conséquence : on écrit **nos propres** classes Vector3, Matrix4, Ray, Color, etc. C'est un choix du sujet, pas une contrainte à contourner. Ces classes doivent être particulièrement soignées puisqu'elles sont partout.

---

## 📄 Format du fichier de scène

Le sujet suggère `libconfig++`. On valide ce choix. La structure peut être adaptée — voici la structure de référence du sujet (à faire évoluer) :

```cfg
# Configuration of the camera
camera:
{
    resolution = { width = 1920; height = 1080; };
    position = { x = 0; y = -100; z = 20; };
    rotation = { x = 0; y = 0; z = 0; };
    fieldOfView = 72.0; # In degree
};

# Primitives in the scene
primitives:
{
    spheres = (
        { x = 60; y = 5; z = 40; r = 25; color = { r = 255; g = 64; b = 64; }; },
        { x = -40; y = 20; z = -10; r = 35; color = { r = 64; g = 255; b = 64; }; }
    );
    planes = (
        { axis = "Z"; position = -20; color = { r = 64; g = 64; b = 255; }; }
    );
};

# Light configuration
lights:
{
    ambient = 0.4;   # Multiplier of ambient light
    diffuse = 0.6;   # Multiplier of diffuse light
    point = (
        { x = 400; y = 100; z = 500; }
    );
    directional = ();
};
```

**À penser dès le début** :
- Comment on va ajouter les **matériaux** proprement (séparer la définition du matériau de l'objet qui l'utilise, via un nom de référence par exemple)
- Comment on va ajouter les **transformations** (translation, rotation, scale) sur chaque objet
- Comment on va gérer l'**import d'une scène dans une autre** (bonus à 2 points)

Le sujet insiste : "There may be better ways to structure your file, take some time to think about it." → on prend ce temps dès le départ.

---

## 🔨 Build

### Choix : CMake

On part sur **CMake** (plus propre que Makefile pour un projet C++ de cette taille, gère naturellement les plugins `.so`).

Résultat attendu :
```
$ mkdir ./build && cd ./build
$ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
$ cd ..
$ ls ./raytracer ./plugins/
./raytracer
./plugins/:
raytracer_cone.so
raytracer_cylinder.so
raytracer_plane.so
raytracer_sphere.so
```

Le `CMakeLists.txt` doit :
- Construire l'exécutable `raytracer` à la **racine** du dépôt
- Construire les plugins dans `./plugins/` à la racine
- Séparer chaque plugin dans sa propre cible CMake
- Utiliser `-Wall -Wextra -Werror -pedantic` au minimum en mode Debug
- Activer les optimisations (`-O2` ou `-O3`) en mode Release — **critique** pour un raytracer

Un `Makefile` wrapper minimal (qui appelle CMake) est recommandé pour satisfaire l'exigence "simple `make` à la racine".

---

## 🗂️ Organisation proposée du dépôt

```
.
├── CMakeLists.txt
├── Makefile                    # wrapper vers cmake
├── PROJECT.md                  # ce fichier
├── README.md                   # doc utilisateur
├── raytracer                   # binaire (généré)
├── src/
│   ├── main.cpp
│   ├── core/                   # moteur : Ray, Scene, Renderer, Camera
│   ├── math/                   # Vector3, Matrix4, utils
│   ├── interfaces/             # IPrimitive, ILight, IMaterial, IPlugin
│   ├── primitives/             # Sphere, Plane, Cylinder, Cone...
│   ├── lights/                 # Directional, Ambient, Point...
│   ├── materials/              # FlatColor, Phong, Checkerboard...
│   ├── parser/                 # SceneParser (libconfig++)
│   ├── plugins/                # PluginLoader (encapsulation libdl)
│   └── output/                 # PPMWriter, SFMLDisplay (bonus)
├── plugins/                    # .so générés
├── scenes/                     # scènes de démo (.cfg)
├── screenshots/                # rendus de référence (.ppm)
├── tests/                      # tests unitaires (si on en fait)
└── bonus/                      # tout ce qui est bonus et séparable
```

Cette structure est une proposition — elle doit évoluer. Ce qui compte : **une séparation claire entre les concepts**, pas un seul gros dossier `src/` plat.

---

## 🧪 Scènes de démonstration

Pour la soutenance, le sujet exige explicitement des scènes prêtes à être rendues **rapidement**. Si on doit attendre 10 minutes un rendu pendant la soutenance, on perd des points ("the teaching team is allowed to say no and move on because you did not prepare enough").

**À produire au fur et à mesure** :
- `scenes/demo_sphere.cfg` / `screenshots/demo_sphere.ppm`
- `scenes/demo_plane.cfg` / `screenshots/demo_plane.ppm`
- `scenes/demo_cylinder.cfg` / `screenshots/demo_cylinder.ppm`
- `scenes/demo_cone.cfg` / `screenshots/demo_cone.ppm`
- `scenes/light_point.cfg` / `screenshots/light_point.ppm`
- `scenes/light_directional.cfg` / `screenshots/light_directional.ppm`
- Une scène par bonus implémenté (`demo_reflection.cfg`, `demo_phong.cfg`, `demo_texture_chess.cfg`, etc.)
- Une scène "showcase" qui combine un maximum de features en un seul rendu impressionnant

---

## 🧭 Ordre de travail recommandé

### Phase 0 — Fondations (avant tout code de rendu)
1. Mettre en place CMake + structure de dossiers
2. Classes mathématiques : `Vector3`, `Ray`, `Color` — **bien testées**
3. Définir les interfaces : `IPrimitive`, `ILight`, `IMaterial`
4. Squelette du `PluginLoader` (même vide, la signature doit être posée)
5. Parser de scène minimal avec libconfig++
6. Sortie PPM fonctionnelle (écriture d'une image de test damier sans raytracing)

### Phase 1 — MUST
7. Camera + génération des rayons primaires
8. Sphère + intersection rayon/sphère
9. Plan + intersection rayon/plan
10. Lumière ambiante + directionnelle
11. Matériau Flat color
12. Translation (transformation basique)
13. **Milestone : `./raytracer scenes/demo_sphere.cfg` produit une image PPM correcte**

### Phase 2 — SHOULD
14. Cylinder, Cone
15. Rotation
16. Drop shadows (rayons d'ombre)
17. **Milestone : tous les MUST + SHOULD valides, architecture toujours propre**

### Phase 3 — Refactor / Pause architecture
18. Revue complète de l'architecture. Est-ce que les plugins marchent vraiment ? Est-ce que l'ajout d'une primitive demande 1 fichier ou 15 modifs ?
19. Mise en place du **système de plugins réel** si pas déjà fait
20. Transformation matrix (bonus structurant à faire tôt dans les bonus)
21. Multithreading (gagner du temps pour tous les rendus suivants)

### Phase 4 — Empilement de bonus
22. Phong reflection model (énorme gain visuel)
23. Reflection, Refraction, Transparency
24. Textures (file, chessboard, Perlin)
25. Antialiasing
26. Primitives exotiques (Torus, Triangles, .OBJ loader…)
27. Space partitioning (BVH) si on a du volume de géométrie
28. Display SFML + hot reload

### Phase 5 — Polish
29. Nettoyage final, revue de tous les warnings
30. Documentation (README utilisateur, commentaires sur les parties mathématiques non-triviales)
31. Préparation des scènes de démo et screenshots
32. Rendu final de la scène "showcase"

---

## ⚠️ Pièges et rappels importants

- **Toujours respecter le code de sortie 84** pour toute erreur (mauvais argument, fichier introuvable, parse error, etc.)
- **Tous les messages d'erreur sur `std::cerr`**, jamais sur `std::cout`
- **Ne jamais pusher** les binaires, les `.o`, les fichiers temporaires, le dossier `build/`
- **`.gitignore` dès le début** : `build/`, `*.o`, `raytracer`, `plugins/*.so`, `*.swp`, `.vscode/`, etc.
- Les plugins doivent être dans `./plugins/` à la racine — **pas** dans `build/plugins/` ou ailleurs
- Le dossier `bonus/` est pour les fichiers de bonus qui peuvent être séparés proprement
- La soutenance se prépare : scènes + screenshots + capacité à expliquer chaque design pattern utilisé

---

## 🧠 Rappels sur le raytracing (pour référence rapide)

Le raytracing simule le chemin **inverse** de la lumière : pour chaque pixel de l'image, on lance un rayon depuis la caméra, on cherche la première intersection avec un objet de la scène, et on calcule la couleur à partir du matériau et des lumières.

Pseudocode minimal du cœur du moteur :

```
pour chaque pixel (x, y) de l'image :
    rayon = caméra.generateRay(x, y)
    hit  = scène.findClosestIntersection(rayon)
    si hit :
        couleur = calculerÉclairage(hit, scène, lumières)
    sinon :
        couleur = couleur_de_fond
    image[x][y] = couleur
écrire image au format PPM
```

Tout le reste (réflexion, réfraction, ombres, textures, Phong…) s'ajoute dans `calculerÉclairage` et éventuellement en relançant des rayons secondaires.

---

## 📌 Convention de travail

- **Avant toute session**, relire la section pertinente de ce fichier
- **Avant de coder une feature**, vérifier qu'elle est à la bonne phase dans l'ordre de travail
- **Avant de rajouter un `if` de plus dans une classe qui grossit**, se demander si le pattern choisi est encore le bon
- **Après chaque feature fonctionnelle**, cocher la case `[ ]` → `[x]` dans ce fichier pour garder une trace claire de ce qui est fait et de ce qui reste à faire
- **Mettre à jour les .md** Quand quoi que ce soit change, il faut mettre a jour les fichiers de documentation pour garder une trace claire des évolutions.

---

*Version du sujet : v1.4 — Epitech G-OOP-400*