#pragma once

#include <array>
#include <cstddef>
#include <new>
#include <utility>

// A small linear (bump) allocator for learning purposes.
//
// Memory is handed out from the beginning of a fixed-size buffer. Individual
// blocks cannot be freed; reset() makes the whole buffer reusable.
class BumpAllocator {
public:
    static constexpr std::size_t capacity = 1024;

    BumpAllocator() = default;
    BumpAllocator(const BumpAllocator&) = delete;
    BumpAllocator& operator=(const BumpAllocator&) = delete;

    // Returns an aligned block, or nullptr when there is not enough space.
    // Alignment must be a non-zero power of two.
    void* allocate(std::size_t size,
                   std::size_t alignment = alignof(std::max_align_t));

    // Allocates memory and constructs one object in it.
    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* memory = allocate(sizeof(T), alignof(T));
        if (memory == nullptr) {
            return nullptr;
        }

        return new (memory) T(std::forward<Args>(args)...);
    }

    // Calls an object's destructor. Its memory is still reclaimed only by
    // reset(), because this allocator has no individual deallocation.
    template <typename T>
    void destroy(T* object) noexcept {
        if (object != nullptr) {
            object->~T();
        }
    }

    // Makes all allocations reusable. Destructors are not called automatically.
    void reset() noexcept;

    [[nodiscard]] bool owns(const void* pointer) const noexcept;
    [[nodiscard]] std::size_t used() const noexcept;
    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] std::size_t peakUsed() const noexcept;
    [[nodiscard]] std::size_t successfulAllocations() const noexcept;
    [[nodiscard]] std::size_t failedAllocations() const noexcept;

private:
    alignas(std::max_align_t) std::array<std::byte, capacity> buffer_{};
    std::size_t offset_ = 0;
    std::size_t peakUsed_ = 0;
    std::size_t successfulAllocations_ = 0;
    std::size_t failedAllocations_ = 0;
};

