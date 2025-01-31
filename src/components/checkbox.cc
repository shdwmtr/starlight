#include <components.h>
#include <dpi.h>
#include <router.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <unordered_map>
#include <animate.h>

using namespace ImGui;

const void RenderCheckBox(bool checked, const char* description, const char* tooltipText)
{
    struct CheckBoxState
    {
        bool isHovered;
        bool isChecked;

        CheckBoxState(bool isChecked) : isHovered(false), isChecked(false) {
            this->isChecked = isChecked;
        }
    };

    static std::unordered_map<std::string, CheckBoxState> checkBoxStates;
    auto& state = checkBoxStates.try_emplace(description, checked).first->second;

    PushStyleVar  (ImGuiStyleVar_FrameRounding,   10.0f                               );
    PushStyleVar  (ImGuiStyleVar_FrameBorderSize, 2.0f                               );
    PushStyleColor(ImGuiCol_Border,               ImVec4(0.18f, 0.184f, 0.192f, 1.0f));
    PushStyleColor(ImGuiCol_CheckMark,            ImVec4(1.0f, 1.0f, 1.0f, 1.0f)     );
    PushStyleColor(ImGuiCol_FrameBgActive,        ImVec4(0.098f, 0.102f, 0.11f, 1.0f));

    std::string checkBoxMessage = "  " + std::string(description);
    Checkbox(checkBoxMessage.c_str(), &state.isChecked);

    if (tooltipText)
    {
        float autoUpdateColor = EaseInOutFloat(description, 0.f, 1.f, state.isHovered, 0.3f);

        if (autoUpdateColor)
        {
            SetMouseCursor(ImGuiMouseCursor_Hand);

            PushStyleVar  (ImGuiStyleVar_WindowPadding,  ImVec2(ScaleX(10), ScaleY(10))     );
            PushStyleVar  (ImGuiStyleVar_WindowRounding, 6.0f                               );
            PushStyleVar  (ImGuiStyleVar_Alpha,          autoUpdateColor                    );
            PushStyleColor(ImGuiCol_PopupBg,             ImVec4(0.098f, 0.102f, 0.11f, 1.0f));

            SetTooltip(tooltipText);

            PopStyleVar  (3);
            PopStyleColor();
        }

        state.isHovered = IsItemHovered() || (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseDown(ImGuiMouseButton_Left));
    }

    PopStyleColor(3);
    PopStyleVar(2);
}