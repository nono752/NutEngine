#include "SDL3/SDL.h"
#include "Core/Application.hpp"
#include "Window.hpp"
#include "Renderer.hpp"

namespace nut 
{
    /**
     * @internal
     * @brief Implémentation cachée de l'Application (Idiome Pimpl).
     */
    struct Application::Impl
    {
        /** @brief Paramètres globaux de l'application. */
        AppSettings settings;
        /** @brief Gestionnaire de la fenêtre principale. */
        Window window;
        /** @brief Gestionnaire de rendu graphique. */
        Renderer renderer;
        /** @brief État d'exécution de la boucle principale. */
        bool running = false;

        /**
         * @brief Constructeur de l'implémentation.
         * @param s Configuration initiale passée par l'Application.
         */
        Impl(const AppSettings &s) 
        : settings(s), window(s.window), renderer(window, s.renderer) 
        {}
    };

    Application::~Application() = default;

    Application::Application(const AppSettings& s) 
    : impl(std::make_unique<Impl>(s))
    { };

    void Application::run()
    {
        SDL_Event ev;
        while (impl->running)
        {
            while (SDL_PollEvent(&ev))
            {
                switch (ev.type)
                {
                    case SDL_EVENT_QUIT:
                        impl->running = false;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        impl->running = false;
                        break;
                    default:
                        break;
                }
            }

            update();
        }
    }

    void Application::update()
    {
        impl->renderer.clear();
        impl->renderer.print();
    }
}