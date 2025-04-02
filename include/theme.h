#include <imgui.h>
#include <dpi.h>

using namespace ImGui;

void SetupColorScheme()
{
    ImGuiStyle& style = GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.067f, 0.071f, 0.078f, 0.99f);
    colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    style.WindowBorderSize = 0.0f;

    style.WindowPadding = ImVec2(ScaleX(30), ScaleY(30));
    style.ChildRounding = ScaleX(10.0f);
    style.FrameRounding = ScaleX(10.0f);
    style.ScrollbarSize = ScaleX(15.0f);

    // Change text input background color
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f); // Background color of modal window dimming
    colors[ImGuiCol_FrameBg]        = ImVec4(0.098f, 0.102f, 0.11f, 1.0f); // Background color of text input
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.122f, 0.125f, 0.133f, 1.0f); // Background color when hovered
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Background color when active
    colors[ImGuiCol_ScrollbarBg]    = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Background color of scrollbar
    colors[ImGuiCol_ScrollbarGrab]  = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Color of the scrollbar grab

    colors[ImGuiCol_Button]         = ImVec4(0.098f, 0.102f, 0.11f, 1.0f); // Background color of button
    colors[ImGuiCol_ButtonHovered]  = ImVec4(0.122f, 0.125f, 0.133f, 1.0f); // Background color when hovered
    colors[ImGuiCol_ButtonActive]   = ImVec4(0.122f, 0.125f, 0.133f, 1.0f); // Background color when active
}
