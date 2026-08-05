#ifndef COLOR_H
#define COLOR_H

/**
 * @file Color.hpp
 * @brief Définition de la structure Color pour la gestion des couleurs RGBA.
 */

#include <cstdint>

/**
 * @brief Regroupe les fonctionnalités du module mathématique.
 */
namespace nut::math
{
    /**
    * @defgroup Math_Module Module Math
    * @brief Ensemble des structures et utilitaires mathématiques du moteur.
    * @{
    */

    /**
     * @brief Représente une couleur RGBA en nuances de 8 bits non signés.
     * * Chaque canal (Rouge, Vert, Bleu, Alpha) varie de 0 à 255. 
     * Cette structure est conçue pour être ultra-légère et directement compatible 
     * avec les API graphiques (comme les fonctions de dessin SDL).
     */
    struct Color
    {
        /** @brief Intensité du canal Rouge (0 = Aucun, 255 = Maximum). */
        uint8_t r = 0;
        /** @brief Intensité du canal Vert (0 = Aucun, 255 = Maximum). */
        uint8_t g = 0;
        /** @brief Intensité du canal Bleu (0 = Aucun, 255 = Maximum). */
        uint8_t b = 0;
        /** @brief Canal de transparence (Alpha) : 0 = Totalement transparent, 255 = Totalement opaque. */
        uint8_t a = 255;
    };

    /** @} */ // Fin du groupe Math_Module
}

#endif