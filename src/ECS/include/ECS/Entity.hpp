#ifndef ENTITY_H
#define ENTITY_H

/**
 * @file Entity.hpp
 * @brief Définition de la classe Entity, un handle orienté objet pour le système ECS.
 */

#include "ECS/EcsTypes.hpp"
#include <limits>
#include <cassert>

namespace nut
{
    class Registry;
    template<typename... Components> class View;

    /**
     * @addtogroup ECS_Module
     * @{
     */

    /**
     * @brief Un handle léger (Wrapper) pour interagir facilement avec une entité.
     * * Contrairement à la Programmation Orientée Objet classique, la classe `Entity` ne 
     * stocke **aucune** donnée de composant. Elle ne contient qu'un identifiant, une version 
     * et un pointeur vers le Registre. 
     * * Son seul but est d'offrir une syntaxe élégante et intuitive (ex: `entity.addComponent<Position>()`) 
     * tout en conservant les performances extrêmes de l'architecture Orientée Données (DOD).
     * * @warning Ce handle peut devenir invalide si l'entité est détruite dans le registre. 
     * Vérifiez toujours sa validité avec `isValid()` ou `if (entity)` avant de l'utiliser.
     */
    class Entity
    {
        friend class Registry;
        template<typename... Components> friend class View;

        /// @internal @name État Interne
        /// @{
        private:
            /** @brief Pointeur vers le registre propriétaire de l'entité. @internal */
            Registry* owner;
            /** @brief L'identifiant brut (index) de l'entité en mémoire. @internal */
            EntityId id = 0;
            /** @brief La version de l'entité au moment de la création de ce handle. @internal */
            EntityVersion version = 0;
        
        private:
            /**
             * @internal
             * @brief Constructeur privé réservé au Registre et aux Vues.
             * @param id L'identifiant assigné par le Registre.
             * @param version La version actuelle de cet identifiant.
             * @param registry Le registre qui gère cette entité.
             */
            Entity(EntityId id, EntityVersion version, Registry* registry);
        /// @}

        public:
            /**
             * @name Cycle de vie et Validité
             * @{
             */

            /**
             * @brief Constructeur par défaut. Crée un handle d'entité "Nul" et invalide.
             */
            Entity() : id(NULL_ENTITY_ID), version(NULL_ENTITY_VERSION), owner(nullptr) {}

            /**
             * @brief Détruit l'entité dans le registre et supprime tous ses composants.
             * * @note Après cet appel, ce handle deviendra invalide.
             */
            void destroy();

            /**
             * @brief Récupère l'identifiant brut de l'entité.
             * @return EntityId L'identifiant de l'entité.
             */
            EntityId getId() const
            {
                return id;
            }

            /**
             * @brief Récupère la version de l'entité stockée dans ce handle.
             * @return EntityVersion La version de l'entité.
             */
            EntityVersion getVersion() const
            {
                return version;
            }

            /**
             * @brief Vérifie si l'entité est toujours en vie dans le registre.
             * * Compare la version de ce handle avec la version actuelle dans le registre
             * pour éviter les comportements indéfinis (Dangling Pointers).
             * * @return true Si l'entité est valide et active.
             * @return false Si l'entité a été détruite.
             */
            bool isValid() const;
            
            /**
             * @brief Surcharge de l'opérateur booléen pour un test de validité rapide.
             * * @par Exemple :
             * @code
             * if (myEntity) { myEntity.destroy(); }
             * @endcode
             * * @return true Si l'entité est valide.
             */
            explicit operator bool() const;

            /**
             * @brief Vérifie si deux handles pointent vers la même entité (même ID, même version, même registre).
             */
            bool operator==(const Entity& other) const;

            /**
             * @brief Vérifie si deux handles pointent vers des entités différentes.
             */
            bool operator!=(const Entity& other) const;

            /** @} */

            /**
             * @name Manipulation des Composants
             * @brief Ces méthodes transfèrent directement l'appel au Registre.
             * @{
             */

            /**
             * @brief Ajoute une copie d'un composant existant à cette entité.
             * @tparam T Le type du composant.
             * @param component L'instance du composant à copier.
             * @return decltype(auto) Une référence vers le composant ajouté.
             */
            template<typename T> decltype(auto) addComponent(const T& component);

            /**
             * @brief Construit un composant en place (Perfect Forwarding) et l'ajoute à l'entité.
             * @tparam T Le type du composant à créer.
             * @tparam Args Les types des arguments du constructeur.
             * @param args Les arguments à passer au constructeur du composant.
             * @return decltype(auto) Une référence vers le composant créé.
             */
            template<typename T, typename... Args> decltype(auto) addComponent(Args&&... args);

            /**
             * @brief Récupère une référence vers le composant de l'entité.
             * @warning Assurez-vous que l'entité possède ce composant (via `hasComponent`) avant d'appeler cette méthode.
             * @tparam T Le type du composant souhaité.
             * @return T& Une référence modifiable vers le composant.
             */
            template<typename T> T& getComponent();

            /**
             * @brief Vérifie si l'entité possède un type de composant spécifique.
             * @tparam T Le type du composant à vérifier.
             * @return true Si l'entité possède le composant.
             */
            template<typename T> bool hasComponent();

            /**
             * @brief Retire un composant de l'entité.
             * @tparam T Le type du composant à retirer.
             */
            template<typename T> void removeComponent();

            /** @} */
    };

    /** @} */ // Fin du groupe ECS_Module
}

#endif