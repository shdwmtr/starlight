#pragma once
#include <string>
#include <router.h>
#include <memory>

// Easing function: ease-in-out quadratic
float EaseInOut(float t);
float EaseInOutTime(float t, float b, float c, float d) ;

struct AnimationState
{
    float currentPosY; // Current position
    float startPosY;   // Start position
    float elapsedTime; // Elapsed time
    bool isAnimating;  // Whether animation is active
    bool wasHovered;   // Last hover state
};

float SmoothFloat(
    const std::string& id, 
    float targetOffset, 
    bool currentState, 
    float displacement, 
    float duration, 
    std::tuple<float, float> colorTransition = std::make_tuple(0.f, 0.f)
);

float TransitionFlatColor(const std::string& id, float lowerBound, float upperBound, bool currentState, float duration);

float AnimateRouteTransition(std::shared_ptr<Router> routerPtr);
float EaseOutBack(float t, float b, float c, float d, float overshoot = 1.20158f);