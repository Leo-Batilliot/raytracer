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
