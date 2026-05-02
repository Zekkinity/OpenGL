#pragma once

#include <cstdint>

class IndexBuffer {
  public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t getCount() const = 0;
};
