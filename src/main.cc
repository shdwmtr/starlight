#define GL_SILENCE_DEPRECATION
#define GLFW_EXPOSE_NATIVE_WIN32

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <wtypes.h>
#include <memory.h>
#include <atomic>
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
#include <renderer.h>
#include <fmt/format.h>
#include <stb_image.h>
#include <util.h>

using namespace ImGui;

int WINDOW_WIDTH  = 663;
int WINDOW_HEIGHT = 434;

static void GLFWErrorCallback(int error, const char* description)
{
    MessageBoxA(NULL, fmt::format("An error occurred.\n\nGLFW Error {}: {}", error, description).c_str(), "Whoops!", MB_ICONERROR);
}

void WindowRefreshCallback(GLFWwindow *window)
{
    glfwSwapBuffers(window);
    glFinish();
}

std::atomic<bool> shouldSetupScaling {false};
GLFWwindow* g_Window = nullptr;

void SetupImGuiScaling(GLFWwindow* window) 
{
    SetupDPI(window);
    float scaleFactor = XDPI;

    std::cout << "Setting up ImGui scaling with factor: " << scaleFactor << std::endl;
    ImGuiIO& io = GetIO();

    // Clear fonts
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF((void*)GeistVaraible, sizeof(GeistVaraible), 16.0f * scaleFactor);
    io.Fonts->AddFontFromMemoryTTF((void*)Geist_Bold,    sizeof(Geist_Bold),    18.0f * scaleFactor);

    /** Load emoji font and merge it into Geist 18pt */
    const auto fontPath = "C:/Windows/Fonts/seguiemj.ttf";

    if (std::filesystem::exists(fontPath)) 
    {
        static ImFontConfig cfg;
        static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };

        cfg.OversampleH = cfg.OversampleV = 1;
        cfg.MergeMode   = true;
        cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
        io.Fonts->AddFontFromFileTTF(fontPath, 16.0f * scaleFactor, &cfg, ranges);
    }

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    GLuint fontTexture;

    glGenTextures  ( 1, &fontTexture );
    glBindTexture  ( GL_TEXTURE_2D, fontTexture );
    glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    io.Fonts->SetTexID((ImTextureID)(intptr_t)fontTexture);
    io.DisplayFramebufferScale = ImVec2(scaleFactor, scaleFactor);

    ImGuiStyle& style = GetStyle();
    style = ImGuiStyle(); // Reset style
    style.ScaleAllSizes(scaleFactor);

    SetupColorScheme();
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height) 
{
    g_Window = window;
    shouldSetupScaling.store(true, std::memory_order_relaxed);
}

void SpawnRendererThread(GLFWwindow* window, const char* glsl_version, std::shared_ptr<RouterNav> router)
{
    glfwMakeContextCurrent(window); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    SetupColorScheme();

    SetupImGuiScaling(window);
    SetBorderlessWindowStyle(window, router);
    LoadTextures();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) 
    {
        if (shouldSetupScaling.load(std::memory_order_relaxed)) 
        {
            SetupImGuiScaling(window);
            shouldSetupScaling.store(false, std::memory_order_relaxed);
        }

        RenderImGui(window, router);

        static bool hasShown = false;

        if (!hasShown)
        {
            glfwShowWindow(window);
            hasShown = true;
        }
    }
}

void setWindowIconFromMemory(GLFWwindow* window, const unsigned char* imageData, int width, int height) 
{
    GLFWimage icon[1];
    icon[0].pixels = const_cast<unsigned char*>(imageData);
    icon[0].width = width;
    icon[0].height = height;

    glfwSetWindowIcon(window, 1, icon);
}

// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, char** argv)
{
    glfwSetErrorCallback(GLFWErrorCallback);
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
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR,      GLFW_TRUE  );
    glfwWindowHint( GLFW_RESIZABLE,             GLFW_FALSE );
    glfwWindowHint( GLFW_VISIBLE,               GLFW_FALSE );
    glfwWindowHint( GLFW_SAMPLES,               4 ); 

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Steam Homebrew", nullptr, nullptr);
    if (window == nullptr)
    {
        return 1;
    }

    SetupDPI(window);

    // Load image from memory
    int width, height, channels;
    ColorScheme colorScheme = GetWindowsColorScheme();

    unsigned char* data = stbi_load_from_memory(
        colorScheme ==  Light ? windowIconDark : windowIconLight, 
        colorScheme ==  Light ? sizeof(windowIconDark) : sizeof(windowIconLight), 
        &width, &height, &channels, 4
    ); 

    if (data)
    {
        setWindowIconFromMemory(window, data, width, height);
        stbi_image_free(data);
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) 
    {
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

    glfwSetWindowRefreshCallback  (window, WindowRefreshCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr;

    std::shared_ptr<RouterNav> router = std::make_shared<RouterNav>(std::vector<Component>{ RenderHome });
    auto rendererThread = std::thread(SpawnRendererThread, window, glsl_version, router);

    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
    }

    rendererThread.join();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
