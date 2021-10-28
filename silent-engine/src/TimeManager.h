#pragma once

#include "Window.h"
#include <memory>

class TimeManager {
public:
    TimeManager(std::shared_ptr<Window> window);
    ~TimeManager();

    void update();

    float getDeltaTime() const;
    float getCurrentTime() const;
    uint64_t getCurrentFrame() const;

private:
    std::shared_ptr<Window> _window;

    float _deltaTime{};
    float _currentTime{};
    uint64_t _currentFrame;
};
