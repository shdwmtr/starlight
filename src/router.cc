#include <vector>
#include <algorithm>
#include <imgui.h>

enum Route {
    HOME,
    INSTALLER_PROMPT,
    UNINSTALLER_SELECT_REASONS,
    INSTALLER,
    UNINSTALLER_PROMPT,
    UNINSTALLER
};

struct RouteNode {
    Route route;
    std::vector<RouteNode*> next;
    RouteNode* parent = nullptr;  // Add parent pointer
};

// Define nodes
RouteNode home = { HOME, {} };
RouteNode installerSelect = { INSTALLER_PROMPT, {} };
RouteNode uninstallerSelectReasons = { UNINSTALLER_SELECT_REASONS, {} };
RouteNode installer = { INSTALLER, {} };
RouteNode uninstallSelect = { UNINSTALLER_PROMPT, {} };
RouteNode uninstaller = { UNINSTALLER, {} };

void initializeRoutes() {
    // Set up tree structure and parent references
    home.next.push_back(&installerSelect);
    home.next.push_back(&uninstallerSelectReasons);

    installerSelect.parent = &home;
    uninstallerSelectReasons.parent = &home;

    installerSelect.next.push_back(&installer);
    installer.parent = &installerSelect;

    uninstallerSelectReasons.next.push_back(&uninstallSelect);
    uninstallSelect.parent = &uninstallerSelectReasons;

    uninstallSelect.next.push_back(&uninstaller);
    uninstaller.parent = &uninstallSelect;
}

class Router {
public:
    Router();

    void route(Route route);
    void goBack();
    void goForward();
    void goHome();
    void goInstallerPrompt();
    void goInstaller();
    void goUninstallerSelectReasons();
    void goUninstallerPrompt();
    void goUninstaller();

    Route getCurrentRoute();
    float getRoutePosition();

private:
    RouteNode* currentRouteNode;
};

Router::Router() {
    initializeRoutes();
    currentRouteNode = &home;
}

void Router::route(Route route) {
    for (auto& node : currentRouteNode->next) {
        if (node->route == route) {
            currentRouteNode = node;
            break;
        }
    }
}

void Router::goBack() {
    if (currentRouteNode->parent) {
        currentRouteNode = currentRouteNode->parent;
    }
}

void Router::goForward() {
    if (!currentRouteNode->next.empty()) {
        currentRouteNode = currentRouteNode->next[0];
    }
}

void Router::goHome() {
    currentRouteNode = &home;
}

void Router::goInstallerPrompt() {
    currentRouteNode = &installerSelect;
}

void Router::goInstaller() {
    currentRouteNode = &installer;
}

void Router::goUninstallerSelectReasons() {
    currentRouteNode = &uninstallerSelectReasons;
}

void Router::goUninstallerPrompt() {
    currentRouteNode = &uninstallSelect;
}

void Router::goUninstaller() {
    currentRouteNode = &uninstaller;
}

Route Router::getCurrentRoute() {
    return currentRouteNode->route;
}

float Router::getRoutePosition() {
    float position = 0.0f;
    RouteNode* node = currentRouteNode;

    while (node->parent) { // Traverse back using parent pointers
        node = node->parent;
        position += 1.0f;
    }

    return -position * ImGui::GetMainViewport()->Size.x;
}