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
#include "imgui_markdown.h" 

using namespace ImGui;

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

std::string SelectNewSteamPath()
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

    return steamPath.string();
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

const bool FetchVersionInfo()
{
    const auto response = Http::Get("https://api.github.com/repos/shdwmtr/millennium/releases", false);

    if (response.empty())
    {
        ShowMessageBox("Whoops!", "Failed to fetch version information from the GitHub API! Make sure you have a valid internet connection.", Error);
        return false;
    }

    bool hasFoundReleaseInfo = false;

    try 
    {
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
                        hasFoundReleaseInfo = true;
                        break;
                    }
                }
    
                break;
            }
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        ShowMessageBox("Whoops!", "Failed to parse version information from the GitHub API! Please wait a moment and try again, you're likely rate limited.", Error);
        return false;
    }
    return hasFoundReleaseInfo;
}

void LinkCallback( ImGui::MarkdownLinkCallbackData data_ );
inline ImGui::MarkdownImageData ImageCallback( ImGui::MarkdownLinkCallbackData data_ );

static ImGui::MarkdownConfig mdConfig; 


void LinkCallback( ImGui::MarkdownLinkCallbackData data_ )
{
    std::string url( data_.link, data_.linkLength );
    if( !data_.isImage )
    {
        ShellExecuteA( NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL );
    }
}

inline ImGui::MarkdownImageData ImageCallback( ImGui::MarkdownLinkCallbackData data_ )
{
    // In your application you would load an image based on data_ input. Here we just use the imgui font texture.
    ImTextureID image = ImGui::GetIO().Fonts->TexID;
    // > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
    ImGui::MarkdownImageData imageData;
    imageData.isValid =         true;
    imageData.useLinkCallback = false;
    imageData.user_texture_id = image;
    imageData.size =            ImVec2( 40.0f, 20.0f );

    // For image resize when available size.x > image width, add
    ImVec2 const contentSize = ImGui::GetContentRegionAvail();
    if( imageData.size.x > contentSize.x )
    {
        float const ratio = imageData.size.y/imageData.size.x;
        imageData.size.x = contentSize.x;
        imageData.size.y = contentSize.x*ratio;
    }

    return imageData;
}

void ExampleMarkdownFormatCallback(const MarkdownFormatInfo& markdownFormatInfo_, bool start_)
{
    defaultMarkdownFormatCallback(markdownFormatInfo_, start_);        
       
    switch(markdownFormatInfo_.type)
    {
        case MarkdownFormatType::HEADING:
        {
            if(start_)
            {
                PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
            }
            else
            {
                PopStyleColor();
            }
            break;
        }

        case MarkdownFormatType::LINK:
        {
            if(start_)
            {
                PushStyleColor(ImGuiCol_Text, ImVec4(0.408f, 0.525f, 0.91f, 1.0f));
            }
            else
            {
                if (IsItemHovered())
                {
                    SetMouseCursor(ImGuiMouseCursor_Hand);
                }

                PopStyleColor();
            }
            break;
        }
    }
}

void replaceNewlines(std::string& str) {
    size_t pos = 0;
    while ((pos = str.find("\n\n", pos)) != std::string::npos) {
        str.replace(pos, 2, "\n");
    }
}

void removeFirstLine(std::string& str) {
    size_t pos = str.find("\n\n\n");
    if (pos != std::string::npos) {
        str.erase(0, pos + 1); 
    }
}

void Markdown(std::string markdown_)
{
    removeFirstLine(markdown_);
    replaceNewlines(markdown_);

    static bool hasLogged = false;

    if (!hasLogged)
    {
        std::cout << "Markdown: " << markdown_ << std::endl;
        hasLogged = true;
    }

    ImGuiIO& io = GetIO();
    // You can make your own Markdown function with your prefered string container and markdown config.
    // > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
    mdConfig.linkCallback =         LinkCallback;
    mdConfig.tooltipCallback =      NULL;
    mdConfig.imageCallback =        ImageCallback;
    mdConfig.headingFormats[0] =    { io.Fonts->Fonts[1], true };
    mdConfig.headingFormats[1] =    { io.Fonts->Fonts[1], true };
    mdConfig.headingFormats[2] =    { io.Fonts->Fonts[1], false };
    mdConfig.userData =             NULL;
    mdConfig.formatCallback =       ExampleMarkdownFormatCallback;
    ImGui::Markdown( markdown_.c_str(), markdown_.length(), mdConfig );
}

const void RenderInstallPrompt(std::shared_ptr<RouterNav> router, float xPos)
{
    static std::string steamPath = GetSteamPath();

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
        Text(fmt::format("Install Millennium {} ✨", releaseInfo["tag_name"].get<std::string>()).c_str());
        PopFont();

        Spacing();
        PushStyleColor(ImGuiCol_Text, ImVec4(0.422f, 0.425f, 0.441f, 1.0f));
        TextWrapped(fmt::format("Released {} • ", ToTimeAgo(releaseInfo["published_at"].get<std::string>())).c_str());
        SameLine(0, ScaleX(5));
        TextColored(ImVec4(0.408f, 0.525f, 0.91f, 1.0f), "View Release Notes");

        if (IsItemHovered())
        {
            SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        static bool showModal = false;
        static bool hasSkippedFirstFrame = false;

        if (IsItemClicked())
        {
            showModal = true;
            hasSkippedFirstFrame = false;
            OpenPopup("ReleaseNotes");
        }

        PushStyleVar(ImGuiStyleVar_WindowRounding, ScaleX(10.0f));
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, ScaleX(1.0f));
        PushStyleColor(ImGuiCol_Border, ImVec4(0.48f, 0.484f, 0.492f, 0.5f));

        SetNextWindowBgAlpha(1.f);
        SetNextWindowSize(ImVec2(ScaleX(825), ScaleY(450)));
        
        static float scrollPosition = 0.0f; 
        static float scrollVelocity = 0.0f; 
        static const float scrollSpeed = 10.0f; 
        static const float scrollFriction = 0.95f;
        static const float minVelocityThreshold = 0.01f;

        float opacityAnimation = EaseInOutFloat("ReleaseNotesAnimation", 0.f, 1.f, showModal, 0.3f);
        PushStyleVar(ImGuiStyleVar_Alpha, opacityAnimation);
        PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.098f, 0.102f, 0.11f, 1.0f));

        if (BeginPopupModal("ReleaseNotes", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse))
        {
            if (io.MouseWheel != 0.0f)
            {
                scrollVelocity -= io.MouseWheel * scrollSpeed;
            }
        
            scrollVelocity *= scrollFriction;
            scrollPosition += scrollVelocity; 
        
            const auto cursorPos = GetCursorPosY();
            PushFont(io.Fonts->Fonts[1]);
            PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
            Text("Release Notes:");
            SetCursorPosY(GetCursorPosY() + ScaleY(10));
            Separator();
            PopStyleColor();
            PopFont();
        
            SetCursorPosY(cursorPos);
            Markdown(releaseInfo["body"].get<std::string>());

            scrollPosition = ImClamp(scrollPosition, 0.0f, GetScrollMaxY());

            if (fabs(scrollVelocity) < minVelocityThreshold)
            {
                scrollVelocity = 0.0f;
            }

            SetScrollY(scrollPosition);
            if (hasSkippedFirstFrame && !ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) 
            {
                showModal = false;
                CloseCurrentPopup();
            }           
            hasSkippedFirstFrame = true;

            // if (opacityAnimation <= 0.f && !showModal)
            // {
            //     std::cout << "done" << std::endl;
            //     CloseCurrentPopup();
            // }

            EndPopup();
        }
        
        PopStyleVar(3);
        PopStyleColor(2);

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
            steamPath = SelectNewSteamPath();
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

        const CheckBoxState* state = RenderCheckBox(
            true, 
            "Automatically check for updates", 
            "With this enabled, Millennium will automatically check for updates.\nIf updates are found, you will be prompted to install them."
        );

        Spacing();

        RenderCheckBox(
            true, 
            "Automatically install found updates", 
            "With this and the setting above enabled, you will NOT be prompted to\ninstall updates. They will be carried out automatically.",
            !state->isChecked
        );

        Spacing();
        Spacing();
        Spacing();
        Spacing();

        BulletText("Size: %s", BytesToReadableFormat(osReleaseInfo["size"].get<int>()).c_str());
        BulletText("Name: %s", osReleaseInfo["name"].get<std::string>().c_str());

        PopStyleColor();
    }
    EndChild();
    PopStyleColor();

    RenderBottomNavBar("InstallPrompt", xPos, [router, xPos] 
    {
        static bool isButtonHovered = false;
        float currentColor = EaseInOutFloat("##InstallButton", 1.0f, 0.8f, isButtonHovered, 0.3f);

        PushStyleColor(ImGuiCol_Button,        ImVec4(currentColor, currentColor, currentColor, 1.0f));
        PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(currentColor, currentColor, currentColor, 1.0f));

        if (Button("Install", ImVec2(xPos + GetContentRegionAvail().x, GetContentRegionAvail().y)))
        {
            std::thread(StartInstaller, steamPath, std::ref(releaseInfo), std::ref(osReleaseInfo)).detach();
            router->navigateNext();
        }

        if (isButtonHovered)
        {
            SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        PopStyleColor(2);
        isButtonHovered = IsItemHovered() || (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseDown(ImGuiMouseButton_Left));
    });
}
