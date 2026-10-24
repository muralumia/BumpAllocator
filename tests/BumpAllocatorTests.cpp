#include "BumpAllocator.h"

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

void testBasicAllocation() {
    BumpAllocator allocator;

    void* memory = allocator.allocate(16, 1);

    check(memory != nullptr, "a small allocation should succeed");
    check(allocator.used() == 16, "a byte-aligned allocation should use 16 bytes");
    check(allocator.remaining() == BumpAllocator::capacity - 16,
          "remaining space should decrease after allocation");
}

void testAlignment() {
    BumpAllocator allocator;
    allocator.allocate(1, 1);

    void* memory = allocator.allocate(8, 32);
    const auto address = reinterpret_cast<std::uintptr_t>(memory);

    check(memory != nullptr, "an aligned allocation should succeed");
    check(address % 32 == 0, "the returned address should be aligned");
}

void testOutOfMemory() {
    BumpAllocator allocator;

    void* allMemory = allocator.allocate(BumpAllocator::capacity, 1);
    void* tooMuch = allocator.allocate(1, 1);

    check(allMemory != nullptr, "the full buffer should be allocatable");
    check(tooMuch == nullptr, "allocation past the buffer should fail");
    check(allocator.used() == BumpAllocator::capacity,
          "a failed allocation should not change used space");
}

void testReset() {
    BumpAllocator allocator;

    void* first = allocator.allocate(24, 8);
    allocator.reset();
    void* second = allocator.allocate(24, 8);

    check(allocator.used() == 24, "reset should make the whole buffer reusable");
    check(first == second, "allocation should start at the beginning after reset");
}

void testInvalidAlignment() {
    BumpAllocator allocator;
    bool exceptionWasThrown = false;

    try {
        allocator.allocate(8, 3);
    } catch (const std::invalid_argument&) {
        exceptionWasThrown = true;
    }

    check(exceptionWasThrown, "non-power-of-two alignment should be rejected");
    check(allocator.used() == 0, "an invalid request should not consume memory");
}

void testZeroSize() {
    BumpAllocator allocator;

    void* memory = allocator.allocate(0, 8);

    check(memory == nullptr, "a zero-size allocation should return nullptr");
    check(allocator.used() == 0, "a zero-size allocation should not consume memory");
}

} // namespace

int main() {
    testBasicAllocation();
    testAlignment();
    testOutOfMemory();
    testReset();
    testInvalidAlignment();
    testZeroSize();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout << "All BumpAllocator v0.1 tests passed.\n";
    return 0;
}

