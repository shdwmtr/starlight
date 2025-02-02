#pragma once
#include <minwindef.h>
#include <wtypes.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <router.h>

extern bool isTitleBarHovered;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetBorderlessWindowStyle(GLFWwindow* window, std::shared_ptr<RouterNav> router);