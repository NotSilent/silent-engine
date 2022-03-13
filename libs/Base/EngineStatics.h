#pragma once
#include <memory>

class Window;

class Camera;

class InputSystem;
class TimeManager;

class EngineStatics {
public:
    explicit EngineStatics(const std::shared_ptr<Window>& window);

    static void update();

    static float getAspectRatio();

    static void setCamera(std::shared_ptr<Camera> camera);
    static std::shared_ptr<Camera> getCamera();

    static std::shared_ptr<InputSystem> getInputManager();
    static std::shared_ptr<TimeManager> getTimeManager();

private:
    static std::shared_ptr<Window> _window;

    // Main Camera used to render a frame
    static std::shared_ptr<Camera> _camera;

    static std::shared_ptr<InputSystem> _inputManager;
    static std::shared_ptr<TimeManager> _timeManager;
};
