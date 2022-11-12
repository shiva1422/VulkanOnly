cmake_minimum_required(VERSION 3.4.1)

project(vulkanmain)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(VULKAN_DIR /Users/shivaaz/projects/SDKS/vulkan/macOs)
include_directories(${VULKAN_DIR}/include)

add_subdirectory(./Vulkan ./ksvulkan)
add_subdirectory(./Application ./ksvulkanapp)

add_executable(vulkanmain main.cpp)
target_link_libraries(vulkanmain ksvulkan ksvulkanapp)
