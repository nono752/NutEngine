# Philosophie du Moteur {#philosophie_du_moteur}
Ce moteur se veut simple, modulaire et puissant. Il constitue une base modifiable en fonction du type de projet ciblé, tout en garantissant des performances optimales.

## Simple
La simplicité doit se retrouver tant dans ses fonctionnalités que dans son interface.

Le moteur fournit le strict nécessaire pour développer un jeu :
- Gestion du rendu et de la fenêtre
- Gestion des données
- Gestion des entrées (inputs)
- Gestion de la logique

Il ne fait rien de plus que ces quatre piliers et ne force aucune structure de jeu spécifique. D'autre part, il propose une interface lisible et facile à prendre en main, avec un nombre volontairement limité de fonctionnalités.

## Modulaire
L'objectif est de permettre à l'utilisateur de façonner le moteur en fonction de ses besoins spécifiques. L'ajout de modules doit être rapide et ne doit en aucun cas nécessiter la modification du fonctionnement interne de base. 

Dans cette optique, le moteur fournit également quelques modules facultatifs qui vont au-delà de sa fonction initiale.

## Puissant
Basé sur une architecture ECS, le moteur est optimisé pour le matériel afin que la lecture et l'écriture de milliers de données à chaque frame ne soient jamais un goulot d'étranglement. C'est ici que réside le véritable défi technique : proposer un système interne complexe, entièrement dédié aux performances, qui reste invisible pour l'utilisateur, offrant ainsi une simplicité externe apparente.