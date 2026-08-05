#ifndef VIEW_H
#define VIEW_H

/**
 * @file View.hpp
 * @brief Définition de la classe View pour l'itération performante sur les entités.
 */

#include "ECS/internal/SparseSet.hpp"
#include "ECS/Entity.hpp"
#include "ECS/EcsTypes.hpp"
#include <array>
#include <tuple>
#include <utility>

namespace nut
{    
    class Registry;

    /**
     * @addtogroup ECS_Module
     * @{
     */

    /**
     * @brief Un filtre intelligent et ultra-rapide pour itérer sur les entités.
     * * La `View` permet de requêter le Registre pour obtenir instantanément toutes les entités
     * possédant une combinaison spécifique de composants. Elle utilise la "Loi du plus petit" 
     * (itération sur le SparseSet le plus court) pour garantir des performances O(N) optimales.
     * * @tparam ComponentTypes Les types de composants que l'entité doit posséder.
     * * @note La View est un objet temporaire très léger. Elle ne stocke pas de données, 
     * mais uniquement des pointeurs vers les tableaux du Registre. Créez-la à la volée 
     * dans vos Systèmes.
     * * @par Exemple d'utilisation :
     * @code
     * auto view = registry.view<Position, Velocity>();
     * for (auto entityId : view) {
     * // ...
     * }
     * @endcode
     */
    template<typename... ComponentTypes>
    class View
    {
        friend class Registry;

        /// @internal @name Interne
        /// @{
        private:
            /** @brief Pointeur vers le registre. */
            Registry* registry;
            /** @brief Tuples de pointeurs vers les SparseSets requis. */
            std::tuple<internal::SparseSet<ComponentTypes>*...> data;
            /** @brief Pointeur vers le tableau d'entités du plus petit SparseSet. */
            const std::vector<EntityId>* minEntities;


            /**
             * @internal
             * @brief Itérateur de base utilisant le pattern CRTP (Curiously Recurring Template Pattern).
             * * Ce pattern permet d'injecter le type dérivé (`Derived`) directement dans la classe de base 
             * au moment de la compilation. Cela permet de mutualiser la logique complexe d'incrémentation 
             * (le saut d'index avec le `do-while`) sans utiliser de polymorphisme dynamique (pas de `virtual`, 
             * donc performances maximales).
             * @tparam Derived Le type de l'itérateur enfant (ex: IdIterator, ComponentIterator).
             */
            template<typename Derived>
            class BaseIterator 
            {
                protected:
                    /** @brief Pointeur vers la View parente contenant les données. */
                    View* view;
                    /** @brief Index actuel de l'itération dans le plus petit tableau (minEntities). */
                    EntityId index;
                
                protected:
                    /**
                     * @brief Constructeur protégé réservé aux classes dérivées.
                     * @param v Pointeur vers la vue propriétaire.
                     * @param i Index de départ.
                     */
                    BaseIterator(View* v, EntityId i) : view(v), index(i) {}
                
                public:
                    /**
                     * @brief Opérateur de pré-incrémentation.
                     * * Avance dans le tableau `minEntities` jusqu'à trouver une entité 
                     * qui possède bien TOUS les composants requis par la View.
                     * @return Derived& Référence vers l'itérateur dérivé (CRTP).
                     */
                    Derived& operator++() 
                    {
                        do 
                        {
                            ++index;
                        } while (index < view->minEntities->size() && !view->contains((*view->minEntities)[index]));

                        return static_cast<Derived&>(*this);
                    }             

                    /**
                     * @brief Vérifie l'inégalité entre deux itérateurs.
                     * @param other L'autre itérateur à comparer.
                     * @return true Si les index sont différents.
                     */
                    bool operator!=(const BaseIterator& other) const 
                    {
                        return index != other.index;
                    }
            };
        /// @}

        private:
            /**
             * @internal
             * @brief Construit une View. (Appelé uniquement par Registry::view<>())
             */
            View(Registry* r, internal::SparseSet<ComponentTypes>*... dataPack);

        public:
            /**
             * @brief Itérateur par défaut retournant uniquement l'identifiant de l'entité.
             */
            class IdIterator : public BaseIterator<IdIterator>
            {
                friend class View<ComponentTypes...>;

                private:
                    /** @brief Constructeur privé appelé par View::begin() et View::end(). */
                    IdIterator(View* v, EntityId i) : BaseIterator<IdIterator>(v, i) {}
                    
                public:
                    /**
                     * @brief Déréférence l'itérateur.
                     * @return EntityId L'identifiant brut de l'entité actuelle.
                     */
                    EntityId operator*() const 
                    {
                        return (*this->view->minEntities)[this->index]; // current id
                    }
            };

            /**
             * @brief Itérateur retournant un tuple contenant l'ID et les références des composants.
             */
            class ComponentIterator : public BaseIterator<ComponentIterator>
            {
                friend class View<ComponentTypes...>;

                private:
                    /** @brief Constructeur privé appelé par View::components(). */
                    ComponentIterator(View* v, EntityId i): BaseIterator<ComponentIterator>(v, i) {}
                    
                public:
                    /**
                     * @brief Déréférence l'itérateur.
                     * @return std::tuple<EntityId, ComponentTypes&...> Un tuple (ID, Composants...).
                     */
                    std::tuple<EntityId, ComponentTypes&...> operator*() const 
                    {
                        EntityId currentId = (*this->view->minEntities)[this->index];
                        
                        return {
                            this->index, 
                            std::get<internal::SparseSet<ComponentTypes>*>(this->view->data)->get(currentId)...
                        }; // current tuple of components*
                    }
            };

            /**
             * @brief Proxy permettant d'itérer directement sur les composants avec le Structured Binding.
             */
            struct ComponentProxy
            {
                /** @brief Pointeur vers la vue parente. @internal */
                View* view;

                /** @brief Retourne l'itérateur de début pour les composants. */
                ComponentIterator begin() 
                { 
                    return ComponentIterator(view, 0); 
                }
                /** @brief Retourne l'itérateur de fin pour les composants. */
                ComponentIterator end()
                { 
                    return ComponentIterator(view, static_cast<EntityId>(view->minEntities->size())); 
                }
            };
        
            /**
             * @brief Itérateur retournant un objet Entity (Handle).
             */
            class EntityIterator : public BaseIterator<EntityIterator>
            {
                friend class View<ComponentTypes...>;

                private:
                    /** @brief Constructeur privé appelé par View::entities(). */
                    EntityIterator(View* v, EntityId i) : BaseIterator<EntityIterator>(v, i) {}
                    
                public:
                    /**
                     * @brief Déréférence l'itérateur.
                     * @return Entity Un handle sécurisé pointant vers l'entité actuelle.
                     */
                    Entity operator*() const;
            };

            /**
             * @brief Proxy permettant d'itérer en récupérant des objets Entity.
             */
            struct EntityProxy
            {
                /** @brief Pointeur vers la vue parente. @internal */
                View* view;
            
                /** @brief Retourne l'itérateur de début pour les entités. */
                EntityIterator begin()
                {
                    return EntityIterator(view, 0); 
                }
                /** @brief Retourne l'itérateur de fin pour les entités. */
                EntityIterator end()
                {
                    return EntityIterator(view, static_cast<EntityId>(view->minEntities->size())); 
                }
            };
        
        public:
            /**
             * @brief Vérifie si une entité possède bien TOUS les composants requis par cette View.
             * @param id L'identifiant de l'entité à vérifier.
             * @return true Si l'entité satisfait la signature de la View.
             * @return false Sinon.
             */
            bool contains(EntityId id)
            {
                return (std::get<internal::SparseSet<ComponentTypes>*>(data)->contains(id) && ...);
            }
            
            /**
             * @brief Applique une fonction ou une lambda à toutes les entités de la View.
             * @tparam F Le type de la fonction (déduit automatiquement).
             * @param func La fonction à exécuter.
             */
            template<typename F> void each(F func);

            /**
             * @name Itération par défaut (EntityId)
             * @brief Permet d'utiliser la View directement dans une boucle for-range.
             * @{
             */
            /**
             * @brief Retourne un itérateur sur la première entité valide de la View.
             * @return IdIterator L'itérateur de début.
             */
            IdIterator begin() 
            { 
                IdIterator it{this, 0};
                
                if (it.index < minEntities->size() && !contains((*minEntities)[it.index]))
                    ++it;
                
                return it;
            }
            /**
             * @brief Retourne un itérateur sur la première entité valide de la View.
             * @return IdIterator L'itérateur de début.
             */
            IdIterator end() 
            { 
                return {this, static_cast<EntityId>(minEntities->size())}; 
            }
            /// @}

            /**
             * @brief Itère en retournant un tuple contenant l'ID et les composants.
             * * Idéal pour le Structured Binding (C++17) :
             * `for (auto [entity, pos, vel] : view.components()) { ... }`
             * * @return ComponentProxy Un objet intermédiaire compatible for-range.
             */
            ComponentProxy components() 
            {
                return ComponentProxy{this}; 
            }

            /**
             * @brief Itère en retournant des Handles `Entity` orientés objet.
             * * @return EntityProxy Un objet intermédiaire compatible for-range.
             */
            EntityProxy entities() 
            { 
                return EntityProxy{this}; 
            }
    };

    /** @} */ // Fin du groupe ecs_module
}

#endif