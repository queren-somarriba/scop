# Roadmap — Projet OpenGL C++ (scop-like)

> Langage : C++ | Rendu : OpenGL (GLFW + GLEW) | Pas de MinilibX | Matrices et parsers maison

---

## Structure des fichiers

```
project/
├── Makefile
├── srcs/
│   ├── main.cpp
│   ├── parser/
│   │   ├── ObjParser.cpp / .hpp   # parsing .obj à la main
│   │   └── Mesh.hpp               # struct Vertex, Face
│   ├── render/
│   │   ├── Renderer.cpp / .hpp    # VAO/VBO, draw calls
│   │   ├── ShaderManager.cpp      # compile GLSL sans lib
│   │   └── TextureLoader.cpp      # lecture BMP ou PPM maison
│   ├── scene/
│   │   ├── Scene.cpp / .hpp       # état global : objet, caméra
│   │   └── Transform.cpp          # matrices 4x4 maison
│   └── window/
│       └── Window.cpp / .hpp      # GLFW + callbacks
├── shaders/
│   ├── vertex.glsl
│   └── fragment.glsl
└── assets/
    ├── 42.obj
    ├── teapot.obj
    └── texture.bmp
```

---

## Dépendances autorisées

| Lib | Usage |
|-----|-------|
| **GLFW** | Fenêtre, contexte OpenGL, événements clavier/souris |
| **GLEW** | Chargement des extensions OpenGL |

Tout le reste (matrices, parser OBJ, loader de texture, compilation shaders) est fait **maison**.

---

## Plan d'implémentation

### Étape 1 — Fondations : fenêtre et contexte OpenGL

- Initialiser GLFW et créer une fenêtre
- Créer un contexte OpenGL 3.3+ core profile
- Initialiser GLEW
- Callback de resize (`glViewport`)
- Boucle principale : `glfwPollEvents` + `glfwSwapBuffers`
- Makefile fonctionnel avec `-lglfw -lGLEW -lGL`

---

### Étape 2 — Parser .obj

Lire le fichier ligne par ligne, identifier les tokens :

| Token | Signification |
|-------|--------------|
| `v x y z` | Position du sommet |
| `vt u v` | Coordonnée de texture |
| `vn x y z` | Normale |
| `f ...` | Face (triangles, quads ou n-gons) |

