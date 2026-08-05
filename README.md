# NutEngine
- [Prérequis](#prerequis)
- [Comment modifier NutEngine](#comment-modifier-nutengine)

## Prérequis 
### package manager
On utilise vcpkg pour gérer les dépendences (mode manifest).

D'abord cloner:
```console
git clone https://github.com/microsoft/vcpkg.git
```

Pour windows lancer `bootstrap-vcpkg.bat`:
```console
cd vcpkg && bootstrap-vcpkg.bat
```

Ajouter la racine de vcpkg à `PATH` dans les variables environnement système.

Pour plus d'information : `https://learn.microsoft.com/fr-ch/vcpkg/`

### build system
CMake pour le build.

Pour plus d'informations: `https://cmake.org/getting-started/`

### code style
Pour la cohérence du style du code utiliser clang-format.

## Comment modifier NutEngine
### configurer cmake
Pour que cmake puisse collaborer avec vcpkg, il faut indiquer le chemin à la racine de vcpkg dans un fichier nommé `CMakeUserPresets.json` qui ressemble à:
```json
{
  "version": 2,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "vcpkg",
      "environment": {
        "VCPKG_ROOT": "<path to vcpkg>"
      }
    }
  ]
}
```

### ajouter des dépendences
La commande:
```console
vcpkg add port [package]
```
sert à ajouter des dépendences au manifest. Par exemple:
```console
vcpkg add port sdl3
```

On peut voir les dependences dans le fichier `vcpkg.json`.

### Compiler avec CMake
Pour générer un build avec un preset, prenons ici le preset default, on utilise la commande :
```console
cmake --preset default
```
Maintenant on peut construire ce build avec la commande :
```console
cmake --build build
```

L'executable créé est dans `./build/debug`.