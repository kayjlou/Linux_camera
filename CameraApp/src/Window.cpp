/*
 * Copyright 2016-2025
 * This file is copyrighted by onexip GmbH.
 * All rights reserved.
 *
 * Date:   2025-05-05
 * Author: Thorsten Knoelker
 */

#include "Window.h"
#include "ActionHandler.h"

#ifdef DEBUG
#include "IntegrationTesting.h"
#endif

bool Window::init(ButtonHandler *buttonHandler)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return false;
    }

    this->buttonHandler = buttonHandler;

    return true;
}

bool Window::open()
{
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        std::cerr << "Failed to get display mode: " << SDL_GetError() << std::endl;
        return false;
    }

#ifdef USE_GLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    sdlWindow = SDL_CreateWindow(
        "ImGui + GStreamer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        displayMode.w,
        displayMode.h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

    if (!sdlWindow)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    sdlGLContext = SDL_GL_CreateContext(sdlWindow);
    if (!sdlGLContext)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }

    // Deactivate vsync
    SDL_GL_SetSwapInterval(0);

#ifndef __APPLE__
#ifdef USE_GLES
    gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
#else
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
#endif
#endif

    const GLubyte *renderer = glGetString(GL_RENDERER);
    printf("Renderer: %s\n", renderer);

    return true;
}

bool Window::shouldClose()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            handleKeyEvent(event.key);
        }
    }
    return false;
}

void Window::close()
{
    if (sdlGLContext)
    {
        SDL_GL_DeleteContext(sdlGLContext);
        sdlGLContext = nullptr;
    }

    if (sdlWindow)
    {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }

    SDL_Quit();
}

SDL_Window *Window::getSdlWindow() const
{
    return sdlWindow;
}

SDL_GLContext Window::getSdlContext() const
{
    return sdlGLContext;
}

void Window::getSize(int *width, int *height) const
{
    SDL_GetWindowSize(sdlWindow, width, height);
}

void Window::handleKeyEvent(const SDL_KeyboardEvent &keyEvent)
{
    if (!buttonHandler)
    {
        return;
    }

    bool pressed = keyEvent.type == SDL_KEYDOWN;
    bool repeat = keyEvent.repeat;
    SDL_Keycode key = keyEvent.keysym.sym;
    SDL_Keymod mod = static_cast<SDL_Keymod>(keyEvent.keysym.mod);

    if (pressed && !repeat)
    {
        if (key == SDLK_q)
        {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
        else if (key == SDLK_u)
        {
            buttonHandler->press(Button::Top);
        }
        else if (key == SDLK_h)
        {
            buttonHandler->press(Button::Left);
        }
        else if (key == SDLK_j)
        {
            buttonHandler->press(Button::Right);
        }
        else if (key == SDLK_n)
        {
            buttonHandler->press(Button::Bottom);
#ifdef DEBUG
        }
        else if (key == SDLK_t)
        {
            system("weston-terminal &");
        }
        else if (key == SDLK_x)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::ReadImxTemperature);
        }
        else if (key == SDLK_c)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::ResetCamera);
        }
        else if (key == SDLK_v)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::ShowVersions);
        }
        else if (key == SDLK_b)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::SimulateUartCommunicationInterruption);
        }
        //TODO: Can be reactivated later again to be used for testing
        /*else if ((mod & KMOD_CTRL) && key == SDLK_i)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::IntegrationTestModeToggle);
        }
        else if (key == SDLK_d)
        {
            if (IntegrationTesting::getInstance().isActive())
            {
                ActionHandler::getInstance().addAction(Action::ActionType::DisplayTemperatureMonitoringToggle);
            }
        }*/
        else if (key == SDLK_f)
        {
            ActionHandler::getInstance().addAction(Action::ActionType::ToggleVideoFreezeSimulation);
        }
        else if (key == SDLK_KP_PLUS || (key == SDLK_EQUALS && (mod & KMOD_SHIFT)))
        { // + key
            if (IntegrationTesting::getInstance().isActive())
            {
                printf("Increase LED brightness\n");
                ActionHandler::getInstance().addAction(Action::ActionType::IncreaseLedBrightness);
            }
        }
        else if (key == SDLK_KP_MINUS || key == SDLK_MINUS)
        {
            if (IntegrationTesting::getInstance().isActive())
            {
                printf("Decrease LED brightness\n");
                ActionHandler::getInstance().addAction(Action::ActionType::DecreaseLedBrightness);
            }
        }
        else if (key == SDLK_1)
        {
            auto action = Action(Action::ActionType::SimulateDistalLimitSwitch, 1);
            ActionHandler::getInstance().addAction(action);
        }
        else if (key == SDLK_2)
        {
            auto action = Action(Action::ActionType::SimulateDistalLimitSwitch, 2);
            ActionHandler::getInstance().addAction(action);
#endif
        }
    }
    else if (keyEvent.type == SDL_KEYUP)
    {
        if (key == SDLK_u)
        {
            buttonHandler->release(Button::Top);
        }
        else if (key == SDLK_h)
        {
            buttonHandler->release(Button::Left);
        }
        else if (key == SDLK_j)
        {
            buttonHandler->release(Button::Right);
        }
        else if (key == SDLK_n)
        {
            buttonHandler->release(Button::Bottom);
        }
    }
}
