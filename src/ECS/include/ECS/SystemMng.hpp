#ifndef SYSTEM_MNG_H
#define SYSTEM_MNG_H

#include <vector>
#include <memory>
#include <concepts>
#include <cassert>
#include <utility>
#include "ISystem.hpp"
#include "EcsTypes.hpp"

/**
 * @file SystemMng.hpp
 * @brief Definition of SystemMng class
 */

namespace nut
{
    class Registry;
    /**
     * @addtogroup ECS_Module
     * @{
     */

    /** * @brief Manager qui gère les systems dérivés de ISystem. */
    class SystemMng
    {
        private:
            /** * @brief Couche regroupant des systèmes. */
            struct Layer
            {
                /** * @brief True si le layer doit etre update. */
                bool enabled = true;
                /** * @brief Les systems du layer. */
                std::vector<std::shared_ptr<ISystem>> systems;
            };
            
            /**  
             * @brief Adaptateur interne permettant d'encapsuler une fonction ou une lambda dans un ISystem.
             * @tparam Func Le type de l'objet appelable.
             * @warning Les fonctions utilisant LambdaSystem doivent garantir que Func est 
             * callable avec (Registry& registry, float dt), et débarrassé de ses références et qualificatifs const.
             */
            template <typename Func>
            class LambdaSystem final : public ISystem
            {
                public:
                    /**
                     * @brief Construit un LambdaSystem en prenant possession de l'objet appelable.
                     * @param fn La fonction ou lambda à exécuter lors de la mise à jour.
                     */
                    explicit LambdaSystem(Func&& fn) : callback(std::move(fn)) {}

                    /** * @brief Exécute la fonction encapsulée. */
                    void update(Registry& registry, float dt) override
                    {
                        callback(registry, dt);
                    }
                private:
                    /** * @brief stoque la fonction. */
                    Func callback;
            };

            /** * @brief Les layers. */
            std::vector<Layer> layers;
        
        public:
            /** * @brief Crée layer 0 pour construction par défaut. */
            SystemMng() { createLayer(); }

            /**
             * @brief Crée un nouveau layer.
             * @return L'id du nouveau layer.
             */
            SystemLayerId createLayer()
            {
                layers.emplace_back();
                return layers.size() - 1;
            }

            /**
             * @brief Active ou désactive un layer.
             * @param layer L'id du layer a activer/désactiver.
             * @param enabled true pour activer false sinon.
             */
            void setLayerEnabled(SystemLayerId layer, bool enabled)
            {
                assert(layer < layers.size() && "Error::setLayerEnabled: invalid layer id.");
                layers[layer].enabled = enabled;
            }

            /** * @brief Retourne true si actif. */
            bool isLayerEnabled(SystemLayerId layer) const
            {
                assert(layer < layers.size() && "Error::isLayerEnabled: invalid layer id.");
                return layers[layer].enabled;
            }

            /** * @brief Retourne le nombre total de layer créés. */
            SystemLayerId getLayerCount() const
            {
                return layers.size();
            }

            /**
             * @brief Ajoute un système existant (pouvant être partagé entre plusieurs scènes) dans un layer.
             * @param system Pointeur partagé vers le système.
             * @param layer L'id du layer (0 par défaut).
             * @return Pointeur brut typé non-propriétaire vers le système.
             * 
             * @warning L'instance est partagée par référence : toute modification de ses attributs 
             * ou de son état (y compris @ref ISystem::setEnabled) affectera toutes les scènes 
             * qui l'utilisent. Utilisez @ref setLayerEnabled pour activer ou désactiver un comportement 
             * localement à une scène.
             */
            template <std::derived_from<ISystem> T>
            T* addSystem(const std::shared_ptr<T>& system, SystemLayerId layer = 0)
            {
                assert(layer < layers.size() && "Error::addSystem: invalid layer id.");
                assert(system != nullptr && "Error::addSystem: system cannot be nullptr.");
                layers[layer].systems.push_back(system);
                return system.get();
            }
            
