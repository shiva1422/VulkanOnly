cmake_minimum_required(VERSION 3.4.1)

project(vulkanlib)

set(VULKAN_DIR "/Users/shivaaz/projects/SDKS/vulkan/macOs")
set(GLFW_DIR "/opt/homebrew/Cellar/glfw/3.3.8")


include_directories(${VULKAN_DIR}/include )

add_library(ksvulkan STATIC KSVulkan.cpp)
set(VULKAN_LIBS ${VULKAN_DIR}/lib/libvulkan.1.dylib ${VULKAN_DIR}/lib/libvulkan.1.3.231.dylib)
target_link_libraries(ksvulkan ${VULKAN_LIBS})
