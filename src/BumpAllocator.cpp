#include "BumpAllocator.h"

#include <cstdint>
#include <stdexcept>

namespace {

bool isPowerOfTwo(std::size_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

} // namespace

void* BumpAllocator::allocate(std::size_t size, std::size_t alignment) {
    if (!isPowerOfTwo(alignment)) {
        throw std::invalid_argument("alignment must be a non-zero power of two");
    }

    if (size == 0) {
        return nullptr;
    }

    const auto currentAddress =
        reinterpret_cast<std::uintptr_t>(buffer_.data() + offset_);
    const std::size_t misalignment = currentAddress % alignment;
    const std::size_t padding =
        misalignment == 0 ? 0 : alignment - misalignment;

    // Written this way to avoid overflowing when size is very large.
    const std::size_t available = capacity - offset_;
    if (padding > available || size > available - padding) {
        return nullptr;
    }

    std::byte* result = buffer_.data() + offset_ + padding;
    offset_ += padding + size;
    return result;
}

void BumpAllocator::reset() noexcept {
    offset_ = 0;
}

std::size_t BumpAllocator::used() const noexcept {
    return offset_;
}

std::size_t BumpAllocator::remaining() const noexcept {
    return capacity - offset_;
}

