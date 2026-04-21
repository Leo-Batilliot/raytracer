# DOCUMENTATION.md — Historique du projet Raytracer

> Ce document est un **journal technique vivant**. À chaque feature implémentée, à chaque choix notable, à chaque patch, on ajoute une entrée.
>
> - `PROJECT.md` = hub descriptif (philosophie, exigences sujet).
> - `PLAN.md` = feuille de route (tickets, phases, critères d'acceptation).
> - `DOCUMENTATION.md` (ce fichier) = **ce qui a été fait, quand, pourquoi, comment**.
>
> **Règle** : on ne coche pas un ticket dans `PLAN.md` sans avoir ajouté son entrée ici.

---

## 📑 Sommaire

- [DOCUMENTATION.md — Historique du projet Raytracer](#documentationmd--historique-du-projet-raytracer)
  - [📑 Sommaire](#-sommaire)
  - [Format d'une entrée](#format-dune-entrée)
  - [Versioning](#versioning)
  - [Changelog](#changelog)
    - [v0.0.1 — 2026-04-21 — Initialisation du dépôt (T0.1)](#v001--2026-04-21--initialisation-du-dépôt-t01)
  - [Architecture \& choix techniques](#architecture--choix-techniques)
    - [Design patterns retenus (v0.0.1)](#design-patterns-retenus-v001)
    - [Choix techniques clés (v0.0.1)](#choix-techniques-clés-v001)
  - [Features documentées](#features-documentées)
    - [Primitives](#primitives)
    - [Lumières](#lumières)
    - [Matériaux](#matériaux)
    - [Transformations](#transformations)
    - [Bonus](#bonus)
  - [Patches / bug fixes](#patches--bug-fixes)
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
