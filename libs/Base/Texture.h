#pragma once

#include "Image.h"
#include <memory>
#include "Sampler.h"

class Texture {
public:
    Texture() = default;

    Texture(const std::shared_ptr<Sampler> &sampler, const std::shared_ptr<Image> &image);

    void destroy(VkDevice device, VmaAllocator allocator);

    [[nodiscard]] VkImageView getImageView() const;

    [[nodiscard]] VkSampler getSampler() const;

private:
    std::shared_ptr<Sampler> _sampler;
    std::shared_ptr<Image> _image;
};
