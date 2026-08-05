#ifndef RENDERER_SDL_H
#define RENDERER_SDL_H

/**
 * @file RendererSDL.hpp
 * @internal
 * @brief Implémentation du backend graphique basé sur l'API SDL2.
 * @warning Ce fichier est strictement interne. Il est instancié et géré 
 * dynamiquement par le gestionnaire de Renderer principal.
 */

#include "IRenderer.hpp"
#include "Math/Color.hpp"

struct SDL_Renderer;

namespace nut
{
    class Window;
    struct RenderSettings;

    /**
     * @addtogroup Core_Module
     * @{
     */

    /**
     * @internal
     * @brief Backend de rendu matériel utilisant la bibliothèque logicielle/matérielle SDL2.
     * * Implémente le contrat `IRenderer` et encapsule les appels natifs à la librairie SDL.
     */
    class RendererSDL : public IRenderer
    {
        /// @name État Interne
        /// @{
        private:
            /** @brief Pointeur natif vers le contexte de rendu SDL (Créé via SDL_CreateRenderer). */
            SDL_Renderer* renderer = nullptr;
            
            /** @brief Cache de la couleur de fond pour éviter de la redemander à chaque frame. */
            math::Color backColor;
        /// @}

        public:
            /**
             * @brief Constructeur par défaut.
             */
            RendererSDL() {}
            
            /**
             * @brief Destructeur. 
             * * Se charge d'appeler `SDL_DestroyRenderer` pour libérer la mémoire vidéo.
             */
            ~RendererSDL() override;

            /**
             * @copydoc IRenderer::init
             */
            void init(Window& window, const RenderSettings& settings) override;
            
            /**
             * @copydoc IRenderer::clear
             */
            void clear() override;
            
            /**
             * @copydoc IRenderer::print
             */
            void print() override;
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif