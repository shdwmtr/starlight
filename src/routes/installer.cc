#include <router.h>
#include <memory>
#include <imgui.h>
#include <string>
#include <chrono>
#include <animate.h>
#include <texture.h>
#include <imgui_stdlib.h>
#include <imspinner.h>
#include <dpi.h>

using namespace ImGui;
using namespace ImSpinner;

static std::string statusText = "Installing Millennium v2.17.2";

const void RenderInstaller(std::shared_ptr<RouterNav> router, float xPos)
{
    ImGuiViewport* viewport = GetMainViewport();
    static const float BottomNavBarHeight = ScaleY(115);

    float startY  = viewport->Size.y + BottomNavBarHeight;
    float targetY = viewport->Size.y - BottomNavBarHeight; 
    
    static float currentY = startY;
    static bool shouldAnimate  = false;

    static auto animationStartTime = std::chrono::steady_clock::now();

    static const int spinnerSize = ScaleX(14);

    SetCursorPos(ImVec2(xPos + (viewport->Size.x / 2) - spinnerSize, (viewport->Size.y / 2) - 50));
    {
        Spinner<SpinnerTypeT::e_st_ang>("SpinnerAngNoBg", Radius{spinnerSize}, Thickness{ScaleX(3)}, Color{ImColor(255, 255, 255, 255)}, BgColor{ImColor(255, 255, 255, 0)}, Speed{6}, Angle{IM_PI}, Mode{0});
    }
    SetCursorPos(ImVec2(xPos + (viewport->Size.x) / 2 - (CalcTextSize(statusText.c_str()).x / 2), viewport->Size.y / 2 + ScaleY(15)));

    Text(statusText.c_str());

    if (IsKeyPressed(ImGuiKey_MouseMiddle))
    {
        shouldAnimate = true;
        animationStartTime = std::chrono::steady_clock::now();
    }

    static const float ANIMATION_DURATION = 0.3f;

    if (shouldAnimate)
    {
        auto now = std::chrono::steady_clock::now();
        float elapsedTime = std::chrono::duration<float>(now - animationStartTime).count();

        if (elapsedTime < ANIMATION_DURATION) 
        {
            currentY = EaseOutBack(elapsedTime, startY, targetY - startY, ANIMATION_DURATION);
        } 
        else 
        {
            currentY = viewport->Size.y - BottomNavBarHeight;
            shouldAnimate = false;
        }
    }

    SetCursorPos(ImVec2(xPos, currentY));

    PushStyleVar  (ImGuiStyleVar_WindowPadding, ImVec2(ScaleX(30), ScaleY(30))     );
    PushStyleColor(ImGuiCol_Border,             ImVec4(0.f, 0.f, 0.f, 0.f)         );
    PushStyleVar  (ImGuiStyleVar_ChildRounding, 0.0f                               );
    PushStyleColor(ImGuiCol_ChildBg,            ImVec4(0.078f, 0.082f, 0.09f, 1.0f));

    BeginChild("##BottomNavBar", ImVec2(viewport->Size.x, BottomNavBarHeight - 3), true, ImGuiWindowFlags_NoScrollbar);
    {
        SetCursorPos(ImVec2(ScaleX(45), GetCursorPosY() + ScaleY(12.5)));
        Image((ImTextureID)(intptr_t)infoIconTexture, ImVec2(ScaleX(25), ScaleY(25)));

        SameLine(0, ScaleX(42));
        const float cursorPosSave = GetCursorPosX();

        SetCursorPosY(GetCursorPosY() - ScaleY(12));
        TextColored(ImVec4(0.322f, 0.325f, 0.341f, 1.0f), "Steam Homebrew & Millennium are not affiliated with");

        SetCursorPos(ImVec2(cursorPosSave, GetCursorPosY() - ScaleY(20)));
        TextColored(ImVec4(0.322f, 0.325f, 0.341f, 1.0f), "Steam®, Valve, or any of their partners.");
        
        SameLine(0);
        SetCursorPosY(GetCursorPosY() - ScaleY(25));

        static bool isButtonHovered = false;
        float currentColor = EaseInOutFloat("##NextButton", 1.0f, 0.8f, isButtonHovered, 0.3f);

        PushStyleColor(ImGuiCol_Button,        ImVec4(currentColor, currentColor, currentColor, 1.0f));
        PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(currentColor, currentColor, currentColor, 1.0f));
        PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        PushStyleColor(ImGuiCol_Text,          ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        static const int ButtonWidth = ScaleX(150);
        const float buttonPos = GetCursorPosY();

        SetCursorPosX(xPos + GetCursorPosX() + GetContentRegionAvail().x - ButtonWidth);

        if (Button("Finish", ImVec2(xPos + GetContentRegionAvail().x, GetContentRegionAvail().y)))
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