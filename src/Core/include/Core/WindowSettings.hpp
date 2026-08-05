#ifndef WINDOW_SETTINGS_H
#define WINDOW_SETTINGS_H

/**
 * @file WindowSettings.hpp
 * @brief Définition des structures et constantes de configuration pour la fenêtre du moteur.
 */

#include <string>
#include <cstdint>

namespace nut 
{
    /**
     * @defgroup Core_Module Module Core
     * @brief Ensemble des systèmes fondamentaux du moteur (Fenêtre, Rendu, Application).
     * @{
     */

    /** @brief Type entier utilisé pour combiner plusieurs propriétés de fenêtre (Bitmask). */
    using WindowFlags = uint64_t;

    /**
     * @brief Regroupe les constantes définissant le comportement et l'apparence de la fenêtre.
     * * Ces flags peuvent être combinés avec l'opérateur bit-à-bit `|` (ex: `FULLSCREEN | BORDERLESS`).
     */
    namespace WindowFlag
    {
        /** @brief Fenêtre classique par défaut (visible, avec bordures, non redimensionnable). */
        extern const WindowFlags NONE;
        
        /** @brief Affiche la fenêtre en plein écran. */
        extern const WindowFlags FULLSCREEN;
        
        /** @brief Crée la fenêtre de manière invisible (utile pour charger des ressources avant l'affichage). */
        extern const WindowFlags HIDDEN;
        
        /** @brief Supprime la barre de titre et les bordures de la fenêtre. */
        extern const WindowFlags BORDERLESS;
        
        /** @brief Autorise l'utilisateur à redimensionner la fenêtre à la volée. */
        extern const WindowFlags RESIZABLE;
    }

    /**
     * @brief Regroupe les constantes de positionnement spécifiques pour la fenêtre.
     */
    namespace WindowPos
    {
        /** @brief Demande au système d'exploitation de centrer la fenêtre sur l'écran actif. */
        extern const int CENTERED;
    }

    /**
     * @brief Structure de configuration (Blueprint) pour l'initialisation de la fenêtre.
     * * Regroupe tous les paramètres nécessaires à la création du contexte graphique.
     * Peut être passée à l'Application ou au constructeur de la fenêtre.
     */
    struct WindowSettings
    {
        /** @brief Largeur de la fenêtre en pixels. */
        int w = 1280;
        
        /** @brief Hauteur de la fenêtre en pixels. */
        int h = 720;
        
        /** @brief Position horizontale sur l'écran (ou `WindowPos::CENTERED`). */
        int x = WindowPos::CENTERED;
        
        /** @brief Position verticale sur l'écran (ou `WindowPos::CENTERED`). */
        int y = WindowPos::CENTERED;
        
        /** @brief Titre qui sera affiché dans la barre supérieure de la fenêtre. */
        std::string name = "Default";
        
        /** @brief Combinaison (Bitmask) des propriétés souhaitées issues de `WindowFlag`. */
        WindowFlags flags = 0;
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif