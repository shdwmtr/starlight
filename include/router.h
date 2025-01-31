#pragma once

enum Route {
    HOME,
    INSTALLER_PROMPT,
    INSTALLER,
    UNINSTALLER_SELECT_REASONS,
    UNINSTALLER_PROMPT,
    UNINSTALLER,
};

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
    Route currentRoute;
    Route previousRoute;
};
