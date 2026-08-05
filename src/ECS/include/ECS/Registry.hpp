#ifndef REGISTRY_H
#define REGISTRY_H

/**
 * @file Registry.hpp
 * @brief Déclaration et définition de la classe Registry pour le moteur ECS.
 */

#include <vector>
#include <memory>
#include "ECS/internal/SparseSet.hpp"
#include "ECS/View.hpp"
#include "ECS/EcsTypes.hpp"

namespace nut
{
    class Entity;

    /**
     * @defgroup ECS_Module Module ECS
     * @brief Ensemble des briques constituant le cœur de l'architecture Entity-Component-System.
     * @{
     */

    /**
     * @brief Le gestionnaire central de l'architecture ECS (Entity-Component-System).
     * * Le Registre est responsable de la création, de la destruction et du cycle de vie
     * des entités. Il gère également le stockage des composants en mémoire via des SparseSets
     * indépendants et fournit l'interface pour lier les composants aux entités.
     */
    class Registry
    {
        friend class Entity;
        
        private:
            /**
             * @internal
             * @name Stockage et État Interne
             * @{
             */

            /**
             * @brief Tableau associant chaque identifiant d'entité (EntityId) à sa version actuelle.
             * @internal
             */
            std::vector<EntityVersion> entityVersions;

            /**
             * @brief Pile des identifiants (EntityId) d'entités détruites, prêts à être réutilisés.
             * @internal
             */
            std::vector<EntityId> freeIds;

            /**
             * @brief Compteur incrémental pour l'attribution de nouveaux identifiants d'entités.
             * @internal
             */
            EntityId idCounter = 0;

            /**
             * @brief Tableau conteneur polymorphe des SparseSets de composants, indexés par ComponentId.
             * @internal
             */
            std::vector<std::unique_ptr<internal::ISparseSet>> components;

            /** @} */

        private:
            /**
             * @internal
             * @name Méthodes Utilitaires Internes
             * @{
             */

            /**
             * @brief Génère un nouvel identifiant unique séquentiel pour chaque type de composant enregistré.
             * @internal
             * @return EntityId L'identifiant attribué au prochain type de composant.
             */
            static EntityId getNextComponentId();

            /**
             * @brief Récupère (ou instancie dynamiquement si inexistant) le SparseSet associé au type T.
             * @internal
             * @tparam T Le type du composant.
             * @return internal::SparseSet<T>& Une référence vers le SparseSet du type T.
             */
            template <typename T> internal::SparseSet<T>& getSparseSet();
        
            /** @} */

        public:
            /**
             * @name Gestion du Cycle de Vie des Entités
             * @{
             */

            /**
             * @brief Crée une nouvelle entité dans le registre.
             * * Gère automatiquement le recyclage des identifiants (ID) d'anciennes entités 
             * détruites pour optimiser l'utilisation de la mémoire.
             * * @return Entity L'objet représentant la nouvelle entité créée.
             */
            Entity createEntity();

            /**
             * @brief Détruit une entité et supprime tous ses composants associés.
             * * @param entity Le handle (Entity) de l'entité à détruire.
             */
            void destroyEntity(Entity entity);

            /**
             * @brief Détruit une entité via son identifiant brut.
             * * @param id L'identifiant (EntityId) de l'entité à détruire.
             */
            void destroyEntity(EntityId id);

            /**
             * @brief Vérifie si un handle d'entité est toujours valide.
             * * Compare la version stockée dans le handle avec la version actuelle dans le registre
             * pour éviter les pointeurs fantômes (dangling pointers).
             * * @param entity Le handle de l'entité à vérifier.
             * @return true Si l'entité est en vie et valide.
             * @return false Si l'entité a été détruite ou recyclée.
             */
            bool isValid(Entity entity) const;

            /**
             * @brief Vérifie si un identifiant d'entité pointe vers une entité active.
             * * @param id L'identifiant (EntityId) à vérifier.
             * @return true Si l'identifiant correspond à une entité en vie.
             * @return false Si l'identifiant est inactif (détruit).
             */
            bool isValid(EntityId id) const;

            /**
             * @brief Récupère la version actuelle d'un identifiant d'entité.
             * * @param id L'identifiant de l'entité.
             * @return EntityVersion La version actuelle de cet identifiant en mémoire.
             */
            EntityVersion getVersion(EntityId id) const
            {
                return entityVersions[id];
            }

            /**
             * @brief Vide intégralement le registre de tous ses composants.
             * * Ne détruit pas les tableaux, mais efface l'intégralité de leur contenu.
             */
            void clearAll();

            /** @} */

            /**
             * @name Vues et Requêtes (Queries)
             * @{
             */

