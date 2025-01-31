#include <memory>
#include <router.h>

bool       RenderTitleBarComponent();
const void RenderHome(float xPos);
const void RenderInstallPrompt(float xPos);
const void RenderInstaller(float xPos);
const void RenderUninstallSelect(float xPos);

const void RenderCheckBox(bool checked, const char* description, const char* tooltipText = nullptr);