#include "Core.h"
#include <raylib.h>

#include "Main.h"

const int fontSize = 100;
const int fontSpacing = 0;
Font font;
Clay_Arena arena;

static void HandleClayErrors(Clay_ErrorData errorData) 
{
    LOG_ERROR(errorData.errorText.chars);
}

static void HandleButtonInteraction(Clay_ElementId elemId, Clay_PointerData pointerData, intptr_t /*userData*/)
{
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
        LOG_DEBUG("Click %.1f %.1f", pointerData.position.x, pointerData.position.y);
}

bool Initialize()
{
    // TODO: implement

    InitWindow(1024, 768, "TestProj");
    SetTargetFPS(60);

    font = LoadFontEx("fonts/anonymous_pro_bold.ttf", fontSize, nullptr, 0);

    uint64 totalMemSize = Clay_MinMemorySize();
    arena = Clay_CreateArenaWithCapacityAndMemory(totalMemSize, Mem::Alloc(totalMemSize));
    Clay_Initialize(arena, {float(GetScreenWidth()), float(GetScreenHeight())}, {HandleClayErrors, nullptr});
    return true;
}

void Release()
{
    // TODO: implement
}

void Update()
{
    // TODO: implement

    BeginDrawing();
    ClearBackground(RAYWHITE);

    const char* text = "Hello World";
    Vector2 textSize = MeasureTextEx(font, text, fontSize, fontSpacing);
    DrawTextEx(font, text, {(1024 - textSize.x)/2, (768 - textSize.y)/2}, fontSize, fontSpacing, LIGHTGRAY);
    EndDrawing();

    Vector2 mousewheel = GetMouseWheelMoveV();
    Clay_SetLayoutDimensions({float(GetScreenWidth()), float(GetScreenHeight())});
    Clay_SetPointerState({float(GetMouseX()), float(GetMouseY())}, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    Clay_UpdateScrollContainers(true, {mousewheel.x, mousewheel.y}, GetFrameTime());
    Clay_BeginLayout();

    Clay_Color buttonHover = { 200, 80, 200, 200 };
    Clay_Color buttonNormal = { 140, 80, 200, 200 };
    CLAY({
        .id = CLAY_ID("FloatingContainer"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(300),
                .height = CLAY_SIZING_FIXED(300)
            },
            .padding = {16, 16, 16, 16}
        },
        .backgroundColor = Clay_PointerOver(CLAY_ID("FloatingContainer")) ? buttonHover : buttonNormal,
        .cornerRadius = CLAY_CORNER_RADIUS(10),
        .floating = {
            .offset = { 0, 0 },
            .zIndex = 1,
            .attachPoints = {
                CLAY_ATTACH_POINT_CENTER_TOP,
                CLAY_ATTACH_POINT_CENTER_TOP
            },
            .attachTo = CLAY_ATTACH_TO_PARENT,
        },
        .border = {
            .color = { 80, 80, 80, 255 },
            .width = { 20, 20, 20, 20 }
        },
    }) {
        Clay_OnHover(HandleButtonInteraction, 0);
        CLAY({
            .id = CLAY_ID("FloatingContainer2"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(100),
                    .height = CLAY_SIZING_FIXED(100)
                },
                .padding = {16, 16, 16, 16}
            },
            .backgroundColor = {100, 100, 100, 255},
            .cornerRadius = CLAY_CORNER_RADIUS(10),
            .floating = {
                .offset = { 0, 0 },
                .zIndex = 1,
                .attachPoints = {
                    CLAY_ATTACH_POINT_CENTER_CENTER,
                    CLAY_ATTACH_POINT_CENTER_CENTER
                },
                .attachTo = CLAY_ATTACH_TO_PARENT,
            }
        });
    }

    RenderClayCommands(Clay_EndLayout());
}
