#pragma once

#include "clay/clay.h"

bool Initialize();
void Release();
void Update();

void RenderClayCommands(const Clay_RenderCommandArray& renderCmds);

//----------------------------------------------------------------------------------------------------------------------
// Inlines
INLINE Color ClayColorToRayColor(Clay_Color c)
{
    return Color { .r = uint8(c.r), .g = uint8(c.g), .b = uint8(c.b),.a = uint8(c.a) };
}

INLINE Rectangle ClayRectToRayRect(Clay_BoundingBox bb)
{
    return Rectangle { .x = bb.x, .y = bb.y, .width = bb.width, .height = bb.height };
}