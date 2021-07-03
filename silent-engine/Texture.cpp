#include "Texture.h"

Texture::Texture(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool pool, const std::string& path)
{
    _image = Image(device, allocator, pool, path);
}

Texture::~Texture()
{
    _image.destroy();
}
