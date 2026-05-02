#pragma once

#include <cstdint>
#include <string>

namespace zk {

using AssetHandle = uint64_t;

class Asset {
  public:
    virtual ~Asset() = default;

    AssetHandle handle;
    std::string debugName;
};

} // namespace zk