            /**
             * @brief Instancie et ajoute un nouveau système dans un layer spécifique.
             * @tparam T Type dérivé de ISystem.
             * @param layer L'id du layer cible.
             * @param args Arguments transmis au constructeur de T.
             * @return Pointeur brut typé non-propriétaire vers le système créé.
             */
            template <std::derived_from<ISystem> T, typename... Args>
            requires std::constructible_from<T, Args...>
            T* addSystem(SystemLayerId layer, Args&&... args)
            {
                assert(layer < layers.size() && "Error::addSystem: invalid layer id.");
                std::shared_ptr<T> system = std::make_shared<T>(std::forward<Args>(args)...);
                T* rawSys = system.get();
                layers[layer].systems.push_back(std::move(system));
                return rawSys;
            }

            /**
             * @brief Instancie et ajoute un nouveau système dans le layer 0 par défaut.
             * @tparam T Type dérivé de ISystem.
             * @param args Arguments transmis au constructeur de T.
             * @return Pointeur brut typé non-propriétaire vers le système créé.
             */
            template <std::derived_from<ISystem> T, typename... Args>
            requires std::constructible_from<T, Args...>
            T* addSystem(Args&&... args)
            {
                return addSystem<T>(0, std::forward<Args>(args)...);
            }

            /**
             * @brief Crée et ajoute un système à la volée à partir d'une fonction ou d'une lambda dans un layer spécifique.
             * @tparam Func Type de l'objet appelable (déduit automatiquement, doit accepter `(Registry&, float)`).
             * @param layer L'identifiant du layer cible.
             * @param fn La fonction ou lambda à exécuter lors de l'appel à @ref update.
             * @return ISystem* Pointeur brut non-propriétaire vers le système créé.
             */
            template <typename Func>
            requires std::invocable<Func, Registry&, float>
            ISystem* addSystem(SystemLayerId layer, Func&& fn)
            {
                assert(layer < layers.size() && "Error::addSystem: invalid layer id.");
                auto system = std::make_shared<LambdaSystem<std::decay_t<Func>>>(std::forward<Func>(fn));
                ISystem* rawSys = system.get();
                layers[layer].systems.push_back(std::move(system));
                return rawSys;
            }
        
            /**
             * @brief Crée et ajoute un système à la volée à partir d'une fonction ou d'une lambda dans le layer par défaut (0).
             * @tparam Func Type de l'objet appelable (déduit automatiquement, doit accepter `(Registry&, float)`).
             * @param fn La fonction ou lambda à exécuter lors de l'appel à @ref update.
             * @return ISystem* Pointeur brut non-propriétaire vers le système créé.
             */
            template <typename Func>
            requires std::invocable<Func, Registry&, float>
            ISystem* addSystem(Func&& fn)
            {
                return addSystem(0, std::forward<Func>(fn));
            }

            /**
             * @brief Met à jour les layers et systems actifs dans l'ordre des layers.
             * @param registry Le registry a mettre à jour avec les systems du systemMng.
             * @param dt Temps écoulé depuis la dernière frame.
             */
            void update(Registry& registry, float dt)
            {
                for (Layer& layer : layers)
                {
                    if (!layer.enabled) continue;

                    for (std::shared_ptr<ISystem>& system : layer.systems)
                        if (system && system->isEnabled())
                            system->update(registry, dt);
                }
            }
    };

    /**
     * @brief Crée un System et retourne le type adapté pour utilisation addSystem.
     * @tparam T Type dérivé de ISystem, la classe du system à créer.
     * @param args Arguments transmis au constructeur de T.
     * @return std::shared_ptr<T>, le type attendu dans une surcharge de addSystem.
     */
    template <std::derived_from<ISystem> T, typename... Args>
    std::shared_ptr<T> makeSystem(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /** @} */ // ECS_Module
}
#endif