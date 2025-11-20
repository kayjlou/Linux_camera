/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "Fonts.h"
#include "WindowUtils.h"
#include <string>

Fonts& Fonts::getInstance() {
    static Fonts instance;
        return instance;
}

Fonts::Fonts() {
}

Fonts::~Fonts() {
}

void Fonts::loadFonts() {
    const char* arialPath = "../data/font/arial.ttf";
    const char* arialBoldPath = "../data/font/arial_bold.ttf";

    auto io = ImGui::GetIO();
    headerFont = io.Fonts->AddFontFromFileTTF(arialBoldPath, WindowUtils::getInstance().adjustToImageHeight(Constants::HeaderFontSize));
    messageFont = io.Fonts->AddFontFromFileTTF(arialPath, WindowUtils::getInstance().adjustToImageHeight(Constants::MessageFontSize));
    recordingModeFont = io.Fonts->AddFontFromFileTTF(arialPath, WindowUtils::getInstance().adjustToImageHeight(Constants::RecordingModeFontSize));
    countdownFont = io.Fonts->AddFontFromFileTTF(arialBoldPath, WindowUtils::getInstance().adjustToImageHeight(Constants::CountdownFontSize));
    switchFont = io.Fonts->AddFontFromFileTTF(arialPath, WindowUtils::getInstance().adjustToImageHeight(Constants::SwitchFontSize));
}

ImFont* Fonts::getHeaderFont() const {
    return headerFont;
}

ImFont* Fonts::getMessageFont() const {
    return messageFont;
}

ImFont* Fonts::getRecordingModeFont() const {
    return recordingModeFont;
}

ImFont* Fonts::getCountdownFont() const {
    return countdownFont;
}

ImFont* Fonts::getSwitchFont() const {
    return switchFont;
}
