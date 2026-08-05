#ifndef APPLICATION_H
#define APPLICATION_H

/**
 * @file Application.hpp
 * @brief Définition de la classe Application (Point central du moteur) et de ses paramètres globaux.
 */

#include <memory>
#include "Core/WindowSettings.hpp"
#include "Core/RenderSettings.hpp"

namespace nut 
{
    /**
     * @addtogroup Core_Module
     * @{
     */

    /**
     * @brief Regroupe l'ensemble des configurations globales nécessaires au lancement de l'application.
     */
    struct AppSettings
    {
        /** @brief Paramètres de configuration et dimensions de la fenêtre principale. */
        WindowSettings window;
        
        /** @brief Paramètres de configuration du rendu graphique. */
        RenderSettings renderer;
    };

    /**
     * @brief L'orchestrateur principal du moteur (Game Loop et gestion des sous-systèmes).
     * * La classe `Application` est le point d'entrée haut niveau de ton moteur. Elle gère 
     * l'initialisation de la fenêtre, du contexte graphique et fait tourner la boucle principale (Game Loop).
     * * @warning **Classe Monolithique Immobile :** L'application est strictement non-copiable et non-déplaçable 
     * (`= delete`). Elle doit être instanciée une seule fois (généralement dans le `main.cpp`). 
     * Cela garantit que son adresse en mémoire ne change jamais, évitant ainsi tout pointeur fantôme 
     * (`dangling pointer`) dans les systèmes ou les composants.
     */
    class Application
    {
        private:
            /**
             * @internal
             * @brief Structure opaque pour masquer les détails d'implémentation (Idiome Pimpl).
             * @warning Masque les dépendances lourdes (comme les pointeurs SDL) hors du header public.
             */
            struct Impl;

            /** @brief Pointeur intelligent vers l'implémentation cachée. @internal */
            std::unique_ptr<Impl> impl;

        private:
            /**
             * @internal
             * @brief Met à jour l'état logique de l'application et rafraîchit le rendu à chaque frame.
             */
            void update();

        public:
            /**
     * @brief Initialise l'application, crée la fenêtre et configure le moteur.
     * @param s Configuration globale initiale (par défaut via AppSettings()).
     * @throw std::runtime_error En cas d'échec critique à l'initialisation (ex: SDL_Init).
     */
            explicit Application(const AppSettings& s = AppSettings());

            /**
             * @brief Destructeur. Libère proprement les ressources graphiques et la fenêtre.
             */
            ~Application();

            // Suppression de la copie et du déplacement (Garantie d'immobilité)
            Application(const Application&) = delete;
            Application(Application&&) = delete;
            
            Application& operator=(const Application&) = delete;
            Application& operator=(Application&&) = delete;

            /**
             * @brief Lance la boucle principale du jeu (Game Loop).
             * * Bloque l'exécution du thread principal dans une boucle `while` 
             * qui gère les événements système, les mises à jour et le rendu jusqu'à la fermeture de la fenêtre.
             */
            void run();
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif