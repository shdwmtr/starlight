#pragma once    

#include <memory>
#include <router.h>
#include <nlohmann/json.hpp>

bool       RenderTitleBarComponent(std::shared_ptr<RouterNav> router);
const void RenderHome(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstallPrompt(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstaller(std::shared_ptr<RouterNav> router, float xPos);
const void RenderUninstallSelect(std::shared_ptr<RouterNav> router, float xPos);

const void RenderBottomNavBar(const char* identifier, float xPos, std::function<void()> buttonRenderCallback);

void StartInstaller(std::string steamPath, nlohmann::json& releaseInfo, nlohmann::json& osReleaseInfo);
void StartUninstaller();
const bool FetchVersionInfo();

enum MessageLevel {
    Error,
    Info,
    Warning
};

void ShowMessageBox(std::string title, std::string body, MessageLevel level);
void RenderMessageBoxes();

struct CheckBoxState
{
    bool isHovered;
    bool isChecked;

    CheckBoxState(bool isChecked) : isHovered(false), isChecked(false) {
        this->isChecked = isChecked;
    }
};

const CheckBoxState* RenderCheckBox(bool checked, const char* description, const char* tooltipText = nullptr, bool disabled = false, bool endChild = false);