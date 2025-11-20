/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef WINDOW_H
#define WINDOW_H

#ifdef __APPLE__
    #include <OpenGL/gl3.h>  // macOS uses OpenGL framework
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

#include <iostream>
#include <atomic>
#include "ButtonHandler.h"

class Window {
public:
    bool init(ButtonHandler* buttonHandler);
    bool open();
    bool shouldClose();
    void close();
    SDL_Window* getSdlWindow() const;
    SDL_GLContext getSdlContext() const;
    void getSize(int* width, int* height) const;
private:
    SDL_Window* sdlWindow;
    SDL_GLContext sdlGLContext;
    ButtonHandler* buttonHandler;

    void handleKeyEvent(const SDL_KeyboardEvent& keyEvent);
};

#endif