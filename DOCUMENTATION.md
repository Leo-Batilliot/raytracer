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

_Aucun patch pour l'instant._

---

## Known issues

_Aucun problème connu pour l'instant._
