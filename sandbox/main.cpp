#include <iostream>
#include "Core/Application.hpp"

int main(int argc, const char* argv)
{
    nut::AppSettings settings;
    settings.window.name = "sandbox";
    settings.renderer.backColor = {255, 255, 255, 255};
    //settings.window.flags = WindowFlag::FULLSCREEN;
    settings.window.h = 500;
    settings.window.w = 500;

    try
    {
        nut::Application app(settings);
        app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}