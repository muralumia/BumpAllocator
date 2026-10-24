#include "BumpAllocator.h"

#include <cstdint>
#include <iostream>
#include <new>

struct Point {
    int x;
    int y;
};

int main() {
    BumpAllocator allocator;

    void* numberMemory = allocator.allocate(sizeof(int), alignof(int));
    void* pointMemory = allocator.allocate(sizeof(Point), alignof(Point));

    if (numberMemory == nullptr || pointMemory == nullptr) {
        std::cerr << "The allocator does not have enough memory.\n";
        return 1;
    }

    // Placement new constructs objects inside memory supplied by the allocator.
    int* number = new (numberMemory) int{42};
    Point* point = new (pointMemory) Point{10, 20};

    std::cout << "BumpAllocator v0.1\n";
    std::cout << "number = " << *number << '\n';
    std::cout << "point = (" << point->x << ", " << point->y << ")\n";
    std::cout << "used bytes (including alignment padding): "
              << allocator.used() << " / " << BumpAllocator::capacity << '\n';

    // int and Point have trivial destructors, so no explicit destruction is
    // needed in this example. reset() reuses the entire buffer.
    allocator.reset();
    std::cout << "used after reset: " << allocator.used() << '\n';

    return 0;
}

