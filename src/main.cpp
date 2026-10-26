#include "BumpAllocator.h"

#include <iostream>

struct Point {
    int x;
    int y;

    Point(int xValue, int yValue) : x(xValue), y(yValue) {}
};

int main() {
    BumpAllocator allocator;

    int* number = allocator.create<int>(42);
    Point* point = allocator.create<Point>(10, 20);

    if (number == nullptr || point == nullptr) {
        std::cerr << "The allocator does not have enough memory.\n";
        return 1;
    }

    std::cout << "BumpAllocator v0.2\n";
    std::cout << "number = " << *number << '\n';
    std::cout << "point = (" << point->x << ", " << point->y << ")\n";
    std::cout << "point belongs to allocator: "
              << (allocator.owns(point) ? "yes" : "no") << '\n';
    std::cout << "successful allocations: "
              << allocator.successfulAllocations() << '\n';
    std::cout << "used bytes: " << allocator.used() << " / "
              << BumpAllocator::capacity
              << '\n';

    allocator.destroy(point);
    allocator.destroy(number);
    allocator.reset();

    std::cout << "used after reset: " << allocator.used() << '\n';
    std::cout << "peak usage: " << allocator.peakUsed() << " bytes\n";

    return 0;
}

