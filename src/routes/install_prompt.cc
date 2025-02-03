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
#include <windows.h>
#include <shobjidl.h>
#include <commdlg.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <http.h>
#include <fmt/format.h>
#include <util.h>

using namespace ImGui;

static std::string steamPath = "C:\\Program Files (x86)\\Steam";

std::string OpenFolderDialog()
{
    const auto WStringToString = [](const std::wstring& wstr) -> std::string {
        if (wstr.empty()) return {};

        size_t size_needed = std::wcstombs(nullptr, wstr.c_str(), 0) + 1;
        std::string str(size_needed, 0);
        std::wcstombs(&str[0], wstr.c_str(), size_needed);
        str.pop_back(); 
        return str;
    };

    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
    {
        return {};
    }

    IFileOpenDialog* pFileOpen = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen))))
    {
        CoUninitialize();
        return {};
    }

    unsigned long options;
    if (SUCCEEDED(pFileOpen->GetOptions(&options)))
    {
        pFileOpen->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
    }

    if (FAILED(pFileOpen->Show(nullptr)))
    {
        pFileOpen->Release();
        CoUninitialize();
        return {};
    }

    IShellItem* pItem = nullptr;
    if (FAILED(pFileOpen->GetResult(&pItem)))
    {
        pFileOpen->Release();
        CoUninitialize();
        return {};
    }

    wchar_t* folderPath = nullptr;
    std::string result;
    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath)))
    {
        if (folderPath)
        {
            result = WStringToString(folderPath);
            CoTaskMemFree(folderPath);
        }
    }

    pItem->Release();
    pFileOpen->Release();
    CoUninitialize();

    return result;
}

void SelectNewSteamPath()
{
    const auto steamPath = std::filesystem::path(OpenFolderDialog());

    if (steamPath.empty())
    {
        return;
    }

    if (!std::filesystem::exists(steamPath / "steam.exe"))
    {
        MessageBoxA(nullptr, "Invalid Steam path selected", "Error", MB_ICONERROR);
        return;
    }

    ::steamPath = steamPath.string();
}

nlohmann::json releaseInfo, osReleaseInfo;

std::string ToTimeAgo(const std::string& isoTimestamp) 
{
    std::tm tm = {};
    std::istringstream ss(isoTimestamp);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ"); // UTC time format

    if (ss.fail()) 
    {
        return "Invalid timestamp";
    }

    // Convert to time_point
    std::time_t inputTime = std::mktime(&tm);
    auto inputTimePoint = std::chrono::system_clock::from_time_t(inputTime);
    auto now = std::chrono::system_clock::now();

    // Calculate difference in seconds
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - inputTimePoint).count();

    // Helper lambda to handle singular/plural formatting
    auto formatTime = [](long count, const char* unit) 
    {
        return fmt::format("{} {}{}", count, unit, (count == 1 ? "" : "s"));
    };

    if (diff < 60)       return "just now";
    if (diff < 3600)     return formatTime(diff / 60, "minute") + " ago";
    if (diff < 86400)    return formatTime(diff / 3600, "hour") + " ago";
    if (diff < 2592000)  return formatTime(diff / 86400, "day") + " ago";
    if (diff < 31536000) return fmt::format("about {} ago", formatTime(diff / 2592000, "month"));
    return fmt::format("about {} ago", formatTime(diff / 31536000, "year"));
}

const void FetchVersionInfo()
{
    const auto response = Http::Get("https://api.github.com/repos/shdwmtr/millennium/releases", false);

    if (response.empty())
    {
        MessageBoxA(nullptr, "Failed to fetch version info", "Error", MB_ICONERROR);
        return;
    }

    releaseInfo = nlohmann::json::parse(response);

    // Filter and find first release that isn't a pre-release
    for (const auto& release : releaseInfo)
    {
        if (!release["prerelease"].get<bool>())
        {
            releaseInfo = release;

            // Fetch OS release info
            for (const auto& asset : releaseInfo["assets"])
            {
                std::string assetName  = asset["name"];
                std::string releaseTag = releaseInfo["tag_name"];

                #ifdef WIN32
                if (assetName == fmt::format("millennium-{}-windows-x86_64.zip", releaseTag))
                #elif __linux__
                if (assetName == fmt::format("millennium-{}-linux-x86_64.tar.gz", release["tag_name"].get<std::string>()))
                #endif
                {
                    osReleaseInfo = asset;
                    break;
                }
            }

            break;
        }
    }
}

const void RenderInstallPrompt(std::shared_ptr<RouterNav> router, float xPos)
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
        Text(fmt::format(u8"Install Millennium {} ✨", releaseInfo["tag_name"].get<std::string>()).c_str());
        PopFont();

        Spacing();
        PushStyleColor(ImGuiCol_Text, ImVec4(0.422f, 0.425f, 0.441f, 1.0f));
        TextWrapped(fmt::format("Released {}", ToTimeAgo(releaseInfo["published_at"].get<std::string>())).c_str());

        Spacing();
        Separator();

        Spacing();
        Spacing();

        Text("Steam Path:");
        Spacing();
        Spacing();
        PopStyleColor();

        PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
        PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ScaleX(10), ScaleY(10)));
        PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        PushStyleColor(ImGuiCol_Border, ImVec4(0.18f, 0.184f, 0.192f, 1.0f));
        PushItemWidth(GetContentRegionAvail().x - ScaleX(55));
        InputText("##SteamPath", &steamPath, ImGuiInputTextFlags_ReadOnly);
        PopItemWidth();

        SameLine();
        PushStyleColor(ImGuiCol_Button, ImVec4(0.098f, 0.102f, 0.11f, 1.0f));

        PushFont(io.Fonts->Fonts[1]);
        float buttonCursorPosX = GetCursorPosX();

        if (Button("...", ImVec2(GetContentRegionAvail().x, ScaleY(44))))
        {
            SelectNewSteamPath();
        }
        PopFont();

        static bool isOpenFolderHovered = false;
        float currentColor = EaseInOutFloat("##OpenFolderButton", 0.f, 1.f, isOpenFolderHovered, 0.3f);

        /** Check if the animation has started */
        if (currentColor != 0.f)
        {
            SetMouseCursor(ImGuiMouseCursor_Hand);
            SetNextWindowPos(ImVec2(buttonCursorPosX - ScaleX(183), GetCursorPosY() + ScaleY(120)));

            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ScaleX(10), ScaleY(10)));
            PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            PushStyleVar(ImGuiStyleVar_Alpha, currentColor);
            PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.098f, 0.102f, 0.11f, 1.0f));
            BeginTooltip();
            Text("Select Steam installation path");
            EndTooltip();
            PopStyleVar(3);
            PopStyleColor();
        }

        isOpenFolderHovered = IsItemHovered() || (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseDown(ImGuiMouseButton_Left));

        PopStyleColor(2);
        PopStyleVar(3);

        PushStyleColor(ImGuiCol_Text, ImVec4(0.422f, 0.425f, 0.441f, 1.0f));

        SetCursorPosY(GetCursorPosY() + ScaleY(30));

        RenderCheckBox(
            true, 
            "Automatically check for updates", 
            "With this enabled, Millennium will automatically check for updates.\nIf updates are found, you will be prompted to install them."
        );

        Spacing();

        RenderCheckBox(
            true, 
            "Automatically install found updates", 
            "With this and the setting above enabled, you will NOT be prompted to\ninstall updates. They will be carried out automatically."
        );

        Spacing();
        Spacing();
        Spacing();
        Spacing();

        Text("Download Information:");
        BulletText("Size: %s", BytesToReadableFormat(osReleaseInfo["size"].get<int>()).c_str());
        BulletText("Name: %s", osReleaseInfo["name"].get<std::string>().c_str());

        PopStyleColor();
    }
    EndChild();
    PopStyleColor();

    SetCursorPos(ImVec2(xPos, viewport->Size.y - BottomNavBarHeight + 1));

    PushStyleVar  (ImGuiStyleVar_WindowPadding, ImVec2(ScaleX(30), ScaleY(30)));
    PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    PushStyleVar  (ImGuiStyleVar_ChildRounding, 0.0f);
    PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.078f, 0.082f, 0.09f, 1.0f));

    BeginChild("##BottomNavBar", ImVec2(viewport->Size.x, BottomNavBarHeight), true, ImGuiWindowFlags_NoScrollbar);
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