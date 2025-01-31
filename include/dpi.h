#include <GLFW/glfw3.h>

extern float XDPI;
extern float YDPI;

void SetupDPI(GLFWwindow* monitor);

float ScaleX(float x);
float ScaleY(float y);