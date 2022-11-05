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

class KSVulkan{

private:
    
     bool createInstance();
    
     bool createSurface();
    
     bool getGpuAndCheckCapabilities();
    
     bool findGFXFamilyQueueAndCreateLogicalDevice();
    
private:
    
     bool bInit = false;
     VkInstance vkInstance;
     VkPhysicalDevice vkGpu;
     VkDevice vkDevice = VK_NULL_HANDLE;
     VkSurfaceKHR vkSurface;
    
};

#endif /* KSVulkan_hpp */
