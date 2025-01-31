#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>
#include <stdio.h>
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <wtypes.h>
#include <memory.h>
#include <animate.h>
#include <texture.h>
#include <filesystem>
#include <router.h>
#include <iostream>
#include <algorithm>
#include <theme.h>
#include <wndproc.h>

#include <imspinner.h>
#include <thread>
#include <components.h>
#include <dpi.h>

using namespace ImGui;

int WINDOW_WIDTH  = 663;
int WINDOW_HEIGHT = 434;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}



void RenderImGui(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    static std::shared_ptr<RouterNav> router = std::make_shared<RouterNav>(std::vector<Component>{ RenderHome });

    ImGuiViewport* viewport = GetMainViewport();
    {
        SetNextWindowPos (viewport->Pos);
        SetNextWindowSize(viewport->Size);

        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        Begin("Millennium", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar); 
        PopStyleVar();

        isTitleBarHovered = RenderTitleBarComponent(router);

        // ImGui::SetCursorPosY(100);
        router->update(); 

        Component currentPanel       = router->getCurrentComponent();
        Component transitioningPanel = router->getTransitioningComponent();

        float xOffsetCurrent       = router->getCurrentOffset(viewport->Size.x);
        float xOffsetTransitioning = router->getTransitioningOffset(viewport->Size.x);

        if (transitioningPanel) 
        {
            PushID("TransitioningPanel");
            transitioningPanel(router, xOffsetTransitioning);
            PopID();
        }

        ImGui::SameLine();
        PushID("CurrentPanel");
        currentPanel(router, xOffsetCurrent);
        PopID();

        if (ImGui::IsKeyPressed(ImGuiKey_MouseX1))
        {
            router->navigateBack();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_MouseX2))
        {
            router->navigateNext();
        }
        End();
    }
    Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    glfwSwapBuffers(window);
}

void window_refresh_callback(GLFWwindow *window)
{
    RenderImGui(window);
    glfwSwapBuffers(window);
    glFinish();
}

void SetupImGuiScaling(const float scaleFactor) 
{
    ImGuiIO& io = GetIO();

    // Clear fonts
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF((void*)GeistVaraible, sizeof(GeistVaraible), 16.0f * scaleFactor);
    io.Fonts->AddFontFromMemoryTTF((void*)Geist_Bold, sizeof(Geist_Bold), 18.0f * scaleFactor);

    /** Load emoji font and merge it into Geist 18pt */
    const auto fontPath = "C:/Windows/Fonts/seguiemj.ttf";

    if (std::filesystem::exists(fontPath)) {
        static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
        static ImFontConfig cfg;
        cfg.OversampleH = cfg.OversampleV = 1;
        cfg.MergeMode = true;
        cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
        io.Fonts->AddFontFromFileTTF(fontPath, 16.0f * scaleFactor, &cfg, ranges);
    }

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    GLuint fontTexture;
    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    io.Fonts->SetTexID((ImTextureID)(intptr_t)fontTexture);
    io.DisplayFramebufferScale = ImVec2(scaleFactor, scaleFactor);

    ImGuiStyle& style = GetStyle();
    style.ScaleAllSizes(scaleFactor);

    SetupColorScheme();
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height) 
{
    SetupDPI(window);
    SetupImGuiScaling(XDPI);
}

// Main code
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, char** argv)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        MessageBoxA(NULL, "Failed to initialize GLFW", "Error", MB_ICONERROR);
        return 1;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor(); 
    if (!monitor) 
    {
        MessageBoxA(NULL, "Failed to get primary monitor", "Error", MB_ICONERROR);
        glfwTerminate();
        return -1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4); 

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Millennium Installer", nullptr, nullptr);
    if (window == nullptr)
    {
        return 1;
    }

    SetupDPI(window);
    SetBorderlessWindowStyle(window);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) {
        std::cerr << "Failed to get primary monitor\n";
        glfwTerminate();
        return -1;
    }

    int monitorX, monitorY;
    glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);

    const GLFWvidmode* vidMode = glfwGetVideoMode(primaryMonitor);
    if (!vidMode) 
    {
        std::cerr << "Failed to get video mode\n";
        glfwTerminate();
        return -1;
    }

    int posX = monitorX + (vidMode->width - (WINDOW_WIDTH * XDPI)) / 2;
    int posY = monitorY + (vidMode->height - (WINDOW_HEIGHT * YDPI)) / 2;

    glfwSetWindowPos(window, posX, posY);

    glfwSetWindowRefreshCallback(window, window_refresh_callback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    SetupColorScheme();

    SetupImGuiScaling(XDPI);
    LoadTextures();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        RenderImGui(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
