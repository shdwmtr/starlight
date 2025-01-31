#include <vector>
#include <algorithm>
#include <imgui.h>
#include <router.h>
#include <math.h>

float easeInOut(float t) 
{
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3) / 2.0f;
}

void RouterNav::navigateNext() 
{
    if (currentIndex + 1 < components.size() && !isAnimating) 
    {
        startAnimation(AnimationDirection::FORWARD);
    }
}

void RouterNav::navigateBack() 
{
    if (currentIndex > 0 && !isAnimating) 
    {
        startAnimation(AnimationDirection::BACKWARD);
    }
}

Component RouterNav::getCurrentComponent() const 
{
    return components[currentIndex];
}

Component RouterNav::getTransitioningComponent() const 
{
    return isAnimating ? components[targetIndex] : nullptr;
}

void RouterNav::update() 
{
    float deltaTime = ImGui::GetIO().DeltaTime;
    if (isAnimating) 
    {
        animTime += deltaTime / animationDuration;
        if (animTime >= 1.0f) 
        {
            animTime = 1.0f;
            isAnimating = false;
            currentIndex = targetIndex;
        }
    }
}

float RouterNav::getCurrentOffset(float viewportWidth) const 
{
    return isAnimating ? lerp(0.0f, -viewportWidth * animDirection, easeInOut(animTime)) : 0.0f;
}

float RouterNav::getTransitioningOffset(float viewportWidth) const 
{
    return isAnimating ? lerp(viewportWidth * animDirection, 0.0f, easeInOut(animTime)) : 0.0f;
}

float RouterNav::lerp(float a, float b, float t) const 
{
    return a + (b - a) * t;
}

void RouterNav::startAnimation(int direction)
{
    isAnimating = true;
    animTime = 0.0f;
    animDirection = direction;
    targetIndex = currentIndex + direction;
}
