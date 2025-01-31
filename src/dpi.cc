#include <dpi.h>
#include <iostream>

float XDPI = 1.0f;
float YDPI = 1.0f;

void SetupDPI(GLFWwindow* window)
{
    if (!window)
    {
        return;
    }

    glfwGetWindowContentScale(window, &XDPI, &YDPI);

    std::cout << "DPI Scale: " << XDPI << ", " << YDPI << std::endl;
}

/**
 * I'm developing this in a 4k environment, with a 150% DPI scale, so I'm dividing by 1.5.
 */
float ScaleX(float x)
{
    return (x / 1.5) * XDPI;
}

float ScaleY(float y)
{
    return (y / 1.5) * YDPI;
}