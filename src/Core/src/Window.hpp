#ifndef WINDOW_H
#define WINDOW_H

/**
 * @file Window.hpp
 * @internal
 * @brief Définition de la classe Window encapsulant la fenêtre du système.
 * @warning Ce fichier est interne au moteur. L'utilisateur final configure la fenêtre
 * via `WindowSettings` (passé à l'Application) mais ne manipule pas directement cette classe.
 */

#include "Core/WindowSettings.hpp"
#include <memory>

struct SDL_Window;

namespace nut 
{
    class Application;
    class Renderer;

    /**
     * @addtogroup Core_Module
     * @{
     */

    /**
     * @internal
     * @brief Gestionnaire de la fenêtre du système d'exploitation.
     * * Utilise l'idiome Pimpl pour masquer totalement les dépendances système (comme <SDL.h>) 
     * du reste du moteur. Le cycle de vie de la fenêtre est strictement contrôlé par l'`Application`.
     */
    class Window
    {
        /** @brief Autorise l'Application à construire cette fenêtre via les constructeurs privés. */
        friend class Application;

        /// @name État Interne
        /// @{
        private:
            /** 
             * @internal
             * @brief Structure opaque contenant le pointeur natif (ex: SDL_Window*). 
             * Masque l'implémentation SDL dans le fichier .cpp.
             */
            struct Impl;
            
            /** @brief Pointeur intelligent vers l'implémentation cachée (Pimpl). @internal */
            std::unique_ptr<Impl> impl;
        /// @}

        private:
            /**
             * @internal
             * @brief Constructeur détaillé réservé à l'Application.
             * @param w Largeur de la fenêtre en pixels.
             * @param h Hauteur de la fenêtre en pixels.
             * @param name Titre de la fenêtre.
             * @param x Position X (ou flag de centrage).
             * @param y Position Y (ou flag de centrage).
             * @param flags Paramètres de style et de comportement (WindowFlags).
             */
            Window(int w, int h, const std::string& name = "", int x = 0, int y = 0, const WindowFlags flags = 0);
            
            /**
             * @internal
             * @brief Constructeur basé sur une configuration globale réservé à l'Application.
             * @param s Les paramètres de configuration de la fenêtre (WindowSettings).
             */
            explicit Window(const WindowSettings& s = WindowSettings());

        public:
            /**
             * @brief Destructeur. Libère la fenêtre système (ex: appel à `SDL_DestroyWindow`).
             */
            ~Window();

            /**
             * @name Gestion de la Copie et du Déplacement
             * @{
             */

            /** @brief Constructeur par copie supprimé (une fenêtre est unique et non duplicable). */
            Window(const Window&) = delete;
            
            /** @brief Constructeur par déplacement par défaut. */
            Window(Window&& other) noexcept = default;

            /** @brief Opérateur d'affectation par copie supprimé. */
            Window& operator=(const Window&) = delete;
            
            /** @brief Opérateur d'affectation par déplacement par défaut. */
            Window& operator=(Window&& other) noexcept = default;

            /** @} */

            /**
             * @brief Récupère le pointeur natif de la fenêtre système.
             * * Utile pour les backends de rendu (comme OpenGL, Vulkan ou SDL_Renderer) 
             * qui ont besoin d'un accès direct à la surface de la fenêtre (ex: `SDL_CreateRenderer`).
             * @return void* Un pointeur opaque (type-erased) vers la fenêtre (ex: `SDL_Window*`).
             */
            void* getWindow() const noexcept;
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif