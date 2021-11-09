#pragma once
#include <string>
#include <chrono>

class ProfillerTimer {
public:
    ProfillerTimer();

    float end() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
};
