#include "SDL3/SDL.h"
#include "RendererSDL.hpp"
#include "Window.hpp"
#include "Core/RenderSettings.hpp"
#include <stdexcept>

namespace nut
{
    void RendererSDL::init(Window& window, const RenderSettings& settings)
    {
        if (!(renderer = SDL_CreateRenderer(static_cast<SDL_Window*> (window.getWindow()), nullptr)))
            throw std::runtime_error(SDL_GetError());

        backColor = settings.backColor;
    }

    RendererSDL::~RendererSDL()
    {
        SDL_DestroyRenderer(renderer);
    }

    void RendererSDL::clear()
    {
        SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
        SDL_RenderClear(renderer);
    }

    void RendererSDL::print()
    {
        SDL_RenderPresent(renderer);
    }
}