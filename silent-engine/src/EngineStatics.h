#pragma once
#include <memory>

class Window;

class Camera;

class InputManager;
class TimeManager;

class EngineStatics {
public:
    EngineStatics(std::shared_ptr<Window> window);

    void update();

    static float getAspectRatio();

    static void setCamera(std::shared_ptr<Camera> camera);
    static std::shared_ptr<Camera> getCamera();

    static std::shared_ptr<InputManager> getInputManager();
    static std::shared_ptr<TimeManager> getTimeManager();

private:
    static std::shared_ptr<Window> _window;

    // Main Camera used to render a frame
    static std::shared_ptr<Camera> _camera;

    static std::shared_ptr<InputManager> _inputManager;
    static std::shared_ptr<TimeManager> _timeManager;
};
