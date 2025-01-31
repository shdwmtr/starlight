#pragma once
#include <minwindef.h>
#include <wtypes.h>
#include <GLFW/glfw3.h>

extern bool isTitleBarHovered;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetBorderlessWindowStyle(GLFWwindow* window);