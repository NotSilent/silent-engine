#include "TimeManager.h"

TimeManager::TimeManager(std::shared_ptr<Window> window)
{
	_window = window;
}

TimeManager::~TimeManager()
{
}

void TimeManager::update()
{
    float currentTime = static_cast<float>(glfwGetTime());
    _deltaTime = currentTime - _currentTime;
    _currentTime = currentTime;

    _currentFrame++;
}

float TimeManager::getDeltaTime() const
{
    return _deltaTime;
}

float TimeManager::getCurrentTime() const
{
    return _currentTime;
}

uint64_t TimeManager::getCurrentFrame() const
{
    return _currentFrame;
}
