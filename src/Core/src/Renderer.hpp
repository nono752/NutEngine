#ifndef RENDERER_H
#define RENDERER_H

/**
 * @file Renderer.hpp
 * @internal
 * @brief Définition du gestionnaire de rendu principal (Façade pour IRenderer).
 * @warning Ce fichier est strictement interne au moteur. L'utilisateur final ne 
 * manipule pas directement le Renderer, c'est l'Application qui s'en charge.
 */

#include "Math/Color.hpp"
#include "Core/RenderSettings.hpp"
#include "IRenderer.hpp"
#include <memory>

// Forward declaration pour la structure C de SDL (au cas où un backend en aurait besoin)
struct SDL_Renderer;

namespace nut 
{
    class Window;
    class Application;

    /**
     * @addtogroup Core_Module
     * @{
     */

    /**
     * @internal
     * @brief Gestionnaire de rendu agissant comme un point d'accès unique.
     * * Gère le cycle de vie du backend graphique actif (le pointeur `IRenderer`) 
     * et lui transfère (forward) les ordres de dessin.
     */
    class Renderer
    {
        /** @brief Autorise l'Application à construire ce Renderer via le constructeur privé. */
        friend class Application;

        /// @name État Interne
        /// @{
        private:
            /** 
             * @brief Pointeur intelligent vers l'implémentation spécifique de l'API graphique (Backend). 
             * @internal
             */
            std::unique_ptr<IRenderer> api;
        /// @}

        private:
            /**
             * @internal
             * @brief Constructeur privé réservé à l'Application.
             * * Alloue dynamiquement le bon backend (ex: SDLRenderer) et appelle son `init()`.
             * @param window Référence vers la fenêtre cible.
             * @param settings Paramètres globaux du rendu (ex: clear color).
             */
            explicit Renderer(Window& window, const RenderSettings& settings = RenderSettings());

        public:
            /**
             * @name Gestion de la Copie et du Déplacement
             * @{
             */
            
            /** @brief Constructeur par copie supprimé (le Renderer est unique pour la fenêtre). */
            Renderer(const Renderer&) = delete;
            
            /** @brief Constructeur par déplacement par défaut. */
            Renderer(Renderer&& other) noexcept = default;

            /** @brief Opérateur d'affectation par copie supprimé. */
            Renderer& operator=(const Renderer&) = delete;
            
            /** @brief Opérateur d'affectation par déplacement par défaut. */
            Renderer& operator=(Renderer&& other) noexcept = default;
            
            /** @} */

            /**
             * @brief Destructeur. Libère proprement le backend graphique (`api`).
             */
            ~Renderer();

            /**
             * @brief Ordonne au backend graphique (IRenderer) de nettoyer l'écran.
             * * Appelé automatiquement par l'Application au début de chaque frame.
             */
            void clear();

            /**
             * @brief Ordonne au backend graphique (IRenderer) d'afficher le rendu final.
             * * Appelé automatiquement par l'Application à la fin de chaque frame.
             */
            void print();
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif