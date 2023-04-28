//
//  main.cpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#include <iostream>
#include "Logger.h"
#include "Application/KSVulkanApplication.hpp"
#include "Application/Test/TestVKApp.h"


int main()
{
    KSVulkanApplication vulkanApp;
    //TestVKApp vulkanApp;//Create Commaand pool

    vulkanApp.run();

    /*if (glfwVulkanSupported())
    {
       
        Logger::printf("Vulkan Supported");
        // Vulkan is available, at least for compute
    }
    else
    {
        Logger::printf("Vulkan Not Supported");
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();*/

    return 0;
}
