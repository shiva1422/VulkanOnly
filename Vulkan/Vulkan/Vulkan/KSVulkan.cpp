//
//  KSVulkan.cpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#include "KSVulkan.hpp"
#include <vector>
#include "../Logger.h"

#define LOGTAG "KSVulkan"

/*
 * steps :
 * Init KVulkan - Load KVulkan and retrieve vulkan api function pointers.
 * create Instance
 *  fill VKApplicationInfo struct(option) with info about the app(may be for driver to
    optimize our specific application based on info)(OPTIONAL).
 *  tell KVulkan driver which global extension and validation layers we want to use(global means that they apply to entire program and not a specific device).(NOT OPTIONAL).
 *create vulkan instance.
 *  create a surface - to create surface , surface extension is needed
 *  Find gpu to use check vulkan capabilites info on gpu,
 *  find GFX queueFamily.Graphics compute etc.
 *  create a logical device from the picked gpu;
    can create multiple lovical devices from same physical device;
 *
 */


//DebugMessennger functions load
//Debug Messenger setup
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,VkDebugUtilsMessageTypeFlagsEXT messageType,const
                                                VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,void* pUserData)
{
    Logger::debug("validationLyaer","%s",pCallbackData->pMessage);
// if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    // Message is important enough to show}
  //  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

bool KSVulkan::configure()
{
    return createInstance();
}

bool KSVulkan::createInstance()
{
    //DebugOnly
    if(enableValidationLayers && !checkValidationSupport())
        return false;
    
    //if validation enable include extension VK_EXT_DEBUG_UTILS_EXTENSION_NAME  for Debug logcallbacks only debug here we are addding all extension now so no need explicit
    
    VkApplicationInfo  appInfo  =  {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                    .pNext=nullptr,
                                    .pApplicationName="KSVulkanApplication",
                                    .applicationVersion=VK_MAKE_VERSION(1,0,0),
                                    .pEngineName="KSVulkanEngine",
                                    .engineVersion=VK_MAKE_VERSION(1,0,0),
                                    .apiVersion=VK_MAKE_VERSION(1,1,0),
                                    };//TODO check update later
    
    //check and print extensions //TODO Debug Only
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr , &extensionCount , nullptr);//check res
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr , &extensionCount,extensions.data());//checkres;
    
    Logger::info(LOGTAG,"available Extensions ...");
    for(const auto& extension :extensions)
    {
        Logger::info(LOGTAG,"extension - %s",extension.extensionName);
    }
    
    
    std::vector<const char*> instanceExt;
    
   //TODO only requried and debug?
    //instanceExt.push_back("VK_KHR_surface");
    //instanceExt.push_back("VK_KHR_android_surface");
    for(const auto& extension : extensions)
    {
        instanceExt.push_back(extension.extensionName);
    }
    
    //Mac incompatiblility err
    instanceExt.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    
    /*GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    // glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/

    //create vulkan Instance
       VkInstanceCreateInfo instanceCreateInfo{.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                               .pNext=nullptr,
                                               .pApplicationInfo =&appInfo,
                                                // global validation layers to enable
                                               .enabledLayerCount=0,
                                               .ppEnabledLayerNames=nullptr,
                                                //TODO below 2 platform specific
                                               .enabledExtensionCount=static_cast<uint32_t>(extensionCount),
                                               .ppEnabledExtensionNames = instanceExt.data(),
                                               };
    
        //Mac error
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    //validation layers
    if (enableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }

       VkResult res ;
    
       if((res = vkCreateInstance(&instanceCreateInfo,nullptr,&vkInstance) )!= VK_SUCCESS)
       {
           Logger::error(LOGTAG,"instance Creation failed - err -  %d",res);
           
           //For mac VK_ERROR_EXTENSION_NOT_PRESENT err
           return false;
       }

    return true;
}

bool isDeviceSuitable(VkPhysicalDevice device)
{
    //To evaluate the suitability of a device we can start by querying for some details. Basic device properties like the name, type //and supported Vulkan version can be queried using vkGetPhysicalDeviceProperties.
    
   // VkPhysicalDeviceProperties deviceProperties;
   // vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    
    //The support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR) can be queried using vkGetPhysicalDeviceFeatures:
    
    return true;//can check for various features and select based on the requirement;//just selecting first gpu now
    
}

bool KSVulkan::selectDevice()
{
    //Multiple gpu devices can be selected and used and device will be destoued with instance so no need to cleanup
    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        return false;
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            vkGpu = device;
            break;
        }
    }

    return vkGpu != VK_NULL_HANDLE;
}
bool KSVulkan::createSurface()
{
    return true;
}


void KSVulkan::release()
{
    if (enableValidationLayers)
    {
      DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
    }

    vkDestroyInstance(vkInstance, nullptr);
}


bool KSVulkan::checkValidationSupport()
{
    uint32_t layerCnt;
    vkEnumerateInstanceLayerProperties(&layerCnt, nullptr);
    std::vector<VkLayerProperties> layerProps(layerCnt);
    vkEnumerateInstanceLayerProperties(&layerCnt,layerProps.data());
    
    for(auto *layer : validationLayers)
    {
        bool layerFound = false;
        
        for (const auto& layerProp : layerProps)
        {
            if (strcmp(layer, layerProp.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound)
        {
                //Doesnt mean all layers not found TODO
            return false;
        }
        
    }
    
    return true;
}


//Load debugger extension function from vulkan lib as this is extension function
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void KSVulkan::setupDebugMessenger()
{
    //as setting up this requires vkInstance can't debug instance creation and destruction but there
    //is a way
    if (!enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
    
    if (CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        Logger::error(LOGTAG,"Error setup debug messenger");
    }

}

