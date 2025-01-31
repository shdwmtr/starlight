#include <animate.h>
#include <cmath> 
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <imgui.h>
#include <memory>

/** Based on https://stackoverflow.com/questions/13462001/ease-in-and-ease-out-animation-formula */
float EaseInOut(float t)
{
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

float TransitionFlatColor(const std::string& id, float lowerBound, float upperBound, bool currentState, float duration)
{
    struct AnimationState {
        bool wasHovered = false;
        bool isAnimating = false;
        bool isReversing = false;
        float elapsedTime = 0.0f;
        float currentValue = 0.0f;  // Default to 0
        float minValue = 0.0f;
        float maxValue = 1.0f;

        // Default constructor (needed for unordered_map)
        AnimationState() = default;

        // Constructor with values
        AnimationState(float lower, float upper)
            : wasHovered(false), isAnimating(false), isReversing(false),
            elapsedTime(0.0f), currentValue(lower), minValue(lower), maxValue(upper) {}
    };


    static std::unordered_map<std::string, AnimationState> animations;

    // Ensure the animation state exists with correct bounds
    auto it = animations.find(id);
    if (it == animations.end()) {
        animations[id] = AnimationState(lowerBound, upperBound);
    }

    AnimationState& state = animations[id];

    float deltaTime = ImGui::GetIO().DeltaTime;

    // Detect state change
    if (currentState != state.wasHovered)
    {
        state.wasHovered = currentState;
        state.isAnimating = true;
        state.isReversing = !currentState;
        state.elapsedTime = 0.0f;
    }

    if (state.isAnimating)
    {
        state.elapsedTime += deltaTime;
        float progress = state.elapsedTime / duration;

        if (progress >= 1.0f)
        {
            progress = 1.0f;
            state.isAnimating = false;
        }

        float easedProgress = EaseInOut(progress);

        // Instead of using function parameters, use stored min/max values
        float startValue = state.currentValue;
        float targetValue = state.isReversing ? state.minValue : state.maxValue;

        state.currentValue = startValue + (targetValue - startValue) * easedProgress;
    }

    return state.currentValue;
}

/**
 * Smoothly float an element to a target offset.
 * @param id The unique identifier for the animation.
 * @param targetOffset The target offset.
 * @param currentState The current state.
 * @param displacement How high the element should float, relative to 0.
 * @param duration The duration of the animation.
 * 
 * 
 */
float SmoothFloat(const std::string& id, float targetOffset, bool currentState, float displacement, float duration, std::tuple<float, float> colorTransition)
{
    static std::unordered_map<std::string, AnimationState> animations;
    const auto [lowerBound, upperBound] = colorTransition;

    AnimationState& state = animations[id];
    if (state.currentPosY == 0.0f)
        state.currentPosY = ImGui::GetCursorPosY();

    float deltaTime = ImGui::GetIO().DeltaTime;

    if (currentState != state.wasHovered)
    {
        state.wasHovered = currentState;
        state.isAnimating = true;
        state.elapsedTime = 0.0f;
        state.startPosY = state.currentPosY;
    }
    float targetPosY = currentState ? targetOffset - displacement : targetOffset;

    float result = upperBound;

    if (state.isAnimating)
    {
        state.elapsedTime += deltaTime;

        float progress = state.elapsedTime / duration;
        if (progress >= 1.0f)
        {
            progress = 1.0f;
            state.isAnimating = false; 
        }

        float easedProgress = EaseInOut(progress);

        result = lowerBound + (upperBound - lowerBound) *  easedProgress;
        state.currentPosY = state.startPosY + (easedProgress * (targetPosY - state.startPosY));
    }
    ImGui::SetCursorPosY(state.currentPosY);

    return result;
}

float AnimateRouteTransition(std::shared_ptr<Router> routerPtr)
{
    static float ANIMATION_DURATION = 0.2f;

    static float startY   = 0;
    static float targetY  = 0;  
    static float currentY = startY; 

    static bool animate   = false;
    static auto startTime = std::chrono::steady_clock::now();

    static float lastTargetY = targetY;

    if (targetY != lastTargetY) 
    {
        startY = currentY; 
        lastTargetY = targetY;
        startTime = std::chrono::steady_clock::now();
        animate = true;
    }

    if (animate) 
    {
        auto now = std::chrono::steady_clock::now();
        float elapsedTime = std::chrono::duration<float>(now - startTime).count();

        if (elapsedTime < ANIMATION_DURATION) 
        {
            currentY = EaseInOutTime(elapsedTime, startY, targetY - startY, ANIMATION_DURATION);
        } 
        else 
        {
            currentY = targetY;
            animate = false;
        }
    }

    // targetY = routerPtr->getRoutePosition();
    return currentY;
}

float EaseInOutTime(float t, float b, float c, float d) 
{
    t /= d / 2;
    if (t < 1) return c / 2 * t * t + b;
    t--;
    return -c / 2 * (t * (t - 2) - 1) + b;
}

/** Based on https://stackoverflow.com/questions/5699091/how-to-implement-easeoutback-easing-in-css-transform-animation */
float EaseOutBack(float t, float b, float c, float d, float overshoot) 
{
    t = t / d - 1;
    return c * (t * t * ((overshoot + 1) * t + overshoot) + 1) + b;
}