- Stocker dans des `std::vector<glm::vec3>` (ou struct maison)
- Formats de face à supporter : `f v`, `f v/vt`, `f v/vt/vn`, `f v//vn`
- **Triangulation** : si la face a plus de 3 sommets → fan triangulation (`v0-v1-v2`, `v0-v2-v3`…)
- **Centrage** : calculer le centre de masse et translater l'objet pour le centrer à l'origine (indispensable pour la rotation autour de l'axe central)

---

### Étape 3 — Upload GPU (VAO / VBO)

- Convertir le mesh en tableau flat de vertices (`position + normal + uv`)
- `glGenVertexArrays` / `glGenBuffers` / `glBufferData`
- Configurer les `glVertexAttribPointer` pour chaque attribut
- Si les normales sont absentes dans le `.obj`, les calculer : `normal = normalize(cross(v1-v0, v2-v0))`

---

### Étape 4 — Matrices maison (sans GLM)

Implémenter une classe `Mat4` avec :

```cpp
Mat4 identity();
Mat4 perspective(float fov, float aspect, float near, float far);
Mat4 translate(float x, float y, float z);
Mat4 rotateX(float angle);
Mat4 rotateY(float angle);
Mat4 rotateZ(float angle);
Mat4 multiply(const Mat4& a, const Mat4& b);
```

Les matrices sont envoyées au shader via `glUniformMatrix4fv`.  
L'ordre de multiplication pour le MVP : `MVP = Projection * View * Model`.

> **Attention** : la rotation doit se faire **après** avoir translaté l'objet à l'origine (`T(-centroid)`), puis retranslaté si besoin. Sinon la rotation tourne autour d'un bord.

---

### Étape 5 — Shaders GLSL

Charger les fichiers `.glsl` avec `fopen` et passer la source à `glShaderSource`.  
Vérifier les erreurs avec `glGetShaderInfoLog`.

**Vertex shader** :
```glsl
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 vNormal;
out vec2 vTexCoord;
```

**Fragment shader — transition couleur ↔ texture** :
```glsl
uniform sampler2D uTexture;
uniform float uBlend;   // 0.0 = couleur, 1.0 = texture

vec4 colorMode = vec4(faceColor, 1.0);  // niveaux de gris via la normale
vec4 texMode   = texture(uTexture, vTexCoord);
fragColor      = mix(colorMode, texMode, uBlend);
```

`uBlend` est incrémenté/décrémenté progressivement dans la boucle principale lors de l'appui sur la touche dédiée.

---

### Étape 6 — Rotation automatique + contrôles clavier

| Touche | Action |
|--------|--------|
| `←` `→` `↑` `↓` | Translation X / Y |
| `PgUp` `PgDn` | Translation Z |
| `T` | Toggle texture (transition douce) |
| `Échap` | Quitter |

La rotation sur Y s'incrémente automatiquement à chaque frame (`angle += deltaTime * speed`).

---

### Étape 7 — Chargeur de texture maison

Implémenter un lecteur **BMP minimal** (header 54 octets, 24 bits/pixel) ou **PPM** (format texte, plus simple) :

```cpp
GLuint loadBMP(const char* path);
// → lit les pixels, appelle glTexImage2D
```

Ne pas utiliser `stb_image` ni toute autre lib externe.

---

### Étape 8 — Couleurs des facettes (niveaux de gris)

Calculer l'éclairage Lambert dans le fragment shader :

```glsl
float brightness = max(dot(normalize(vNormal), lightDir), 0.0);
vec3  faceColor  = vec3(brightness);  // niveaux de gris
```

Pour les normales plates (face shading), utiliser `flat` en qualificateur GLSL ou passer la normale de face comme attribut.

---

## Bonus

### Bonus du sujet

- **Fichiers .obj ambigus / non coplanaires / concaves** : remplacer le fan triangulation par l'algorithme **ear-clipping** pour les polygones concaves
- **Texture sans étirement** : utiliser les coordonnées `vt` du `.obj` sans modification, ou implémenter une projection UV sphérique si elles sont absentes

### Bonus supplémentaires recommandés

#### Rendu

- **Phong shading** : modèle ambiant + diffus + spéculaire avec une source de lumière mobile (ex: touche `L` pour déplacer la lumière)
- **Normal mapping** : charger une texture de normales et perturber les normales dans le fragment shader
- **Wireframe mode** : toggle avec `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` pour afficher les arêtes
- **Face culling** : activer `glEnable(GL_CULL_FACE)` et permettre de le désactiver pour les objets non watertight

#### Interaction

- **Rotation libre à la souris** (arcball rotation) : cliquer-glisser pour faire tourner l'objet librement en 3D
- **Zoom à la molette** : callback `glfwSetScrollCallback` pour modifier le FOV ou la distance caméra
- **Chargement dynamique** : passer le chemin `.obj` en argument, et/ou recharger un autre objet à chaud avec une touche

#### Technique

- **Support `.mtl`** : parser les fichiers de matériaux liés au `.obj` pour récupérer les couleurs diffuses (`Kd`) et les chemins de texture (`map_Kd`)
- **Multi-objet** : afficher plusieurs meshes simultanément avec leurs propres matrices model
- **Frustum culling** basique : ne pas dessiner les objets hors du champ de vision

#### Divers

- **Screenshot BMP** : sauvegarder le framebuffer dans un fichier `.bmp` maison avec une touche dédiée (`F1`)
- **FPS counter** : afficher le nombre d'images par seconde dans le titre de la fenêtre via `glfwSetWindowTitle`
- **Mode plein écran** : toggle entre fenêtré et plein écran avec `Alt+Entrée`

---

## Points de vigilance

- La rotation **doit** se faire autour du centre de masse, pas de l'origine → penser à la translation avant/après rotation
- Les shaders sont compilés **au runtime** depuis les fichiers `.glsl` → toujours vérifier `glGetShaderiv(shader, GL_COMPILE_STATUS, ...)`
- Les matrices sont en **column-major** en OpenGL → ne pas transposer à tort dans `glUniformMatrix4fv` (3e param `GL_FALSE` si la matrice est déjà column-major)
- BMP est stocké **BGR** et non RGB, et les lignes sont souvent inversées → gérer le flip vertical lors du chargement
- Pour la transition couleur/texture, incrémenter `uBlend` dans la boucle avec un `deltaTime` pour que la vitesse soit indépendante du framerate
