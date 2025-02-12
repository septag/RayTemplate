#include "Core.h"
#include <raylib.h>

#if USE_LIVEPP
#include <LPP_API_x64_CPP.h>
#include <LPP_API_Options.h>
#endif

#define CLAY_IMPLEMENTATION
#include "clay/clay.h"

// Always put after clay.h 
#include "Main.h"

int main(int argc, char* argv[])
{
    #if USE_LIVEPP
    lpp::LppSynchronizedAgent lppAgent = lpp::LppCreateSynchronizedAgentANSI(nullptr, STRINGIZE(LIVEPP_PATH));
    lppAgent.EnableModuleANSI(lpp::LppGetCurrentModulePathANSI(), lpp::LPP_MODULES_OPTION_NONE, nullptr, nullptr);
    if (!lpp::LppIsValidSynchronizedAgent(&lppAgent)) {
        ASSERT_MSG(0, "LivePP initialization failed. Make sure cwd is the root directory of the project");
        return false;
    }
    #endif
    
    Log::SetSettings(LogLevel::Debug, false, false);

    #if PLATFORM_WINDOWS
    OS::Win32EnableProgramConsoleCoding();
    #endif

    if (!Initialize()) {
        LOG_ERROR("Init application '%s' failed", STRINGIZE(APPNAME));
        return -1;
    }
    LOG_INFO("Initialized");

    while (!WindowShouldClose()) {
        #if USE_LIVEPP
        if (lppAgent.WantsReload(lpp::LPP_RELOAD_OPTION_SYNCHRONIZE_WITH_RELOAD))
            lppAgent.Reload(lpp::LPP_RELOAD_BEHAVIOUR_WAIT_UNTIL_CHANGES_ARE_APPLIED);
        if (lppAgent.WantsRestart())
            lppAgent.Restart(lpp::LPP_RESTART_BEHAVIOUR_INSTANT_TERMINATION, 0u, nullptr);
        #endif // USE_LIVEPP

        Update();
    }    

    Release();
    CloseWindow();

    #if USE_LIVEPP
    lppAgent.DisableModuleANSI(lpp::LppGetCurrentModulePathANSI(), lpp::LPP_MODULES_OPTION_NONE, nullptr, nullptr);
    lpp::LppDestroySynchronizedAgent(&lppAgent);
    #endif 

    return 0;
}

void RenderClayCommands(const Clay_RenderCommandArray& renderCmds)
{
    for (int i = 0; i < renderCmds.length; i++) {
        const Clay_RenderCommand& renderCmd = renderCmds.internalArray[i];
        const Clay_BoundingBox& bb = renderCmd.boundingBox;
        switch (renderCmd.commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            const Clay_RectangleRenderData& rect = renderCmd.renderData.rectangle;
            float radius = (rect.cornerRadius.topLeft * 2) / float((bb.width > bb.height) ? bb.height : bb.width);
            if (rect.cornerRadius.topLeft > 0) {
                DrawRectangleRounded({bb.x, bb.y, bb.width, bb.height}, radius, 8, ClayColorToRayColor(rect.backgroundColor));
            }
            else {
                DrawRectangle(bb.x, bb.y, bb.width, bb.height, ClayColorToRayColor(rect.backgroundColor));
            }
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            const Clay_BorderRenderData& border = renderCmd.renderData.border;
            // Left border
            if (border.width.left > 0) {
                DrawRectangle(int(bb.x), int(bb.y + border.cornerRadius.topLeft), int(border.width.left), 
                              int(bb.height - border.cornerRadius.topLeft - border.cornerRadius.bottomLeft), 
                              ClayColorToRayColor(border.color));
            }
            // Right border
            if (border.width.right > 0) {
                DrawRectangle(int(bb.x + bb.width - border.width.right), int(bb.y + border.cornerRadius.topRight), 
                              int(border.width.right), int(bb.height - border.cornerRadius.topRight - border.cornerRadius.bottomRight), 
                              ClayColorToRayColor(border.color));
            }
            // Top border
            if (border.width.top > 0) {
                DrawRectangle(int(bb.x + border.cornerRadius.topLeft), int(bb.y), 
                              int(bb.width - border.cornerRadius.topLeft - border.cornerRadius.topRight), int(border.width.top), 
                              ClayColorToRayColor(border.color));
            }
            // Bottom border
            if (border.width.bottom > 0) {
                DrawRectangle(int(bb.x + border.cornerRadius.bottomLeft), int(bb.y + bb.height - border.width.bottom), 
                              int(bb.width - border.cornerRadius.bottomLeft - border.cornerRadius.bottomRight), int(border.width.bottom), 
                              ClayColorToRayColor(border.color));
            }
            if (border.cornerRadius.topLeft > 0) {
                DrawRing({ (bb.x + border.cornerRadius.topLeft), (bb.y + border.cornerRadius.topLeft) }, 
                           (border.cornerRadius.topLeft - border.width.top), border.cornerRadius.topLeft, 
                            180, 270, 10, ClayColorToRayColor(border.color));
            }
            if (border.cornerRadius.topRight > 0) {
                DrawRing({ (bb.x + bb.width - border.cornerRadius.topRight), (bb.y + border.cornerRadius.topRight) }, 
                         (border.cornerRadius.topRight - border.width.top), border.cornerRadius.topRight, 
                         270, 360, 10, ClayColorToRayColor(border.color));
            }
            if (border.cornerRadius.bottomLeft > 0) {
                DrawRing({ (bb.x + border.cornerRadius.bottomLeft), (bb.y + bb.height - border.cornerRadius.bottomLeft) }, 
                    (border.cornerRadius.bottomLeft - border.width.top), border.cornerRadius.bottomLeft, 
                    90, 180, 10, ClayColorToRayColor(border.color));
            }
            if (border.cornerRadius.bottomRight > 0) {
                DrawRing({ (bb.x + bb.width - border.cornerRadius.bottomRight), (bb.y + bb.height - border.cornerRadius.bottomRight) }, 
                    (border.cornerRadius.bottomRight - border.width.bottom), border.cornerRadius.bottomRight, 
                0.1, 90, 10, ClayColorToRayColor(border.color));
            }
            break;
        }
        }
    }
}