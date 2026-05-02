#pragma once

#include <cstdint>
#include <filesystem>

namespace zk {

// CPU image container
struct ImageData {
    void* data = nullptr;
    int width = 0;
    int height = 0;
    int channels = 0;
};

class ImageLoader {
  public:
    static ImageData load(const std::filesystem::path& path, bool flip = true);
    static void free(ImageData& data);
};

} // namespace zk
