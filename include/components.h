#include <memory>
#include <router.h>

bool       RenderTitleBarComponent(std::shared_ptr<RouterNav> router);
const void RenderHome(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstallPrompt(std::shared_ptr<RouterNav> router, float xPos);
const void RenderInstaller(std::shared_ptr<RouterNav> router, float xPos);
const void RenderUninstallSelect(std::shared_ptr<RouterNav> router, float xPos);

const void RenderCheckBox(bool checked, const char* description, const char* tooltipText = nullptr);