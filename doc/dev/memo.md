# memo {#memo}

## idée direction générale
- cacher SDL c.a.d. l'utilisateur ne doit pas avoir a utiliser/connaitre sdl.
- éviter inclusion sdl dans les headers -> pimpl (temps compilation)
- De ne pas inclure dans header les header qui ne doivent pas etre exposés (normalement la gestion des dépendences cmake devrait causer erreur compilation).
- RAII
- important de tout mettre dans le namespace nut pour eviter colisions.

## Core
- application
- window
- renderer

## fonctionnement général
- window et renderer ne sont pas exposés. Pour cela, ne permet pas creation -> constructeurs privés
- L'utilisateur doit obligatoirement passer par application pour opérer sur window ou renderer.
- window et renderer sont move-only c.a.d. on ne peut pas les copier.
- application ne peut pas être copiée ni move.

## renderer
- Irenderer est une api pour le rendu
- renderer commande cette api
- ainsi on peut créer par exemple rendererSDL qui s'occupe du rendu en sdl et par la suite ajouter pour d'autres lib graphiques.
- remarquer que le choix de la methode se fait dans renderer actuellement. Quand ce sera nécessaire faire : RendererFactory, --> renderer n'a plus besoin de connaitre l'implementation specifique a chaque lib graphique et centralise le code a modifier lorsqu'on ajoute methode rendu.

# ECS
le système data oriented.
- Registry
- SparseSet

## SparseSet
structure de stockage données avec accès et boucle très rapide.

3 tableaux:
- denseData : les données
- denseID : tableau parallèle a densedata qui renvoie a la position dans le sparse
- sparse : toutes les entités ordonnées par id et contient position de dense qui correspond a la donnée de l'entité.

Propriétés:
- trouver la donnée de l'entité par son id simplement avec ```densedata[sparse[id]]```.
- boucler sur les données directements via denseData.
- move-only

## Registry

