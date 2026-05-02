#include "zkpch.hpp"
#include "ImageLoader.hpp"

#include "ZkEngine/Core/Log.hpp"
#include "stb_image.h"

namespace zk {

ImageData ImageLoader::load(const std::filesystem::path& path, bool flip) {
    if (!std::filesystem::exists(path)) {
        ZK_CORE_ERROR("File does not exist: {}", path.string());
        return {};
    }

    ImageData img;

    stbi_set_flip_vertically_on_load(flip);

    img.data =
        stbi_load(path.string().c_str(), &img.width, &img.height, &img.channels, STBI_rgb_alpha);

    img.channels = 4;

    if (!img.data) {
        ZK_CORE_ERROR("Failed to load image: {}", path.string());
    }

    return img;
}

void ImageLoader::free(ImageData& img) {
    stbi_image_free(img.data);
    img.data = nullptr;
}

} // namespace zk
