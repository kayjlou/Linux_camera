/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "TextOverlay.h"

TextOverlay::TextOverlay() {
}

void TextOverlay::setFontColor(ImU32 color) {
    this->color = color;
}

void TextOverlay::setCenter(int x, int y) {
    center  = ImVec2(x, y);
    positionCorrectionRequest = true;
}

void TextOverlay::setCenter(float x, float y) {
    center  = ImVec2(x, y);
    positionCorrectionRequest = true;
}

void TextOverlay::setText(const std::string& text) {
    this->text = text;
    positionCorrectionRequest = true;
}

void TextOverlay::setFont(ImFont* font) {
    this->font = font;
}

void TextOverlay::draw() {
    if (text.empty()){
        return;
    }

    ImGui::PushFont(font);

    if (positionCorrectionRequest) {
        // Pushing a font is only allowed after newFrame() call. Therefore, position correction is done here and not beforehand.
        positionCorrection();
    }

    auto textCstr = text.c_str();
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddText(topLeft, color, textCstr);
    ImGui::PopFont();
}

void TextOverlay::positionCorrection() {
    if (text.empty()) {
        return;
    }

    // Correct font has to be already pushed at this point
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

    topLeft.x = center.x - (textSize.x * 0.5f);
    topLeft.y = center.y - (textSize.y * 0.5f);
}

ImVec2 TextOverlay::getTopLeft() const {
    return topLeft;
}

ImVec2 TextOverlay::getCenter() const {
    return center;
}
