#include <renderer.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <components.h>
#include <wndproc.h>

using namespace ImGui;

void RenderImGui(GLFWwindow* window, std::shared_ptr<RouterNav> router)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    ImGuiViewport* viewport = GetMainViewport();
    {
        SetNextWindowPos (viewport->Pos );
        SetNextWindowSize(viewport->Size);

        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        Begin("Millennium", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); 
        PopStyleVar();

        isTitleBarHovered = RenderTitleBarComponent(router);
        router->update(); 

        auto currentPanel         = router->getCurrentComponent();
        auto transitioningPanel   = router->getTransitioningComponent();
        auto xOffsetCurrent       = router->getCurrentOffset(viewport->Size.x);
        auto xOffsetTransitioning = router->getTransitioningOffset(viewport->Size.x);

        if (transitioningPanel) 
        {
            PushID("TransitioningPanel");
            transitioningPanel(router, xOffsetTransitioning);
            PopID();
        }

        SameLine();
        currentPanel(router, xOffsetCurrent);

        if (IsKeyPressed(ImGuiKey_MouseX1))
        {
            if (router->canGoBack()) router->navigateBack();
        }
        if (IsKeyPressed(ImGuiKey_MouseX2))
        {
            if (router->canGoForward()) router->navigateNext();
        }
        End();
        RenderMessageBoxes();
    }
    Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    glfwSwapBuffers(window);
}