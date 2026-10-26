#include "BumpAllocator.h"

#include <cstdint>
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

struct TrackedObject {
    static int livingObjects;
    int value;

    explicit TrackedObject(int newValue) : value(newValue) {
        ++livingObjects;
    }

    ~TrackedObject() {
        --livingObjects;
    }
};

int TrackedObject::livingObjects = 0;

void testBasicAllocation() {
    BumpAllocator allocator;
    void* memory = allocator.allocate(16, 1);

    check(memory != nullptr, "a small allocation should succeed");
    check(allocator.used() == 16, "the allocation should consume 16 bytes");
    check(allocator.remaining() == BumpAllocator::capacity - 16,
          "remaining space should decrease");
}

void testAlignment() {
    BumpAllocator allocator;
    allocator.allocate(1, 1);
    void* memory = allocator.allocate(8, 32);

    check(memory != nullptr, "an aligned allocation should succeed");
    check(reinterpret_cast<std::uintptr_t>(memory) % 32 == 0,
          "the returned address should be aligned");
}

void testTypedObject() {
    BumpAllocator allocator;
    TrackedObject* object = allocator.create<TrackedObject>(73);

    check(object != nullptr, "create should return an object");
    check(object->value == 73, "create should pass constructor arguments");
    check(TrackedObject::livingObjects == 1,
          "the object constructor should be called");

    allocator.destroy(object);
    check(TrackedObject::livingObjects == 0,
          "destroy should call the object destructor");
}

void testOwnership() {
    BumpAllocator allocator;
    int outsideValue = 0;
    void* inside = allocator.allocate(1, 1);

    check(allocator.owns(inside), "an allocated address should belong to the allocator");
    check(!allocator.owns(&outsideValue), "an external address should not belong");
    check(!allocator.owns(nullptr), "nullptr should not belong to the allocator");
}

void testStatisticsAndReset() {
    BumpAllocator allocator;
    void* first = allocator.allocate(24, 1);
    allocator.reset();
    void* second = allocator.allocate(8, 1);

    check(first == second, "reset should reuse memory from the beginning");
    check(allocator.used() == 8, "reset should clear current usage");
    check(allocator.peakUsed() == 24, "reset should preserve peak usage");
    check(allocator.successfulAllocations() == 2,
          "successful allocations should be counted");
}

void testOutOfMemory() {
    BumpAllocator allocator;
    allocator.allocate(BumpAllocator::capacity, 1);
    void* tooMuch = allocator.allocate(1, 1);

    check(tooMuch == nullptr, "allocation past the buffer should fail");
    check(allocator.failedAllocations() == 1,
          "failed allocations should be counted");
    check(allocator.used() == BumpAllocator::capacity,
          "a failed allocation should not consume memory");
}

void testInvalidRequests() {
    BumpAllocator allocator;
    bool exceptionWasThrown = false;

    try {
        allocator.allocate(8, 3);
    } catch (const std::invalid_argument&) {
        exceptionWasThrown = true;
    }

    check(exceptionWasThrown, "invalid alignment should be rejected");
    check(allocator.allocate(0, 8) == nullptr,
          "a zero-size allocation should return nullptr");
    check(allocator.used() == 0, "invalid requests should not consume memory");
}

} // namespace

int main() {
    testBasicAllocation();
    testAlignment();
    testTypedObject();
    testOwnership();
    testStatisticsAndReset();
    testOutOfMemory();
    testInvalidRequests();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout << "All BumpAllocator v0.2 tests passed.\n";
    return 0;
}

