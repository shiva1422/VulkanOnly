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
#include <unistd.h>

#define LOGTAG "KSVulkanApplication"

 

KSVulkanApplication::KSVulkanApplication()
{
    vulkan.setWindowInterface(this);
}

KSVulkanApplication::~KSVulkanApplication()
{
    Logger::info(LOGTAG,"destroying");
    onDestroy();
}

void KSVulkanApplication::run()
{
    onWindowInit();
    vulkanInit();
    assert(createVKSurface());

    while (!glfwWindowShouldClose(window))
    {
        usleep(32000);
     //   Logger::info(LOGTAG, "Main Loop");
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

bool KSVulkanApplication::createVKSurface()
{
    //TOOD move to KSVulkan and create surface like any other vk creation with platform specifs
    if(glfwCreateWindowSurface(vulkan.vkInstance,window, nullptr,&vulkan.vkSurface) != VK_SUCCESS)
    {
        Logger::error(LOGTAG,"couldn't create VKSurface");
        return false;
    }
    return true;
}

void KSVulkanApplication::getFrameBufferSize(int &width, int &height) {
    //get pixels size from screen coordinates dp
    glfwGetFramebufferSize(window,&width, &height);
}
