#pragma once

#include "ZkEngine/Core/Log.hpp"

#include <expected>
#include <memory>

namespace zk {


#ifdef __linux__
#include <signal.h>
#define ZK_DEBUGBREAK() raise(SIGTRAP)
#else
#define ZK_DEBUGBREAK()
#endif

#define ZK_DEBUG_BUILD

#ifdef ZK_DEBUG_BUILD

// Make macros for assertions
#define ZK_CORE_ASSERT(x, msg)                                                                     \
    if (!(x)) {                                                                                    \
        ZK_CORE_ERROR(std::string("Assertion Failed: ") + msg);                                      \
        ZK_DEBUGBREAK();                                                                           \
    }
#else
#define ZK_CORE_ASSERT(x, msg)
#endif

// Error handling
template <typename T>
using Result = std::expected<T, std::string>;

inline std::unexpected<std::string> ErrImpl(std::string msg, const char* file, int line) {
    std::string filename = std::filesystem::path(file).filename().string();

    std::string fullMessage = "[" + filename + ":" + std::to_string(line) + "] " + msg;
    return std::unexpected<std::string>(std::move(fullMessage));
}

#define Err(msg) ErrImpl((msg), __FILE__, __LINE__)

// template <typename T>
// Result<T> Err(std::string msg) {
//     return std::unexpected(std::move(msg));
// }
//
// inline std::unexpected<std::string> Err(std::string msg) {
//     return std::unexpected<std::string>(std::move(msg));
// }
//

// Smart pointers
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace zk
