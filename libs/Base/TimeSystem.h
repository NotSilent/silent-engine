#pragma once

#include "../Base/Window.h"
#include <memory>

class TimeManager {
    friend class EngineStatics;

public:
    TimeManager(std::shared_ptr<Window> window);

    ~TimeManager();

    float getDeltaTime() const;

    float getCurrentTime() const;

    uint64_t getCurrentFrame() const;

private:
    void update();

    std::shared_ptr<Window> _window;

    float _deltaTime{};
    float _currentTime{};
    uint64_t _currentFrame;
};
