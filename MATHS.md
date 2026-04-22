# COURS_MATH.md — Petit cours de maths pour le Raytracer

> **Pour qui ?** Quelqu'un qui sait coder et qui a fait un minimum de maths mais qui n'a **jamais fait de 3D**, jamais manipulé de matrices "pour de vrai", et n'a pas envie de lire un bouquin de 400 pages.
>
> **Objectif** : comprendre, dans l'ordre, **tout ce qu'un raytracer utilise en maths**, avec le lien direct vers le code de ce projet. À la fin, tu dois par exemple pouvoir relire `Matrix4.cpp` en comprenant **pourquoi** chaque ligne est là.
>
> **Ce que ce cours ne fait PAS** : pas de preuves formelles, pas d'analyse, pas d'algèbre linéaire abstraite (espaces vectoriels, bases, déterminants théoriques…). Juste ce qu'il faut pour rendre des images.
>

---

## 📑 Sommaire

- [COURS\_MATH.md — Petit cours de maths pour le Raytracer](#cours_mathmd--petit-cours-de-maths-pour-le-raytracer)
  - [📑 Sommaire](#-sommaire)
  - [1. Le décor : le repère 3D](#1-le-décor--le-repère-3d)
  - [2. Vecteurs](#2-vecteurs)
    - [Opérations de base](#opérations-de-base)
  - [3. Le produit scalaire (dot)](#3-le-produit-scalaire-dot)
    - [Utilisations concrètes en raytracing](#utilisations-concrètes-en-raytracing)
  - [4. Le produit vectoriel (cross)](#4-le-produit-vectoriel-cross)
    - [Utilisations](#utilisations)
  - [5. Normalisation](#5-normalisation)
  - [6. Couleurs : linéaire vs sRGB](#6-couleurs--linéaire-vs-srgb)
    - [Espace linéaire](#espace-linéaire)
    - [Espace sRGB](#espace-srgb)
    - [Pourquoi on travaille en linéaire](#pourquoi-on-travaille-en-linéaire)
  - [7. Le rayon](#7-le-rayon)
    - [Intervalle valide](#intervalle-valide)
  - [8. Les matrices : pourquoi, comment](#8-les-matrices--pourquoi-comment)
    - [Pourquoi une matrice ?](#pourquoi-une-matrice-)
    - [Ce qui rend les matrices géniales](#ce-qui-rend-les-matrices-géniales)
  - [9. Multiplier une matrice par un vecteur](#9-multiplier-une-matrice-par-un-vecteur)
    - [Le mécanisme](#le-mécanisme)
    - [Visualisation](#visualisation)
  - [10. Multiplier deux matrices (composition)](#10-multiplier-deux-matrices-composition)
    - [Ordre : CAPITAL](#ordre--capital)
  - [11. Les coordonnées homogènes : le truc `w`](#11-les-coordonnées-homogènes--le-truc-w)
    - [Le problème](#le-problème)
    - [L'astuce](#lastuce)
  - [12. Les transformations classiques](#12-les-transformations-classiques)
    - [Translation](#translation)
    - [Scaling (mise à l'échelle)](#scaling-mise-à-léchelle)
    - [Rotation](#rotation)
  - [13. L'inverse d'une matrice](#13-linverse-dune-matrice)
    - [Définition](#définition)
    - [Pourquoi c'est crucial en raytracing](#pourquoi-cest-crucial-en-raytracing)
    - [Comment on calcule `M⁻¹`](#comment-on-calcule-m)
  - [14. Le piège des normales](#14-le-piège-des-normales)
    - [Le problème](#le-problème-1)
    - [La formule correcte](#la-formule-correcte)
    - [Pourquoi Transform cache l'inverse](#pourquoi-transform-cache-linverse)
  - [15. La classe Transform](#15-la-classe-transform)
  - [16. Les bounding boxes (AABB)](#16-les-bounding-boxes-aabb)
    - [Convention "boîte vide"](#convention-boîte-vide)
  - [17. (À venir) Le pipeline raytracing](#17-à-venir-le-pipeline-raytracing)
    - [Camera pinhole (T0.13)](#camera-pinhole-t013)
    - [Intersection rayon/sphère (T1.1)](#intersection-rayonsphère-t11)
    - [Intersection rayon/plan (T1.2)](#intersection-rayonplan-t12)
    - [Shadow rays (T2.4)](#shadow-rays-t24)
    - [Lambert + Phong (T4.1)](#lambert--phong-t41)
    - [Réflexion (T4.2)](#réflexion-t42)
    - [Réfraction (T4.3) — loi de Snell](#réfraction-t43--loi-de-snell)
  - [🧭 Pour aller plus loin](#-pour-aller-plus-loin)
  - [📎 Mémo des formules](#-mémo-des-formules)

---

## 1. Le décor : le repère 3D

Tout se passe dans un espace à 3 dimensions. On pose un **repère** : trois axes perpendiculaires X, Y, Z, qui se coupent en un point appelé **origine** (coordonnées `(0, 0, 0)`).

```
        Y (haut)
        ▲
        │
        │
        │          ← scène
        │        ╱
        │      ╱
        │    ╱
        │  ╱
        ╰───────────► X (droite)
       ╱
      ╱
     ╱
    ▼
    Z (vers l'observateur)
```

**Convention de ce projet** : repère **main droite** (right-handed). Concrètement : si tu pointes l'index de ta main droite vers X et le majeur vers Y, le pouce pointe vers Z (vers toi). La grande majorité des raytracers (PBRT, Blender en mode droit, OpenGL) utilise cette convention.

Un **point** dans cet espace = 3 nombres. Position de la caméra, centre d'une sphère, point où un rayon touche un objet : tout est `(x, y, z)`.

🔧 **CODE** : `src/math/Vector3.hpp` stocke 3 `double`. On utilise le même type pour les **points** et les **directions** — le contexte d'usage dit lequel c'est. (On verra section 11 qu'un système plus rigoureux existe avec les coordonnées homogènes.)

---

## 2. Vecteurs

Un **vecteur** en 3D c'est un triplet `(x, y, z)`. Géométriquement il représente soit :

- un **point** (position absolue dans l'espace),
- une **direction** (pas d'origine, juste une flèche qui pointe quelque part),
- un **déplacement** (du point A au point B).

```
   Point P            Direction D            Déplacement (B - A)
                      ↗
     •                                          B
    (3,2,1)          direction n'a                ↗
                     pas de position              A

```

Le **même triplet** peut représenter les trois. La différence est sémantique — c'est pour ça que section 11 introduira un **4ᵉ nombre** qui dit "point ou direction ?".

### Opérations de base

```
Addition :       A + B = (Ax+Bx, Ay+By, Az+Bz)
Soustraction :   A - B = (Ax-Bx, Ay-By, Az-Bz)   ← donne un VECTEUR allant de B à A
Multiplication
par un scalaire : k * V = (k·Vx, k·Vy, k·Vz)     ← allonge / raccourcit / inverse
```

**Interprétation géométrique** :

```
   A                           A+B                       A
    \                          ↗                         ↘
     \                        ↗                           ↘
      ▼   +   B  ──►         ↗        A - B  ──►           B  ───► vecteur B→A
             ↗              ↗                             ↗
```

🔧 **CODE** : `Vector3.hpp` fournit `operator+`, `operator-`, `operator*(double)`. Tous renvoient **un nouveau vecteur** — on ne modifie jamais en place (règle d'immuabilité du projet).

---

## 3. Le produit scalaire (dot)

**Définition** :

$$
A \cdot B = A_x B_x + A_y B_y + A_z B_z
$$

C'est un **nombre** (un scalaire), pas un vecteur. Tu multiplies composant par composant et tu additionnes.

**Pourquoi c'est utile** : il encode l'**angle** entre deux vecteurs.

$$
A \cdot B = \|A\| \cdot \|B\| \cdot \cos(\theta)
$$

où `‖A‖` est la longueur de A et θ l'angle entre A et B.

```
Angle θ :        dot result :      interprétation :
═════════        ═════════════     ═════════════════
θ = 0°            = +‖A‖·‖B‖        vecteurs alignés (même sens)
θ = 90°           = 0               perpendiculaires  ← TRÈS utilisé
θ = 180°          = -‖A‖·‖B‖        opposés
```

Cas particulier **crucial** : si A et B sont tous les deux **unitaires** (longueur 1), alors `A·B = cos(θ)` directement.

### Utilisations concrètes en raytracing

1. **Éclairage Lambert (diffus)** : une surface de normale `n` éclairée par une lumière de direction `l` reçoit une intensité proportionnelle à `max(0, n · l)`.

    ```
        lumière l
           ╲
            ╲  θ
             ▼──────── surface
             │ n (normale)

         n·l = cos(θ)
         → 1 quand face à la lumière
         → 0 quand la surface est de côté
         → négatif quand elle tourne le dos (on clampe à 0)
    ```

2. **Détecter si un rayon tape la face avant d'un objet** : `dot(ray.direction, normal) < 0` → la normale pointe contre le rayon → face avant.

3. **Projection d'un vecteur sur un autre** : `proj_B(A) = (A·B / B·B) · B`.

4. **Réflexion** (formule Phong) :

$$
R = D - 2(D \cdot N) N
$$

   où D est la direction du rayon entrant et N la normale.

🔧 **CODE** :
```cpp
double Vector3::dot(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}
```

---

## 4. Le produit vectoriel (cross)

**Définition** :

$$
A \times B = \begin{pmatrix} A_y B_z - A_z B_y \\ A_z B_x - A_x B_z \\ A_x B_y - A_y B_x \end{pmatrix}
$$

Résultat : **un vecteur** (pas un nombre), qui est **perpendiculaire à A ET à B** en même temps.

```
        A × B   (perpendiculaire au plan A,B)
         ▲
         │
         │        B
         │      ↗
         │    ↗
         │  ↗
         │↗
         ●────────► A
```

**Règle de la main droite** : si tu places ta main droite pour passer de A à B, le pouce donne la direction de `A × B`. Conséquence : `B × A = -(A × B)` (l'ordre compte).

**Norme** : `‖A × B‖ = ‖A‖·‖B‖·sin(θ)` — c'est l'**aire du parallélogramme** formé par A et B.

### Utilisations

1. **Construire une base orthogonale** à partir d'une direction — la caméra pinhole le fera (T0.13) : à partir de `forward` et d'un `up` approximatif, on calcule `right = forward × up`, puis `up_correct = right × forward`.

2. **Normale d'un plan défini par 3 points P, Q, R** : `n = (Q - P) × (R - P)`, puis normalisation.

3. **Tester de quel côté d'une arête est un point** (utile pour l'intersection triangle, bonus T4.11).

🔧 **CODE** :
```cpp
Vector3 Vector3::cross(const Vector3& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
    };
}
```

⚠️ **PIÈGE** : `cross` n'est **PAS commutatif**. `A × B ≠ B × A`.

---

## 5. Normalisation

La **longueur** (ou **norme**) d'un vecteur :

$$
\|V\| = \sqrt{V_x^2 + V_y^2 + V_z^2}
$$

Un vecteur **unitaire** a une longueur de 1. Beaucoup de formules de raytracing supposent des vecteurs unitaires (la formule Lambert `n·l = cos(θ)` ne marche que si n et l sont unitaires !).

**Normaliser** = diviser par sa longueur :

$$
\hat{V} = \frac{V}{\|V\|}
$$

```
V = (3, 0, 4)   ─► ‖V‖ = √(9+16) = 5   ─►  V̂ = (0.6, 0, 0.8)
```

⚠️ **PIÈGE** : on ne peut pas normaliser un vecteur nul `(0,0,0)` — division par zéro.

🔧 **CODE** :
```cpp
Vector3 Vector3::normalized() const {
    double len = length();
    if (len == 0.0) throw MathError("Vector3::normalized: zero-length vector");
    return *this / len;
}
```

Le projet traite ce cas comme une **erreur de frontière** (on lève `rt::MathError`), jamais dans les hot paths.

---

## 6. Couleurs : linéaire vs sRGB

Une couleur se stocke en RGB : trois canaux `(r, g, b)`, chacun entre 0 et 1 (ou plus : HDR).

**Mais attention** : il y a **deux espaces** très différents.

### Espace linéaire

Les valeurs représentent physiquement l'**énergie lumineuse**. Additionner deux lumières = additionner leurs valeurs linéaires. Multiplier une couleur de surface par une lumière = multiplication canal par canal.

```
Couleur de surface   ×   Radiance de la lumière   =   Couleur vue
  (albedo, [0,1])        (peut être > 1, HDR)         (linéaire)

(0.8, 0.3, 0.1)      ×   (1.0, 1.0, 1.0)         =   (0.8, 0.3, 0.1)
surface orange           lumière blanche standard
```

### Espace sRGB

Ce que ton écran affiche. Il applique une courbe de correction gamma (≈ `x^(1/2.2)`) pour compenser la perception non-linéaire de l'œil.

```
sRGB ─correction gamma⁻¹─► linéaire ─calculs─► linéaire ─correction gamma─► sRGB (écran)
```

### Pourquoi on travaille en linéaire

Si on additionne deux lumières **en sRGB**, on obtient des résultats faux (trop saturés). Tous les calculs d'éclairage doivent se faire en linéaire, et on ne convertit qu'**au tout dernier moment**, avant d'écrire le fichier image.

🔧 **CODE** :
- `src/math/Color.hpp` : `Color` stocke en **float linéaire**, pas de clamp, pas de conversion.
- `Color * Color` = multiplication **composante par composante** (produit de Hadamard) : utilisé pour `albedo × radiance`.
- `Color + Color` = accumulation additive (sommes de sources lumineuses).
- La conversion linéaire → sRGB sera faite dans `PPMWriter` (T0.12) — et nulle part ailleurs.

---

## 7. Le rayon

Un **rayon** est une demi-droite :

$$
P(t) = O + t \cdot D, \quad t \geq 0
$$

- `O` = origine (point),
- `D` = direction (vecteur, idéalement unitaire),
- `t` = paramètre scalaire (distance le long du rayon si D est unitaire).

```
         t=0              t=1                   t=2.5
          ●────────────────●──────────────────────●────────►
         O                                                   D
     (origine)                                        (direction)
```

Trouver une **intersection** = trouver `t ≥ 0` tel que `P(t)` touche un objet.

### Intervalle valide

En pratique on borne `t` entre `tMin` et `tMax` :

- `tMin` : un petit epsilon positif (`1e-4`) pour éviter qu'un rayon réfléchi touche immédiatement la surface d'où il part (à cause d'erreurs d'arrondi flottant — ce piège classique s'appelle **self-intersection**, on le voit sous la forme de taches noires bizarres sur les surfaces réfléchissantes).

- `tMax` : `+∞` par défaut, ou la distance de la lumière (pour les shadow rays : "est-ce qu'il y a un obstacle entre ce point et cette lumière ?").

🔧 **CODE** : `src/interfaces/Ray.hpp` — `Ray { origin, direction, tMin, tMax }`, avec `kRayEpsilon = 1e-4`. Helper `at(t) = origin + t*direction`.

---

## 8. Les matrices : pourquoi, comment

Une **matrice** est un tableau rectangulaire de nombres. En 3D, on utilise des matrices **4×4** (on verra pourquoi 4 et pas 3 en section 11).

```
         ┌ m00  m01  m02  m03 ┐
    M =  │ m10  m11  m12  m13 │
         │ m20  m21  m22  m23 │
         └ m30  m31  m32  m33 ┘
```

### Pourquoi une matrice ?

Une matrice **encode une transformation** de l'espace : déplacement, rotation, mise à l'échelle, ou n'importe quelle combinaison de ces trois. Appliquer la transformation à un point = **multiplier** la matrice par le point.

### Ce qui rend les matrices géniales

Composer deux transformations = **multiplier deux matrices**. Une fois multipliées, on a **une seule matrice** qui fait les deux opérations en un seul coup.

```
Au lieu de :    "applique rotation, puis translation, puis échelle, puis ..."
                (N opérations par point)

On fait :       M_total = T × R × S × ...            (1 fois, à la construction)
                P' = M_total × P                     (1 opération par point)
```

Dans un raytracer on transforme **des millions de rayons par image** — cette compression est vitale.

🔧 **CODE** : `src/math/Matrix4.hpp` stocke `std::array<double, 16>` en **row-major** (élément `(r, c)` à l'index `r*4 + c`). Default = **identité** (la matrice qui ne change rien).

```
             ┌ 1 0 0 0 ┐
Identité I = │ 0 1 0 0 │    →  I × V = V   pour tout V
             │ 0 0 1 0 │
             └ 0 0 0 1 ┘
```

---

## 9. Multiplier une matrice par un vecteur

### Le mécanisme

Transformer un point `P = (x, y, z, w)` par une matrice `M` : chaque composante du résultat est le **produit scalaire** d'une **ligne** de M par le vecteur P.

$$
M \cdot P = \begin{pmatrix} \text{ligne 0 } \cdot P \\ \text{ligne 1 } \cdot P \\ \text{ligne 2 } \cdot P \\ \text{ligne 3 } \cdot P \end{pmatrix}
$$

Concrètement pour la ligne 0 :

```
P'_x = m00·x + m01·y + m02·z + m03·w
```

Et pareil pour `P'_y`, `P'_z`, `P'_w`.

### Visualisation

```
   ┌ m00 m01 m02 m03 ┐   ┌ x ┐     ┌ m00·x + m01·y + m02·z + m03·w ┐
   │ m10 m11 m12 m13 │   │ y │     │ m10·x + m11·y + m12·z + m13·w │
   │ m20 m21 m22 m23 │ × │ z │  =  │ m20·x + m21·y + m22·z + m23·w │
   └ m30 m31 m32 m33 ┘   └ w ┘     └ m30·x + m31·y + m32·z + m33·w ┘
```

🔧 **CODE** :
```cpp
Vector3 Matrix4::applyToPoint(const Vector3& p) const {
    // w = 1 pour un point (voir section 11)
    double x = (*this)(0,0)*p.x + (*this)(0,1)*p.y + (*this)(0,2)*p.z + (*this)(0,3);
    double y = (*this)(1,0)*p.x + (*this)(1,1)*p.y + (*this)(1,2)*p.z + (*this)(1,3);
    double z = (*this)(2,0)*p.x + (*this)(2,1)*p.y + (*this)(2,2)*p.z + (*this)(2,3);
    return {x, y, z};
}
```

---

## 10. Multiplier deux matrices (composition)

`C = A × B` : chaque élément `C(i,j)` est le **produit scalaire** de la ligne `i` de A par la colonne `j` de B.

$$
C_{ij} = \sum_{k=0}^{3} A_{ik} \cdot B_{kj}
$$

```
   ┌────────────────┐       ┌──b0j──┐          ┌────────────┐
   │ A line i ──►   │   ×   │  b1j  │    =     │ ...  Cij...│
   │                │       │  b2j  │          │            │
   └────────────────┘       └──b3j──┘          └────────────┘
```

### Ordre : CAPITAL

La multiplication matricielle **n'est pas commutative** : `A × B ≠ B × A` en général.

Dans ce projet on utilise la convention :

> **`M = T × R × S`** signifie "appliquer S d'abord, puis R, puis T"

C'est-à-dire qu'on lit **de droite à gauche** dans l'ordre d'application. Raison : `M × P = T × (R × (S × P))` — le point P passe d'abord dans la matrice la plus à droite.

```
Un exemple :  placer une sphère à (10, 0, 0), orientée, taillée à 2x

M = Translation(10,0,0) × Rotation × Scale(2)

Ordre logique appliqué au point :
   1. Scale(2) : la sphère grossit (toujours centrée à l'origine)
   2. Rotation : elle tourne (toujours centrée à l'origine)
   3. Translation : elle se déplace à sa position finale

C'est TOUJOURS cet ordre : S → R → T   (Scale, Rotate, Translate)
```

⚠️ **PIÈGE** : inverser l'ordre donne des résultats visuellement catastrophiques. `R × T` fait tourner **avec le pivot à l'origine**, ce qui déplace aussi l'objet au lieu de le faire tourner sur lui-même.

🔧 **CODE** : `Matrix4::operator*` — triple boucle classique O(64) multiplications.

---

## 11. Les coordonnées homogènes : le truc `w`

### Le problème

Une matrice 3×3 peut faire des rotations et des scaling, mais **pas des translations** (on ne peut pas additionner une constante en multipliant). Pourtant "déplacer" est l'opération la plus courante en 3D.

### L'astuce

On ajoute une **4ᵉ coordonnée** `w` aux vecteurs :

- **Point** : `w = 1`
- **Direction** : `w = 0`

Et on passe à des matrices **4×4**. La 4ᵉ colonne contient maintenant la translation :

```
      ┌ 1  0  0  tx ┐         ┌ x + tx ┐
      │ 0  1  0  ty │   ×     │ y + ty │     ← translation fonctionne !
      │ 0  0  1  tz │         │ z + tz │       (car w = 1)
      └ 0  0  0  1  ┘         └   1    ┘

                              ┌ x ┐
                      ×       │ y │             ← sur une direction (w=0)
                              │ z │               la translation est IGNORÉE
                              └ 0 ┘               (car m·0 = 0)
```

**C'est exactement ce qu'on veut** : déplacer un objet **déplace ses points**, mais **ne change pas la direction des flèches** posées dessus.

```
Point P    ═══ translate ═══►   P + t     (position change)
Direction D ══ translate ══►   D           (direction IDEM)
```

🔧 **CODE** : le 4ᵉ composant n'est **pas stocké** dans `Vector3` — il est implicite dans la fonction appelée :

```cpp
Matrix4::applyToPoint(v);      // traite v comme (x, y, z, 1)
Matrix4::applyToDirection(v);  // traite v comme (x, y, z, 0)
```

C'est un choix d'API : plus simple que de trimballer un `Vector4` partout.

---

## 12. Les transformations classiques

### Translation

$$
T(t_x, t_y, t_z) = \begin{pmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{pmatrix}
$$

Son **inverse** : `T(-tx, -ty, -tz)`. Trivial.

### Scaling (mise à l'échelle)

$$
S(s_x, s_y, s_z) = \begin{pmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}
$$

Si `sx = sy = sz`, on parle de scale **uniforme**. Sinon, **non uniforme** (une sphère devient une ellipsoïde) — attention, les normales deviennent fausses si on ne les traite pas correctement (section 14).

Son inverse : `S(1/sx, 1/sy, 1/sz)`. Impossible si une composante = 0 → `MathError`.

### Rotation

Autour de l'axe **X** d'angle θ :

$$
R_x(\theta) = \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos\theta & -\sin\theta & 0 \\ 0 & \sin\theta & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}
$$

Autour de **Y** :

$$
R_y(\theta) = \begin{pmatrix} \cos\theta & 0 & \sin\theta & 0 \\ 0 & 1 & 0 & 0 \\ -\sin\theta & 0 & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}
$$

Autour de **Z** :

$$
R_z(\theta) = \begin{pmatrix} \cos\theta & -\sin\theta & 0 & 0 \\ \sin\theta & \cos\theta & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}
$$

```
Rotation autour de Z, angle θ :

    Y▲        Y▲
     │         │   ●  nouveau point
     │         │  ╱
     │ ● old   │ ╱ θ
     │╱        │╱
     ●────►X   ●────►X
```

**Propriété magique** : l'inverse d'une rotation est sa **transposée** (échanger lignes ↔ colonnes). Ou plus simple : `R(θ)⁻¹ = R(-θ)`.

🔧 **CODE** : `Matrix4::rotationX/Y/Z` construit la matrice ; `Transform::rotationX(θ).inverse()` est gratuit car on stocke les deux (cf. section 15).

---

## 13. L'inverse d'une matrice

### Définition

L'inverse de M est la matrice `M⁻¹` telle que :

$$
M \cdot M^{-1} = M^{-1} \cdot M = I
$$

Autrement dit, elle **défait** l'effet de M. Si M "déplace, tourne, agrandit", `M⁻¹` "rapetisse, tourne en arrière, déplace en arrière".

### Pourquoi c'est crucial en raytracing

Les primitives (sphère, plan, cylindre…) sont définies dans une forme **canonique** : sphère de rayon 1 centrée à l'origine, plan Z=0, cylindre vertical… Calculer une intersection rayon/sphère canonique est **simple** (une équation du 2ᵈ degré).

Dans la scène, la sphère a un `Transform` (position, orientation, taille). Au lieu de transformer la sphère (lourd, chaque face), on **transforme le rayon dans l'autre sens** :

```
      Espace monde                 Espace objet
      ════════════                 ════════════
                                        
         sphère déformée            sphère unité
           ╱╲     ↖                   ───
          (  )    ╲                   ╱ ╲
           ╲╱      rayon              ───
                                        ↗
                                       rayon dans l'espace objet

  rayon_objet = M⁻¹ × rayon_monde
  → intersecter contre la sphère unité (simple)
  → transformer le point de contact par M (retour monde)
```

### Comment on calcule `M⁻¹`

Pour une matrice quelconque, on utilise l'**élimination de Gauss-Jordan** avec pivot partiel. L'idée :

1. On écrit la matrice augmentée `[M | I]` (M à gauche, identité à droite).
2. On applique des opérations de lignes (échanges, multiplications, soustractions) pour transformer M en I.
3. Les mêmes opérations appliquées à I donnent `M⁻¹` à droite.

```
État initial :     ┌ M M M M │ 1 0 0 0 ┐
                   │ M M M M │ 0 1 0 0 │
                   │ M M M M │ 0 0 1 0 │
                   └ M M M M │ 0 0 0 1 ┘

Après Gauss :      ┌ 1 0 0 0 │ I I I I ┐
                   │ 0 1 0 0 │ I I I I │      ← ça, c'est M⁻¹
                   │ 0 0 1 0 │ I I I I │
                   └ 0 0 0 1 │ I I I I ┘
```

**Pivot partiel** : à chaque étape, on échange la ligne courante avec celle qui a le plus grand élément en valeur absolue dans la colonne en cours. Sans ça, des erreurs d'arrondi explosent sur des matrices "presque singulières".

**Singularité** : certaines matrices n'ont pas d'inverse (un scale avec un 0, par exemple). Le code le détecte par un pivot trop petit et lève `MathError`.

🔧 **CODE** : `Matrix4::inverse()` dans `Matrix4.cpp` — Gauss-Jordan + pivot partiel, `kSingularityThreshold = 1e-12`.

---

## 14. Le piège des normales

### Le problème

Tu as une sphère. Tu la scales `(2, 1, 1)` (elle devient une ellipsoïde allongée). Tu prends une normale `n = (1, 1, 0)` normalisée, et tu la transformes avec la **même** matrice que les points.

Résultat : la normale **ne pointe plus perpendiculairement à la surface**.

```
Avant scale :                 Après scale (naïf) :
   
     ⬆ n = (1,1,0)                       ⬆ n_naïf = (2,1,0)
     │ ╱                                   │    ╱
     │╱                                    │  ╱
     ●────── surface                       ●──────── surface scalée
                                             
  ✓ n perpendiculaire                  ✗ pas perpendiculaire !
```

### La formule correcte

Pour transformer une **normale**, il faut utiliser la **transposée de l'inverse** :

$$
n' = (M^{-1})^T \cdot n
$$

Intuition : la matrice qui préserve la perpendicularité est l'**inverse transposée** de la matrice qui déforme l'espace. On le démontre en 3 lignes d'algèbre mais tu peux le prendre comme un fait.

**Cas particulier** : si M est une rotation pure ou un scale **uniforme**, alors `(M⁻¹)ᵀ = M` (à un scalaire près) — et la transformation naïve marche par chance. Le piège apparaît uniquement avec les scales **non uniformes** et les shears.

### Pourquoi Transform cache l'inverse

Puisqu'on a **besoin** de `M⁻¹` pour transformer les rayons (section 13) ET pour transformer les normales (cette section), on stocke les deux matrices côte à côte. L'inverse est calculé **une fois**, à la construction du Transform, puis réutilisé à chaque rayon.

🔧 **CODE** :
```cpp
Vector3 Transform::applyToNormal(const Vector3& n) const {
    // (M⁻¹)ᵀ · n   — mais on ne construit PAS la transposée :
    // on accède à inverse_(k, i) au lieu de inverse_(i, k).
    double x = inverse_(0,0)*n.x + inverse_(1,0)*n.y + inverse_(2,0)*n.z;
    double y = inverse_(0,1)*n.x + inverse_(1,1)*n.y + inverse_(2,1)*n.z;
    double z = inverse_(0,2)*n.x + inverse_(1,2)*n.y + inverse_(2,2)*n.z;
    return {x, y, z};
    // Le caller normalise si besoin (économie de sqrt si pas nécessaire).
}
```

---

## 15. La classe Transform

Maintenant que tu as tous les ingrédients, voici la logique du type `Transform` :

```
┌──────────────────────────────────────────────────┐
│                    Transform                     │
├──────────────────────────────────────────────────┤
│  Matrix4 matrix_    ← la transformation directe  │
│  Matrix4 inverse_   ← pré-calculée               │
└──────────────────────────────────────────────────┘
```

**Pourquoi les deux** :
- `matrix_` : transformer un point/direction de l'espace objet vers l'espace monde (`applyToPoint`, `applyToDirection`).
- `inverse_` : transformer un rayon du monde vers l'objet, **et** transformer une normale via `(M⁻¹)ᵀ`.
- `inverse()` comme opération : **zéro FLOP**, on échange les deux membres.

**Factories analytiques** : quand on connaît l'inverse exact (`translation → négation`, `rotation → angle opposé`, `scaling → réciproques`), on le construit **directement** sans passer par Gauss-Jordan. Plus rapide + plus précis.

```cpp
Transform::translation(1, 2, 3)
  → matrix_  = [I | (1,2,3)]
  → inverse_ = [I | (-1,-2,-3)]       ← pas de Gauss !

Transform(some_arbitrary_matrix)
  → matrix_  = some_arbitrary_matrix
  → inverse_ = Gauss-Jordan(matrix_)  ← peut throw MathError
```

**Composition** : `T1 * T2` signifie "appliquer T2 d'abord, puis T1". La matrice résultat est `T1.matrix_ * T2.matrix_`. L'inverse du résultat est `T2.inverse_ * T1.inverse_` (**ordre inversé** — c'est un théorème).

```
(A · B)⁻¹ = B⁻¹ · A⁻¹
```

Intuition : pour défaire "chaussettes puis chaussures", il faut d'abord enlever les chaussures, puis les chaussettes.

---

## 16. Les bounding boxes (AABB)

**AABB** = Axis-Aligned Bounding Box = boîte rectangulaire dont les faces sont parallèles aux axes X, Y, Z.

```
         ┌─────────────────┐  ← max = (xMax, yMax, zMax)
        ╱                 ╱│
       ╱                 ╱ │
      ┌─────────────────┐  │
      │     objet       │  │
      │      ●          │  │
      │    ╱   ╲        │  ╱
      │   ●──●──●       │ ╱
      │                 │╱
      └─────────────────┘
        ↑
        min = (xMin, yMin, zMin)
```

**Pourquoi** : tester un rayon contre une AABB est **très rapide** (6 comparaisons). Si le rayon ne touche pas la boîte, il ne peut pas toucher l'objet dedans → on skip l'intersection précise. Base de toutes les structures d'accélération (BVH, T3.3 bonus).

### Convention "boîte vide"

Une AABB par défaut doit représenter "aucun point". La convention naïve `min = max = (0,0,0)` est un piège : si tous les points sont négatifs, la boîte résultante reste coincée à l'origine.

**Bonne convention** : `min = (+∞, +∞, +∞)` et `max = (−∞, −∞, −∞)`. Cette boîte inversée est **vide** (aucun point ne la satisfait), mais n'importe quel `expanded(p)` produit immédiatement une boîte finie correcte.

```cpp
AABB box;                                  // empty sentinel
for (auto& p : points) box = box.expanded(p);  // toujours correct
```

🔧 **CODE** : `src/interfaces/AABB.hpp` — `expanded(p)`, `merged(other)`, `isEmpty()`.

---

## 17. (À venir) Le pipeline raytracing

Cette section est un **prévis** de ce qu'on implémentera en Phase 1+ ; les maths ci-dessus suffisent à tout comprendre.

### Camera pinhole (T0.13)

La caméra est un point (position), trois axes `(right, up, forward)` et un FOV (champ de vision). Pour chaque pixel `(x, y)` de l'image, on construit un rayon qui part de la caméra et passe par la position virtuelle du pixel dans l'espace monde.

```
                     ●  pixel (x,y)
   camera           ╱
      ●───────────╱   ← rayon primaire
                ╱
              ╱
            ╱
          ╱  scène
        ╱
```

### Intersection rayon/sphère (T1.1)

Sphère de centre `C` et rayon `r` : les points `P` sur la surface satisfont `‖P - C‖² = r²`.

Rayon `P(t) = O + tD`. On remplace :

$$
\|O + tD - C\|^2 = r^2
$$

En développant (en utilisant `‖V‖² = V·V`), on obtient une **équation du second degré en t** :

$$
(D \cdot D) t^2 + 2 (D \cdot (O - C)) t + ((O - C) \cdot (O - C) - r^2) = 0
$$

Discriminant `Δ = b² - 4ac`. Si Δ < 0 : pas d'intersection. Sinon, deux racines `t₁ ≤ t₂` ; on garde la plus petite positive.

La **normale** au point d'intersection : `n = (P - C) / r`.

### Intersection rayon/plan (T1.2)

Plan Z=0 canonique : `P.z = 0`. On résout `O.z + t·D.z = 0` → `t = -O.z / D.z` (si `D.z ≠ 0`). Pour un plan orienté quelconque, on applique le `Transform` à l'envers sur le rayon — la sphère et le plan restent en forme canonique.

### Shadow rays (T2.4)

Pour savoir si un point P est éclairé par une lumière L, on lance un **rayon depuis P vers L**. Si ce rayon rencontre un objet avant d'atteindre L, P est dans l'ombre.

```
    L   ●
         \
          \  shadow ray
           \    ✘ ← bloqué par un mur
            \
             \
              \
               P  ●
```

### Lambert + Phong (T4.1)

**Lambert (diffus)** : `I = albedo × light × max(0, n·l)`. Simple produit scalaire.

**Phong (spéculaire)** : ajoute une reflet brillant. Le rayon réfléchi `R = D - 2(D·N)N`. L'intensité spéculaire = `(R·V)^shininess` où V est la direction vers la caméra.

```
      L (lumière)        V (caméra)
        ╲                  ╱
         ╲   │n          ╱
          ╲  │         ╱
           ╲ │       ╱ ≈ direction R réfléchie
            ╲│     ╱
     ────────●────────   surface
```

### Réflexion (T4.2)

Rayon secondaire dans la direction `R`. On intersecte la scène avec ce nouveau rayon, on shade, on pondère. La récursion est bornée par `depth` dans `ShadingContext`.

### Réfraction (T4.3) — loi de Snell

Quand un rayon traverse un matériau transparent, il change de direction :

$$
n_1 \sin\theta_1 = n_2 \sin\theta_2
$$

où n₁, n₂ sont les indices de réfraction (air = 1, eau ≈ 1.33, verre ≈ 1.5). La direction réfractée se calcule vectoriellement à partir de la normale, du rayon incident et du rapport `n₁/n₂`.

---

## 🧭 Pour aller plus loin

- **Ray Tracing in One Weekend** (Peter Shirley, gratuit en ligne) : le meilleur tuto d'entrée, en C++, complémentaire à ce cours.
- **Physically Based Rendering (PBRT)** (Pharr, Humphreys, Jakob) : la bible. Disponible gratuitement sur pbr-book.org. À picorer, pas à lire d'un trait.
- **Scratchapixel** (scratchapixel.com) : des articles illustrés sur chaque sujet (matrices, caméra, ombres, BRDF…).
- **3Blue1Brown — Essence of Linear Algebra** (YouTube) : la meilleure intuition visuelle des matrices qui existe, en 15 vidéos.

---

## 📎 Mémo des formules

| Opération | Formule | Notes |
|-----------|---------|-------|
| Norme | `‖V‖ = √(x² + y² + z²)` | |
| Normalisation | `V̂ = V / ‖V‖` | interdit si V = 0 |
| Produit scalaire | `A·B = Ax·Bx + Ay·By + Az·Bz` | = `‖A‖·‖B‖·cos θ` |
| Produit vectoriel | `A×B = (AyBz - AzBy, AzBx - AxBz, AxBy - AyBx)` | perpendiculaire à A et B |
| Rayon | `P(t) = O + t·D` | `t ≥ tMin` |
| Transform point | `P' = M · (P, 1)` | translation comprise |
| Transform direction | `D' = M · (D, 0)` | translation ignorée |
| Transform normale | `n' = (M⁻¹)ᵀ · n` | piège ⚠️ |
| Inverse d'un produit | `(AB)⁻¹ = B⁻¹·A⁻¹` | ordre inversé |
| Réflexion | `R = D - 2(D·N)N` | D et N unitaires |
| Intersection sphère | quadratique en t | garder plus petite racine ≥ 0 |
| Snell | `n₁ sin θ₁ = n₂ sin θ₂` | réfraction |
