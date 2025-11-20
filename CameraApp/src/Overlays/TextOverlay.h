/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef TEXT_OVERLAY_H
#define TEXT_OVERLAY_H

#include "Overlay.h"
#include <string>

class TextOverlay : Overlay {
public:
    TextOverlay();
    void draw() override;
    void setFontColor(ImU32 color);
    void setCenter(int x, int y);
    void setCenter(float x, float y);
    void setText(const std::string& text);
    void setFont(ImFont* font);
    ImVec2 getTopLeft() const;
    ImVec2 getCenter() const;
private:
    static constexpr ImU32 defaultTextColor = IM_COL32(28, 27, 31, 255);

    ImFont* font;
    ImU32 color = defaultTextColor;
    ImVec2 topLeft = ImVec2(10, 10);
    ImVec2 center = ImVec2(10, 10);
    std::string text = "";
    bool positionCorrectionRequest = false;

    void positionCorrection();
};

#endif