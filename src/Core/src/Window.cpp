#include <SDL3/SDL.h>
#include <stdexcept>
#include <iostream>
#include "window.hpp"

namespace nut
{
    namespace WindowFlag
    {
        const WindowFlags NONE = 0;
        const WindowFlags FULLSCREEN = SDL_WINDOW_FULLSCREEN;
        const WindowFlags HIDDEN = SDL_WINDOW_HIDDEN;
        const WindowFlags BORDERLESS = SDL_WINDOW_BORDERLESS;
        const WindowFlags RESIZABLE = SDL_WINDOW_RESIZABLE;
    }
    namespace WindowPos
    {
        const int CENTERED = SDL_WINDOWPOS_CENTERED;
    }

    /**
     * @internal
     * @brief Implémentation cachée de la fenêtre (Idiome Pimpl).
     */
    struct Window::Impl
    {
        /** @brief Largeur de la fenêtre en pixels. */
        int w;
        /** @brief Hauteur de la fenêtre en pixels. */
        int h;
        /** @brief Position X sur l'écran. */
        int x;
        /** @brief Position Y sur l'écran. */
        int y;
        /** @brief Titre affiché. */
        std::string name;
        /** @brief Pointeur natif vers la fenêtre SDL. */
        SDL_Window* window = nullptr;
    };

    Window::Window(int w, int h, const std::string& name, int x, int y, const WindowFlags flags)
    : impl(std::make_unique<Impl>())
    {
        impl->w = w;
        impl->h = h;
        impl->x = x;
        impl->y = y;
        impl->name = name;

        if (!(impl->window = SDL_CreateWindow(name.c_str(), w, h, flags)))
            throw std::runtime_error(SDL_GetError());

        if (!SDL_SetWindowPosition(impl->window, x, y))
        {
            SDL_SetWindowPosition(impl->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            std::cerr << "Warning: cannot move window : " << SDL_GetError() << std::endl;
        }
    }

    Window::Window(const WindowSettings& s) : Window(s.w, s.h, s.name, s.x, s.y, s.flags) {}

    Window::~Window()
    {
        SDL_DestroyWindow(impl->window);
    }

    void* Window::getWindow() const noexcept 
    { 
        return impl->window; 
    }
}
