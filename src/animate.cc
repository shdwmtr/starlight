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

float EaseInOutFloat(const std::string& id, float lowerBound, float upperBound, bool currentState, float duration)
{
    struct AnimationState 
    {
        bool wasHovered    = false;
        bool isAnimating   = false;
        bool isReversing   = false; 

        float elapsedTime  = 0.0f;
        float currentValue = 0.0f; 
        float minValue     = 0.0f;
        float maxValue     = 1.0f;

        AnimationState() = default;

        AnimationState(float lower, float upper) : wasHovered(false), isAnimating(false), isReversing(false), elapsedTime(0.0f), currentValue(lower), minValue(lower), maxValue(upper) {}
    };


    static std::unordered_map<std::string, AnimationState> animations;

    auto it = animations.find(id);
    if (it == animations.end()) 
    {
        animations[id] = AnimationState(lowerBound, upperBound);
    }

    AnimationState& state = animations[id];

    float deltaTime = ImGui::GetIO().DeltaTime;

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
