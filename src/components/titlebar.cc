#include <imgui.h>
#include <texture.h>
#include <dpi.h>

using namespace ImGui;

/**
 * Render the title bar component.
 * @return Whether the title bar component is hovered.
 */
bool RenderTitleBarComponent()
{
    static const char* strTitleText = "Steam Homebrew";

    ImGuiViewport* viewport = GetMainViewport();
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ScaleX(15), ScaleY(15)));

    BeginChild("##TitleBar", ImVec2(viewport->Size.x, ScaleY(75)), false, ImGuiWindowFlags_NoScrollbar);
    {
        float iconDimension = ScaleX(20);
        float closeButtonDimension = ScaleY(20);

        float titlePadding = ScaleX(20);

        SetCursorPos(ImVec2(titlePadding, titlePadding));
        Image((ImTextureID)(intptr_t)logoTexture, ImVec2(iconDimension, iconDimension));

        SameLine(0, titlePadding);
        SetCursorPosY(GetCursorPosY() - 2);
        Text(strTitleText);
        SameLine(0, 0);

        // Get frame rate
        Text("FPS: %.1f", GetIO().Framerate);


        SameLine(0, 0);

        static bool isCloseButtonHovered = false;

        if (isCloseButtonHovered) 
        {
            PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.769f, 0.169f, 0.11f, 1.0f));
        }

        ImVec2 closeButtonDimensions = ImVec2(ScaleX(70), ScaleY(43));

        SetCursorPos(ImVec2(viewport->Size.x - closeButtonDimensions.x, 0));

        PushStyleVar(ImGuiStyleVar_ChildRounding, 0);
        BeginChild("##CloseButton", ImVec2(closeButtonDimensions.x, closeButtonDimensions.y), true, ImGuiWindowFlags_NoScrollbar);
        {
            SetCursorPos(ImVec2(ScaleX(25), ScaleY(12)));
            Image((ImTextureID)(intptr_t)closeButtonTexture, ImVec2(closeButtonDimension, closeButtonDimension));
        }
        PopStyleVar();
        EndChild();

        if (IsItemClicked(ImGuiMouseButton_Left))
        {
            PostQuitMessage(0);
        }

        if (isCloseButtonHovered)
        {
            PopStyleColor();
        }

        isCloseButtonHovered = IsItemHovered();
    }
    EndChild();
    PopStyleVar();

    return IsItemHovered() || (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseDown(ImGuiMouseButton_Left));
}