            /**
             * @brief Crée une vue (View) pour itérer sur les entités possédant certains composants.
             * * @tparam ComponentTypes Les types de composants requis.
             * @return View<ComponentTypes...> Une vue optimisée (O(N) sur le plus petit tableau) 
             * permettant de parcourir les composants demandés.
             */
            template<typename... ComponentTypes> View<ComponentTypes...> view()
            {
                return View<ComponentTypes...>(this, &getSparseSet<ComponentTypes>()...);
            }  

            /** @} */

            /**
             * @name Gestion des Composants
             * @{
             */

            /**
             * @brief Récupère l'identifiant unique (statique) associé à un type de composant.
             * * @tparam T Le type du composant.
             * @return ComponentId L'identifiant unique assigné à ce type.
             */
            template <typename T> static ComponentId getComponentId()
            {
                static const ComponentId id = getNextComponentId();
                return id;
            }

            /**
             * @brief Construit et ajoute un composant à une entité (perfect forwarding).
             * * @tparam T Le type du composant à ajouter.
             * @tparam Args Les types des arguments du constructeur du composant.
             * @param id L'identifiant de l'entité cible.
             * @param args Les arguments transmis au constructeur du composant.
             * @return T& Une référence vers le composant fraîchement créé.
             */
            template <typename T, typename... Args> T& addComponent(EntityId id, Args&&... args)
            {
                return getSparseSet<T>().emplace(id, std::forward<Args>(args)...);
            }

            /**
             * @brief Ajoute une copie d'un composant existant à une entité.
             * * @tparam T Le type du composant.
             * @param id L'identifiant de l'entité cible.
             * @param component Le composant à copier.
             * @return T& Une référence vers le composant inséré.
             */
            template <typename T> T& addComponent(EntityId id, const T& component)
            {
                return getSparseSet<T>().insert(id, component);
            }

            /**
             * @brief Supprime toutes les instances d'un type de composant spécifique.
             * * @tparam T Le type du composant dont le SparseSet doit être vidé.
             */
            template <typename T> void clearComponent()
            {
                getSparseSet<T>().clear();
            }

            /**
             * @brief Retire un composant spécifique d'une entité.
             * * @tparam T Le type du composant à retirer.
             * @param id L'identifiant de l'entité.
             */
            template <typename T> void removeComponent(EntityId id)
            {
                getSparseSet<T>().erase(id);
            }

            /**
             * @brief Vérifie si une entité possède un composant donné.
             * * @tparam T Le type du composant à vérifier.
             * @param id L'identifiant de l'entité.
             * @return true Si l'entité possède le composant.
             * @return false Si l'entité ne possède pas le composant.
             */
            template <typename T> bool hasComponent(EntityId id)
            {
                return getSparseSet<T>().contains(id);
            }

            /**
             * @brief Récupère une référence vers le composant d'une entité.
             * * @warning L'entité DOIT posséder ce composant avant l'appel (comportement indéfini sinon).
             * * @tparam T Le type du composant.
             * @param id L'identifiant de l'entité.
             * @return T& Une référence modifiable vers le composant.
             */
            template <typename T> T& getComponent(EntityId id)
            {
                return getSparseSet<T>().get(id);
            }

            /** @} */

            /**
             * @name Gestion de la Mémoire et Pré-allocation
             * @{
             */

            /**
             * @brief Prédéfinit la capacité mémoire du SparseSet d'un composant spécifique.
             * * Permet d'éviter les réallocations coûteuses lors de la création massive d'entités.
             * * @tparam T Le type du composant ciblé.
             * @param capacity Le nombre d'éléments à réserver en mémoire.
             */
            template <typename T> void reserve(size_t capacity)
            {
                getSparseSet<T>().reserve(capacity); // compiler will use the non-virtual version of reserve
            }

            /**
             * @brief Prédéfinit la capacité mémoire pour tous les SparseSets actuellement actifs.
             * * @param capacity Le nombre d'éléments à réserver par défaut.
             */
            void reserveAll(size_t capacity);

            /** @} */
    };

    /** @} */ // Fin du groupe ECS_Module
    
    template <typename T>
    internal::SparseSet<T>& Registry::getSparseSet() 
    {
        ComponentId componentId = getComponentId<T>();

        if (componentId >= components.size())
            components.resize(componentId + 1);

        if (!components[componentId])
            components[componentId] = std::make_unique<internal::SparseSet<T>>();

        return static_cast<internal::SparseSet<T>&>(*components[componentId]);
    }
}

#include "ECS/internal/Entity.inl"
#include "ECS/internal/View.inl"

#endif