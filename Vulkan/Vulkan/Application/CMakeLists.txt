cmake_minimum_required(VERSION 3.4.1)

project(Vulkanapp)


#[[if (NOT DEFINED ENV{GLFW_HOME})
    message(FATAL_ERROR "found no env named GLFW_HOME")
endif()
if (NOT DEFINED ENV{GLM_HOME})
    message(FATAL_ERROR "found no env named GLM_HOME")
endif()
set(GLFW_HOME $ENV{GLFW_HOME})]]

set(GLFW_DIR "/opt/homebrew/Cellar/glfw/3.3.8")
include_directories( ${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib)
set(GLFW_LIBRARIES ${GLFW_DIR}/lib/libglfw.3.3.dylib )

add_library(ksvulkanapp STATIC  Application.cpp KSVulkanApplication.cpp)
target_link_libraries(ksvulkanapp ${GLFW_LIBRARIES} )

