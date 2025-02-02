#pragma once
#include <string>
#include <router.h>
#include <memory>
#include <dpi.h>

// Easing function: ease-in-out quadratic
float EaseInOut(float t);
float EaseInOutTime(float t, float b, float c, float d) ;

struct AnimationState
{
    float currentPosY;
    float startPosY;
    float elapsedTime; 
    bool  isAnimating;
    bool  wasHovered;

    float lastXDPI = XDPI, lastYDPI = YDPI;
};

float SmoothFloat(
    const std::string& id, 
    float targetOffset, 
    bool currentState, 
    float displacement, 
    float duration, 
    std::tuple<float, float> colorTransition = std::make_tuple(0.f, 0.f)
);

float EaseInOutFloat(const std::string& id, float lowerBound, float upperBound, bool currentState, float duration);

float EaseOutBack(float t, float b, float c, float d, float overshoot = 1.20158f);