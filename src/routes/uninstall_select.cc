#include <imgui.h>
#include <animate.h>
#include <chrono>
#include <texture.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <memory>
#include <router.h>
#include <iostream>
#include <dpi.h>
#include <unordered_map>
#include <components.h>

using namespace ImGui;

static std::string steamPath = "C:\\Program Files (x86)\\Steam";

const void RenderUninstallSelect(float xPos)
{
    ImGuiIO& io = GetIO();
    ImGuiViewport* viewport = GetMainViewport();
    
    const int XPadding           = ScaleX(55);
    const int BottomNavBarHeight = ScaleY(115);

    const int PromptContainerWidth  = viewport->Size.x - XPadding * 2;
    const int PromptContainerHeight = viewport->Size.y / 1.5f;

    float startY  = viewport->Size.y + BottomNavBarHeight;
    float targetY = viewport->Size.y - BottomNavBarHeight; 
    
    static float currentY = startY;
    static bool animate   = true;

    static auto animationStartTime = std::chrono::steady_clock::now();

    SetCursorPos(ImVec2(
        xPos + (viewport->Size.x - PromptContainerWidth) / 2, 
        (viewport->Size.y - PromptContainerHeight) / 2
    ));

    PushStyleColor(ImGuiCol_Border, ImVec4(0.169f, 0.173f, 0.18f, 1.0f));

    BeginChild("##PromptContainer", ImVec2(PromptContainerWidth, PromptContainerHeight), false);
    {
        PushFont(io.Fonts->Fonts[1]);
        Text(u8"Uninstall Millennium ✨");
        PopFont();

        Spacing();
        PushStyleColor(ImGuiCol_Text, ImVec4(0.422f, 0.425f, 0.441f, 1.0f));
        TextWrapped("Select the components you would like to remove.");

        Spacing();

        SetCursorPosY(GetCursorPosY() + ScaleY(30));

        RenderCheckBox(true, "Millennium: 82.11 MB", "");

        Spacing();

        RenderCheckBox(true, "Custom Steam Components: 1.49 GB", "");

        Spacing();

        RenderCheckBox(true, "Dependencies: 75.65 MB", "");

        Spacing();

        RenderCheckBox(true, "Themes: 62.12 MB", "");

        Spacing();

        RenderCheckBox(true, "Plugins: 12.59 MB", "");

        SetCursorPosY(GetCursorPosY() + ScaleY(40));    


        Separator();

        Spacing();

        Text("Uninstalling from: %s", steamPath.c_str());
        Text("Reclaimed Disk Space: 1.72 GB");

        PopStyleColor();
    }
    EndChild();
    PopStyleColor();

    SetCursorPos(ImVec2(xPos, viewport->Size.y - BottomNavBarHeight + 1));

    PushStyleVar  (ImGuiStyleVar_WindowPadding, ImVec2(ScaleX(30), ScaleY(30)));
    PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    PushStyleVar  (ImGuiStyleVar_ChildRounding, 0.0f);
    PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.078f, 0.082f, 0.09f, 1.0f));

    BeginChild("##BottomNavBar", ImVec2(viewport->Size.x, BottomNavBarHeight - 3), true, ImGuiWindowFlags_NoScrollbar);
    {
        SetCursorPos(ImVec2(ScaleX(45), GetCursorPosY() + ScaleY(12.5)));
        Image((ImTextureID)(intptr_t)infoIconTexture, ImVec2(ScaleX(25), ScaleY(25)));

        SameLine(0, ScaleX(42));
        const float cursorPosSave = GetCursorPosX();

        SetCursorPosY(GetCursorPosY() - ScaleX(12));
        TextColored(ImVec4(0.322f, 0.325f, 0.341f, 1.0f), "Steam Homebrew & Millennium are not affiliated with");

        SetCursorPos(ImVec2(cursorPosSave, GetCursorPosY() - ScaleY(20)));
        TextColored(ImVec4(0.322f, 0.325f, 0.341f, 1.0f), "Steam®, Valve, or any of their partners.");
        
        SameLine(0);
        SetCursorPosY(GetCursorPosY() - ScaleY(25));

        static bool isButtonHovered = false;
        float currentColor = TransitionFlatColor("##NextButton", 1.0f, 0.8f, isButtonHovered, 0.3f);

        PushStyleColor(ImGuiCol_Button,        ImVec4(currentColor, currentColor, currentColor, 1.0f));
        PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(currentColor, currentColor, currentColor, 1.0f));
        PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        PushStyleColor(ImGuiCol_Text,          ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        const int FooterContainerWidth = ScaleX(300);
        const float buttonPos = GetCursorPosY();

        SetCursorPosX(xPos + GetCursorPosX() + GetContentRegionAvail().x - FooterContainerWidth);
        SetCursorPosY(GetCursorPosY() + ScaleY(10));

        Image((ImTextureID)(intptr_t)discordIconTexture, ImVec2(ScaleX(30), ScaleY(30)));
        SameLine(0, ScaleX(25));
        SetCursorPosY(GetCursorPosY() - ScaleY(15));

        Image((ImTextureID)(intptr_t)gtihubIconTexture, ImVec2(ScaleX(30), ScaleY(30)));
        SameLine(0, ScaleX(25));

        SetCursorPosY(buttonPos);

        if (Button("Install", ImVec2(xPos + GetContentRegionAvail().x, GetContentRegionAvail().y)))
        {
            // routerPtr->goInstaller();
        }

        if (isButtonHovered)
        {
            SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        PopStyleColor(4);
        isButtonHovered = IsItemHovered() || (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseDown(ImGuiMouseButton_Left));
    }
    EndChild();

    PopStyleVar(2);
    PopStyleColor(2);
}