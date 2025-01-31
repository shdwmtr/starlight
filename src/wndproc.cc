#define GLFW_EXPOSE_NATIVE_WIN32
#include <wndproc.h>
#include <GLFW/glfw3native.h>

WNDPROC g_OriginalWindProcCallback;
bool    isTitleBarHovered = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_NCCALCSIZE:
        {
            if (wParam && lParam)
            {
                NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                pParams->rgrc[0].top += 1;
                pParams->rgrc[0].right -= 1;
                pParams->rgrc[0].bottom -= 1;
                pParams->rgrc[0].left += 1;
            }
            return 0;
        }
        case WM_NCPAINT:
        {
            return 1;
        }
        case WM_DESTROY: 
        {
            PostQuitMessage(0);
            break;
        }
        case WM_NCHITTEST:
        {
            if (isTitleBarHovered) 
            {
                return HTCAPTION;
            }
            return HTCLIENT;
        }
        case WM_NCACTIVATE:
        {
            return false;
        }
    }
    
    return CallWindowProc(g_OriginalWindProcCallback, hWnd, uMsg, wParam, lParam);
}

void SetBorderlessWindowStyle(GLFWwindow* window)
{
    HWND hWnd = glfwGetWin32Window(window);

    LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    lStyle |= WS_THICKFRAME;
    lStyle &= ~WS_CAPTION;
    SetWindowLongPtr(hWnd, GWL_STYLE, lStyle);

    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);
    int width  = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    g_OriginalWindProcCallback = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
    SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE);
}