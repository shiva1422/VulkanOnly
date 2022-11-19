//
//  KSVulkan.hpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#ifndef KSVulkan_hpp
#define KSVulkan_hpp

#include <stdio.h>
#include "vulkan/vulkan.h"

/*
 TODO : Check Vulkan Supported
 I do a few things to determine whether the Vulkan backend of my graphics lib is supported:

 Try to load the Vulkan shared library -- if I can't even do that, then it's not supported.

 Create an instance and enumerate the physical devices. If there's not at least one physical device, then it's not supported. (Your #1)

 Enumerate the available instance extensions (vkEnumerateInstanceExtensionProperties), and check that the minimum extensions are supported (e.g. VK_KHR_surface and the platform's surface extension.) This is your #2. IMO it's probably overkill to do all that work (which is platform-specific since it involves creating a window) on top of checking that the extensions are supported.
 */



//TO store all indices of queuefamilies we nee
class QueueFamilyIndices{

private:

    friend class KSVulkan;
    //TODO 0 indicates a valid queue value so its not good to decide if its valid number try set to max of use std::optional  c++17
    uint32_t graphicsFamily;
};

class KSVulkan{
    
public:
    
    bool configure();//TODO later use config properties common to graphics folder.
    
    void release();

private:

    //In Order

    bool createInstance();

    //TODO Check previous android code
    bool findGFXFamilyQueues(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool selectDevice();

    bool createLogicalDevice();

    bool createSurface();





     bool checkValidationSupport();
    
     void setupDebugMessenger();
private:
    
     bool bInit = false;
    
   /* A Vulkan Instance is an object that gathers the state of an application. It encloses information such as an application name, name and version of an engine used to create an application, or enabled instance-level extensions and layers.
    */

     VkInstance vkInstance;
     VkPhysicalDevice vkGpu = VK_NULL_HANDLE;
     VkDevice vkDevice = VK_NULL_HANDLE;
     VkQueue graphicsQueue;
     VkSurfaceKHR vkSurface;
     QueueFamilyIndices indices;

     //TODO createLogicalDevice
     VkPhysicalDeviceFeatures deviceFeatures{};


    //debug
    VkDebugUtilsMessengerEXT debugMessenger;

    
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    
    
};


#endif /* KSVulkan_hpp */


/*
 * Vulkan functions can be divided into three levels, which are global, instance, and device. Device-level functions are used to perform typical operations such as drawing, shader- modules creation, image creation, or data copying. Instance-level functions allow us to create logical devices. To do all this, and to load device and instance-level functions, we need to create an Instance. This operation is performed with global-level functions, which we need to load first.
 In Vulkan, there are only three global-level functions: vkEnumerateInstanceExtensionProperties(), vkEnumerateInstanceLayerProperties(), and vkCreateInstance(). They are used during Instance creation to check, what instance-level extensions and layers are available and to create the Instance itself.
 
 
    *   Instance-level functions are used mainly for operations on physical devices manipulating with physical devices, checking their properties, abilities and, creating logical devices. There are multiple instance-level functions, with vkEnumeratePhysicalDevices(), vkGetPhysicalDeviceProperties(), vkGetPhysicalDeviceFeatures(), vkGetPhysicalDeviceQueueFamilyProperties(), vkCreateDevice(), vkGetDeviceProcAddr(), vkDestroyInstance() or vkEnumerateDeviceExtensionProperties etc
        
    All device-level functions have their first parameter of type VkDevice, VkQueue, or VkCommandBuffer.So, if a function doesn't have such a parameter and is not from the global level, it is from an instance level
 
 *
 */
