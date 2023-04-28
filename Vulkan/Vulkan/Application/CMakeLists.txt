cmake_minimum_required(VERSION 3.4.1)

project(Vulkanapp)


if (NOT DEFINED ENV{GLFW_HOME})
    message(STATUS "found no env named GLFW_HOME")
   #message(FATAL_ERROR "found no env named GLM_HOME")

endif()
#set(GLFW_HOME $ENV{GLFW_HOME})

#TODO
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(GLFW_DIR "/opt/homebrew/Cellar/glfw/3.3.8")
include_directories( ${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib)
set(GLFW_LIBRARIES ${GLFW_DIR}/lib/libglfw.3.3.dylib )

add_library(ksvulkanapp STATIC  Application.cpp KSVulkanApplication.cpp 15_hello_triangle.cpp Test/TestVKApp.cpp Test/TestVKApp.h)
target_link_libraries(ksvulkanapp ${GLFW_LIBRARIES} )

