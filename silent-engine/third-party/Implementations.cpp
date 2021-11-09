#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT // _crt_secure_no_warnings deprecation in stb_image_write fix
#include "tinygltf\tiny_gltf.h"

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"