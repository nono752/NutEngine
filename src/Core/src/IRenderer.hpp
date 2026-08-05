#ifndef I_RENDERER_H
#define I_RENDERER_H

/**
 * @file IRenderer.hpp
 * @internal
 * @brief Définition de l'interface abstraite pour le moteur de rendu graphique.
 * @warning Ce fichier est strictement interne au moteur (architecture des backends graphiques).
 */

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
     * @brief Interface abstraite (Contrat) interne pour les systèmes de rendu.
     * * Cette classe permet d'isoler la logique du moteur (Nut Engine) de l'API graphique 
     * sous-jacente (SDL, OpenGL, Vulkan, etc.). N'importe quel backend graphique peut être 
     * implémenté par le moteur tant qu'il respecte ce contrat.
     */
    class IRenderer 
    {
        public:
            /** 
             * @brief Destructeur virtuel par défaut. 
             * * Indispensable pour garantir la destruction correcte des classes dérivées 
             * via un pointeur polymorphe.
             */
            virtual ~IRenderer() = default;

            /**
             * @brief Initialise le contexte graphique en le liant à la fenêtre.
             * @param window La fenêtre (Window) sur laquelle le contexte graphique doit dessiner.
             * @param settings Les paramètres de configuration du rendu (ex: couleur de fond).
             */
            virtual void init(Window& window, const RenderSettings& settings) = 0;

            /**
             * @brief Efface l'écran avant de commencer un nouveau dessin.
             * * Cette méthode doit être appelée au tout début de chaque frame.
             * Elle nettoie la mémoire vidéo, généralement en remplissant l'écran 
             * avec la couleur définie dans les `RenderSettings`.
             */
            virtual void clear() = 0;

            /**
             * @brief Applique et affiche le rendu final à l'écran.
             * * Équivalent à un "Present" ou "SwapBuffers" selon les API. 
             * Cette méthode doit être appelée à la toute fin de la frame, une fois 
             * que toutes les entités ont été dessinées.
             */
            virtual void print() = 0;
    };

    /** @} */ // Fin du groupe Core_Module
}

#endif