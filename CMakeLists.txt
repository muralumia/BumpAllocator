cmake_minimum_required(VERSION 3.16)

project(BumpAllocator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(bump_allocator
    src/BumpAllocator.cpp
)

target_include_directories(bump_allocator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(bump_allocator_demo
    src/main.cpp
)
target_link_libraries(bump_allocator_demo PRIVATE bump_allocator)

include(CTest)

if(BUILD_TESTING)
    add_executable(bump_allocator_tests
        tests/BumpAllocatorTests.cpp
    )
    target_link_libraries(bump_allocator_tests PRIVATE bump_allocator)

    add_test(NAME bump_allocator_tests COMMAND bump_allocator_tests)
endif()

