#include "SDL3/SDL.h"
#include "Window.hpp"
#include "Renderer.hpp"
#include "RendererSDL.hpp"
#include <stdexcept>

struct SDL_Window;

namespace nut 
{
    Renderer::Renderer(Window& window, const RenderSettings& settings) 
    {
        api = std::make_unique<RendererSDL>();
        api->init(window, settings);
    }

    Renderer::~Renderer() = default;

    void Renderer::clear() 
    {
        api->clear();
    }
    
    void Renderer::print() 
    {
        api->print();
    }
}