/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef FONTS_H
#define FONTS_H

#include <imgui.h>

class Fonts {
public:
    Fonts(const Fonts&) = delete;
    Fonts& operator=(const Fonts&) = delete;
    static Fonts& getInstance();
    void loadFonts();

    ImFont* getHeaderFont() const;
    ImFont* getMessageFont() const;
    ImFont* getRecordingModeFont() const;
    ImFont* getCountdownFont() const;
    ImFont* getSwitchFont() const;
private:
    Fonts();
    ~Fonts();

    // Central place for all fonts, because fonts are pre-rendered in ImGui
    // and therefore it is easier to load them only once with the desired size.
    ImFont* headerFont;
    ImFont* messageFont;
    ImFont* recordingModeFont;
    ImFont* countdownFont;
    ImFont* switchFont;
};

#endif
