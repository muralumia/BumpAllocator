#pragma once

#include <array>
#include <cstddef>

// A small linear (bump) allocator for learning purposes.
//
// Memory is handed out from the beginning of a fixed-size buffer. Individual
// allocations cannot be freed; reset() makes the whole buffer reusable.
class BumpAllocator {
public:
    static constexpr std::size_t capacity = 1024;

    // Returns an aligned block of memory, or nullptr when there is not enough
    // space. Alignment must be a non-zero power of two.
    void* allocate(std::size_t size,
                   std::size_t alignment = alignof(std::max_align_t));

    // Makes every allocation reusable at once. Objects placed in the buffer
    // are not destroyed automatically.
    void reset() noexcept;

    [[nodiscard]] std::size_t used() const noexcept;
    [[nodiscard]] std::size_t remaining() const noexcept;

private:
    alignas(std::max_align_t) std::array<std::byte, capacity> buffer_{};
    std::size_t offset_ = 0;
};

