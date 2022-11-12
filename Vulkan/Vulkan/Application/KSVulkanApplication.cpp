//
//  KSVulkanApplication.cpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#include "KSVulkanApplication.hpp"
#include "cassert"
#include "../Logger.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define LOGTAG "KSVulkanApplication"

 

KSVulkanApplication::KSVulkanApplication()
{
    
}

KSVulkanApplication::~KSVulkanApplication()
{
    Logger::info(LOGTAG,"destroying");
    onDestroy();
}

void KSVulkanApplication::run()
{
    onWindowInit();
    
    assert(vulkanInit());
    
    while (!glfwWindowShouldClose(window))
    {
       // Logger::info(LOGTAG, "Loop");
        glfwPollEvents();
    }
   // KSApplication::run();
}

bool KSVulkanApplication::vulkanInit()
{
    return vulkan.configure();
}

void KSVulkanApplication::onWindowInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, LOGTAG , nullptr, nullptr);
   // KSApplication::onWindowInit();
}


void KSVulkanApplication::onDestroy()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
