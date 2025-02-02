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