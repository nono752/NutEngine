#ifndef ISYSTEM_H
#define ISYSTEM_H

/**
 * @file ISystem.hpp
 * @brief Définition de la classe de base ISystem.
 */

namespace nut
{
    class Registry;

    /**
     * @addtogroup ECS_Module
     * @{
     */

    /**
     * @brief Interface de base pour tous les systèmes de l'ECS.
     */
    class ISystem
    {
        public:
            virtual ~ISystem() = default;

            /**
             * @brief Méthode appelée à chaque frame pour mettre à jour la logique du système.
             * @param registry Le registre contenant toutes les entités et composants.
             * @param dt delta time (temps écoulé depuis la dernière frame).
             */
            virtual void update(Registry& registry, float dt) = 0;

            /**
             * @brief Active ou désactive l'exécution de ce système.
             */
            void setEnabled(bool enable) { enabled = enable; }

            /**
             * @brief Vérifie si le système est actuellement actif.
             */
            bool isEnabled() const { return enabled; }

        private:
            /**
             * @brief True si le système doit être mis à jour
             */
            bool enabled = true;
    };

    /** @} */ // Fin du groupe ECS_Module
}

#endif