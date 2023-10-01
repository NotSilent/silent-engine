#pragma once

#include "VkBootstrap.h"
#include <memory>
#include <vector>

class PipelineLayout;

class PipelineLayoutManager {
public:
    PipelineLayoutManager(VkDevice _device);

    std::shared_ptr<PipelineLayout> getLayout();

    // TODO: Remove all destroys
    void destroy();

private:
    VkDevice device;

    std::vector<std::shared_ptr<PipelineLayout>> _layouts;
};
