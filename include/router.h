#pragma once
#include <functional>
#include <vector>
#include <memory>

class RouterNav;
using Component = std::function<void(std::shared_ptr<RouterNav> router, float xOffset)>;

enum AnimationDirection 
{
    FORWARD = 1,
    BACKWARD = -1
};

class RouterNav 
{
public:
    RouterNav(const std::vector<Component>& components) : components(components), currentIndex(0), isAnimating(false), animTime(0.0f) {}

    void navigateNext();
    void navigateBack();

    bool canGoBack()    const { return softCanGoBack && currentIndex > 0;                        }
    bool canGoForward() const { return softCanGoForward && currentIndex + 1 < components.size(); }

    void setCanGoBack(const bool newValue);
    void setCanGoForward(const bool newValue);

    Component getCurrentComponent() const;
    Component getTransitioningComponent() const;

    void update();

    float getCurrentOffset(float viewportWidth) const;
    float getTransitioningOffset(float viewportWidth) const;

    void setComponents(const std::vector<Component>& components) { this->components = components; }
private:
    std::vector<Component> components;

    size_t currentIndex;
    size_t targetIndex;

    bool isAnimating;
    float animTime;
    int animDirection;
    const float animationDuration = 0.3f;

    bool softCanGoBack = true;
    bool softCanGoForward = true;

    float lerp(float a, float b, float t) const;
    void startAnimation(int direction);
};
