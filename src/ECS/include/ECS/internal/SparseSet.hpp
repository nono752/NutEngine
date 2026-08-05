#ifndef SPARSE_SET_H
#define SPARSE_SET_H

/**
 * @file SparseSet.hpp
 * @internal
 * @brief Structure de données bas niveau (Tableau Creux) pour le stockage ECS.
 * @warning Ce fichier fait partie de la machinerie interne du moteur. 
 * Il ne doit jamais être inclus ou manipulé directement par l'utilisateur final.
 */

#include "ECS/EcsTypes.hpp"
#include <vector>
#include <limits>
#include <cassert>
#include <utility>

namespace nut
{
    template<typename... Components> class View;
}

/**
 * @internal
 * @namespace nut::internal
 * @brief Espace de noms masqué contenant l'implémentation bas niveau du moteur.
 */
namespace nut::internal
{
    /**
     * @addtogroup ECS_Module
     * @{
     */

    /**
     * @internal
     * @brief Interface polymorphe (Type Erasure) pour le Registre.
     * * Permet au `Registry` de stocker une collection de SparseSets contenant des 
     * types complètement différents dans un seul `std::vector<std::unique_ptr<ISparseSet>>`, 
     * et de les manipuler (suppression, nettoyage) sans connaître le type sous-jacent.
     */
    struct ISparseSet
    {
        virtual ~ISparseSet() = default;

        /** @brief Supprime à l'aveugle l'entité du tableau. */
        virtual void erase(EntityId entity) = 0; // Could be not virtual if called a lot
        
        /** @brief Vide la mémoire du tableau sans la désallouer. */
        virtual void clear() = 0;
        
        /** @brief Pré-alloue la mémoire pour optimiser les performances. */
        virtual void reserve(size_t capacity) = 0;
    };

    struct SparseSetTester;

    /**
     * @internal
     * @brief Conteneur mémoire ultra-rapide garantissant la contiguïté des données (Data-Oriented Design).
     * * Assure une lecture des composants et un accès via EntityId en $O(1)$.
     * Maintient les données parfaitement alignées en mémoire RAM pour éviter les "Cache Miss".
     * * @tparam T Le type de composant stocké.
     */
    template <typename T>
    class SparseSet : public ISparseSet
    {
        template<typename... Components> friend class nut::View;
        friend struct SparseSetTester;

        /// @name Tableaux Internes
        /// @{
        private:
            /** @brief Tableau dense contenant les instances réelles des composants. Parfaitement contigu. */
            std::vector<T> denseData;
            /** @brief Tableau dense contenant l'ID des entités (Mapping inverse : Index -> EntityId). */
            std::vector<EntityId> denseId;
            /** @brief Tableau creux servant d'index (Mapping direct : EntityId -> Index). */
            std::vector<size_t> sparse;
        /// @}

        private:
            /**
             * @brief Retourne une référence constante vers le tableau des IDs denses.
             * * Principalement utilisé par la `View` pour itérer sur les entités.
             */
            const std::vector<EntityId>& getDenseId() const noexcept
            {
                return denseId;
            }

        public:
            SparseSet() = default;
            ~SparseSet() override = default;

            /**
             * @name Gestion de la Copie et du Déplacement (Rule of 5)
             * @{
             */

            /** @brief Constructeur par copie supprimé (interdit le clonage des SparseSets). */
            SparseSet(const SparseSet& other) = delete;

            /** @brief Constructeur par déplacement par défaut. */
            SparseSet(SparseSet&& other) noexcept = default;

            /** @brief Opérateur d'affectation par copie supprimé. */
            SparseSet& operator=(const SparseSet& other) = delete;

            /** @brief Opérateur d'affectation par déplacement par défaut. */
            SparseSet& operator=(SparseSet&& other) noexcept = default;

            /** @} */

            /**
             * @brief Vérifie rapidement si une entité possède ce composant.
             * @param entity L'identifiant de l'entité.
             * @return true Si l'entité est indexée et valide dans ce tableau.
             */
            bool contains(EntityId entity) const
            {
                return entity < sparse.size() && sparse[entity] != NULL_ENTITY_ID;
            }

            /**
             * @brief Récupère le composant associé à l'entité.
             * @warning L'entité doit impérativement posséder le composant (assert interne).
             * @param entity L'identifiant de l'entité.
             * @return T& Référence modifiable vers le composant.
             */
            T& get(EntityId entity)
            {
                assert(contains(entity) && "Entity don't have this component");
                return denseData[sparse[entity]];
            }

            /** @brief Retourne le nombre exact de composants stockés. */
            size_t size() const noexcept
            { 
                return denseData.size(); 
            }

            void reserve(size_t capacity) override
            { 
                denseData.reserve(capacity);
                denseId.reserve(capacity);
            }

            void clear() override
            {
                denseData.clear();
                denseId.clear();
                sparse.clear();
            }

            /**
             * @brief Insère un composant existant par copie ou déplacement.
             * @param entity L'entité cible.
             * @param component Le composant à stocker.
             * @return T& Référence vers le composant inséré.
             */
            T& insert(EntityId entity, T component); 

            /**
             * @brief Construit un composant en place pour éviter toute copie (Perfect Forwarding).
             * @tparam Args Types des arguments du constructeur du composant.
             * @param entity L'entité cible.
             * @param args Arguments transférés au constructeur.
             * @return T& Référence vers le composant fraîchement construit.
             */
            template <typename... Args> T& emplace(EntityId entity, Args&&... args);

            /**
             * @brief Supprime le composant d'une entité en maintenant la contiguïté mémoire.
             * * Utilise l'algorithme de "Swap-and-Pop" : O(1).
             * @param entity L'entité dont le composant doit être détruit.
             */
            void erase(EntityId entity) override;
    };

    template<typename T>
    T& SparseSet<T>::insert(EntityId entity, T component)
    {
        if (contains(entity)) 
        {
            denseData[sparse[entity]] = std::move(component);
            return denseData[sparse[entity]];
        }

        if (entity >= sparse.size()) 
            sparse.resize(entity + 1, NULL_ENTITY_ID);

        size_t index = denseData.size();
        sparse[entity] = index;

        denseData.push_back(std::move(component));
        denseId.push_back(entity);

        return denseData.back();
    }

    template <typename T>
    template <typename... Args>
    T& SparseSet<T>::emplace(EntityId entity, Args&&... args) 
    {
        if (contains(entity)) 
        {
            denseData[sparse[entity]] = T(std::forward<Args>(args)...);
            return denseData[sparse[entity]];
        }

        if (entity >= sparse.size())
            sparse.resize(entity + 1, NULL_ENTITY_ID);

        size_t index = denseData.size();
        sparse[entity] = index;

        denseData.emplace_back(std::forward<Args>(args)...);
        denseId.push_back(entity);

        return denseData.back();
    }

    template<typename T>
    void SparseSet<T>::erase(EntityId entity)
    {
        if (!contains(entity)) return;

        size_t removeIndex = sparse[entity];
        size_t lastIndex = denseData.size() - 1;
        EntityId lastId = denseId[lastIndex];

        denseData[removeIndex] = std::move(denseData[lastIndex]);
        denseId[removeIndex] = lastId;

        sparse[lastId] = removeIndex;

        sparse[entity] = NULL_ENTITY_ID;

        denseData.pop_back();
        denseId.pop_back();
    }

    /** @} */ // Fin du groupe ECS_Module
}
#endif