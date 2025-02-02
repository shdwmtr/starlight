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
#include <map>
#include <filesystem>
#include <imspinner.h>

using namespace ImGui;
using namespace ImSpinner;
namespace fs = std::filesystem;

static std::filesystem::path steamPath = "C:\\Program Files (x86)\\Steam";

struct ComponentProps
{
    float byteSize;
    std::vector<std::string> pathList; 
};

bool isUninstalling = false; 

struct ComponentState
{
    bool isHovered;
    bool isSelected;

    struct UninstallState
    {
        enum State
        {
            Uninstalling,
            Success,
            Failed
        };

        State state;
    };

    UninstallState uninstallState;

    ComponentState(bool hovered = false, bool selected = false) : isHovered(hovered), isSelected(selected) {}
};

uint64_t GetFolderSize(const fs::path& path, std::error_code& ec)
{
    if (!fs::exists(path, ec)) 
    {
        return 0;
    }
    
    std::error_code file_ec;
    auto metadata = fs::status(path, file_ec);
    if (file_ec) 
    {
        return 0;
    }
    
    if (fs::is_regular_file(metadata))
    {
        return fs::file_size(path, file_ec);
    }
    
    uint64_t size = 0;
    for (const auto& entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec))
    {
        if (ec) 
        {
            continue;
        }
        
        auto entry_metadata = fs::status(entry, file_ec);
        if (file_ec) 
        {
            continue;
        }
        
        if (fs::is_directory(entry_metadata))
        {
            size += GetFolderSize(entry.path(), ec);
        }
        else if (fs::is_regular_file(entry_metadata))
        {
            size += fs::file_size(entry, file_ec);
        }
    }
    return size;
}

ComponentProps MakeComponentProps(std::vector<fs::path> pathList)
{
    uint64_t byteSize = 0;

    for (const auto& path : pathList)
    {
        std::error_code ec;
        byteSize += GetFolderSize(path, ec);
    }

    std::vector<std::string> pathListStr;

    for (const auto& path : pathList)
    {
        pathListStr.push_back(path.string());
    }

    return { byteSize, pathListStr };
}

std::vector<std::pair<std::string, std::tuple<ComponentState, ComponentProps>>> uninstallComponents;

void StartUninstaller()
{
    uninstallComponents = 
    {
        { "Millennium",                 std::make_tuple(ComponentState({ false, true }), MakeComponentProps({ steamPath / "user32.dll", steamPath / "millennium.dll" }))},
        { "Custom Steam Components",    std::make_tuple(ComponentState({ false, true }), MakeComponentProps({ steamPath / "ext" / "data" / "assets", steamPath / "ext" / "data" / "shims" }))},
        { "Dependencies",               std::make_tuple(ComponentState({ false, true }), MakeComponentProps({ steamPath / "ext" / "data" / "cache" }))},
        { "Themes",                     std::make_tuple(ComponentState({ false, true }), MakeComponentProps({ steamPath / "steamui" / "skins" }))},
        { "Plugins",                    std::make_tuple(ComponentState({ false, true }), MakeComponentProps({ steamPath / "plugins" }))},
    };
}

std::string BytesToReadableFormat(float bytes)
{
    const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    int suffixIndex = 0;

    while (bytes >= 1024 && suffixIndex < 4)
    {
        bytes /= 1024;
        suffixIndex++;
    }

    char buffer[32];
    sprintf(buffer, "%.2f %s", bytes, suffixes[suffixIndex]);

    return std::string(buffer);
}

std::string GetReclaimedSpace()
{
    uint64_t totalSize = 0;

    for (auto& componentPair : uninstallComponents)
    {
        const auto& [state, props] = componentPair.second;
        totalSize += state.isSelected ? props.byteSize : 0;
    }

    return BytesToReadableFormat(totalSize);
}

const void RenderComponents()
{
    for (auto& componentPair : uninstallComponents)
    {
        auto& component = componentPair.first;
        auto& [state, props] = componentPair.second;

        SetCursorPosY(GetCursorPosY() + ScaleY(5));

        std::string strPathList;

        for (const auto& path : props.pathList)
        {
            strPathList += path + "\n";
        }

        std::string formattedComponent = component + ": " + BytesToReadableFormat(props.byteSize);

        if (!isUninstalling)
        {
            state.isSelected = RenderCheckBox(state.isSelected, formattedComponent.c_str(), strPathList.c_str())->isChecked;
        }
        else
        {
            if (!state.isSelected)
            {
                SetCursorPos(ImVec2(GetCursorPosX() + ScaleX(4), GetCursorPosY() + ScaleY(4)));
                Image((ImTextureID)(intptr_t)excludedIconTexture, ImVec2(ScaleX(25), ScaleY(25)));
                SetCursorPosY(GetCursorPosY() + ScaleY(1));
                SameLine(0, ScaleX(19));
                Text(formattedComponent.c_str());
                SetCursorPosY(GetCursorPosY() + ScaleY(3));
            }
            else
            {
                switch (state.uninstallState.state)
                {
                    case ComponentState::UninstallState::Uninstalling:
                    {
                        const int spinnerSize = ScaleX(10);

                        SetCursorPos(ImVec2(GetCursorPosX() + spinnerSize / 2, GetCursorPosY() + spinnerSize / 2));
                        Spinner<SpinnerTypeT::e_st_ang>("SpinnerAngNoBg", Radius{(spinnerSize)}, Thickness{(ScaleX(3))}, Color{ImColor(255, 255, 255, 255)}, BgColor{ImColor(255, 255, 255, 0)}, Speed{6}, Angle{IM_PI}, Mode{0});
                        SameLine(0, ScaleX(23));
                        SetCursorPosY(GetCursorPosY() - spinnerSize / 4);
                        Text(formattedComponent.c_str());
                        // SetCursorPosY(GetCursorPosY() + ScaleY(2));
                        break;
                    }
                    case ComponentState::UninstallState::Success:
                        Text("%s: Uninstalled", formattedComponent.c_str());
                        break;
                    case ComponentState::UninstallState::Failed:
                        Text("%s: Failed", formattedComponent.c_str());
                        break;
                }
            }
        }
    }
}

const void RenderUninstallSelect(std::shared_ptr<RouterNav> router, float xPos)
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

        SetCursorPosY(GetCursorPosY() + ScaleY(5));
        PushStyleColor(ImGuiCol_Text, ImVec4(0.422f, 0.425f, 0.441f, 1.0f));
        TextWrapped("Select the components you would like to remove.");

        SetCursorPosY(GetCursorPosY() + ScaleY(30));
        RenderComponents();
        SetCursorPosY(GetCursorPosY() + ScaleY(40));    

        Separator();
        SetCursorPosY(GetCursorPosY() + ScaleY(5)); 

        Text("Uninstalling from: %s", steamPath.string().c_str());
        Text("Reclaimed Disk Space: %s", GetReclaimedSpace().c_str());

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
        float currentColor = EaseInOutFloat("##NextButton", 1.0f, 0.8f, isButtonHovered, 0.3f);

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

        if (Button("Uninstall", ImVec2(xPos + GetContentRegionAvail().x, GetContentRegionAvail().y)))
        {
            isUninstalling = !isUninstalling;
            for (auto& componentPair : uninstallComponents)
            {
                auto& [state, props] = componentPair.second;
                if (state.isSelected)
                {
                    state.uninstallState.state = ComponentState::UninstallState::Uninstalling;
                }
            }
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