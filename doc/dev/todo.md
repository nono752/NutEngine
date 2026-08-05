# todo {#todo}

## maintenant
- La doc doxygen est actuellement générée par IA. Il faut relire enlever le superflus (attention warnings doxygen stricts) et traduire en anglais pour cohérence.
- revoir les commentaires des implementations

## très prochainement
- ajout des systèmes
- pouvoir changer parametre window/renderer via application apres son initialisation.

## prochainement
- meilleur système d'erreur : lib spdlog
- rendererFactory + rendererOpenGL
- lorsque gerer textures penser à faire un gestionnaire en DOD en C pour gérer par identifiants.
- changer sparseset pour que erase ne soit plus virtuelle (utile si bcp d'entités détruites a chque frames)
- gerer fragmentation des sparseset cf benchmarks ECS analyse.
- faire un readme convenable
- finir documentation dev avec conventions et ranger le bordel dans memo et todo
- faire une ebauche de guide/tutoriel

## idée bonus pour la fin si elle arrive
- multi thread géré en C (vitesse et frisson du danger du bas niveau)
- envisager api python pour pouvoir utiliser nutengine avec python
- envisager de créer un langage pour le moteur (nutLang) qui complemente python et si possible qui ait une abstraction pour l'etendre