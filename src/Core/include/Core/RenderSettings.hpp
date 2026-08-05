#ifndef RENDER_SETTINGS_H
#define RENDER_SETTINGS_H

/**
 * @file RenderSettings.hpp
 * @brief Définition des paramètres de configuration du rendu graphique.
 */

#include "Math/Color.hpp"

namespace nut 
{
    /**
     * @addtogroup Core_Module
     * @{
     */

    /**
     * @brief Structure de configuration (Blueprint) pour l'initialisation du Renderer.
     * * Contient les paramètres globaux appliqués au contexte de rendu graphique 
     * (SDL_Renderer, OpenGL, Vulkan, etc.) lors de sa création par le moteur.
     */
    struct RenderSettings
    {
        /** * @brief Couleur de fond par défaut de l'application (Clear Color).
         * * Cette couleur est utilisée par le Renderer pour effacer (nettoyer) l'écran 
         * au tout début de chaque nouvelle frame, avant de dessiner les entités.
         * * Par défaut : Noir totalement opaque (R:0, G:0, B:0, A:255).
         */
        math::Color backColor{0, 0, 0, 255};
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif