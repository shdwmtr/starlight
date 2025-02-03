#include <memory>
#include <router.h>

bool       RenderTitleBarComponent(std::shared_ptr<RouterNav> router);
const void RenderHome(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstallPrompt(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstaller(std::shared_ptr<RouterNav> router, float xPos);
const void RenderUninstallSelect(std::shared_ptr<RouterNav> router, float xPos);

void StartUninstaller();
const void FetchVersionInfo();

struct CheckBoxState
{
    bool isHovered;
    bool isChecked;

    CheckBoxState(bool isChecked) : isHovered(false), isChecked(false) {
        this->isChecked = isChecked;
    }
};

const CheckBoxState* RenderCheckBox(bool checked, const char* description, const char* tooltipText = nullptr);