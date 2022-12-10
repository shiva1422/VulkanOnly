//
//  KSVulkan.cpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#include "KSVulkan.hpp"
#include <vector>
#include "../Logger.h"
#include "set"
#include "string"
#include <algorithm>
#include <limits>
#include "../Shaders/ShaderManager.h"
#define LOGTAG "KSVulkan"


//platforms
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

const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
bool KSVulkan::configure()
{
    bool res = createInstance() ;
    setupDebugMessenger();
    // The window surface needs to be created right after the instance creation, because it can actually influence the physical device selection
    assert(vkSurfaceCreator != nullptr);
    vkSurfaceCreator->createVKSurface();//TODO refact
    res = res && selectPhysicalDevice() && createLogicalDevice() && createSwapChain() && createImageViews() && createRenderPass() && createGraphicsPipeline();
    assert(res);
    return res;
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
    //instanceExt.push_back();
    
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
                                               .enabledExtensionCount=static_cast<uint32_t>(instanceExt.size()),
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

bool KSVulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
{

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool KSVulkan::isDeviceSuitable(VkPhysicalDevice device)
{
    //To evaluate the suitability of a device we can start by querying for some details. Basic device properties like the name, type //and supported Vulkan version can be queried using vkGetPhysicalDeviceProperties.
    
   // VkPhysicalDeviceProperties deviceProperties;
   // vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
//           deviceFeatures.geometryShader;/
//    Instead of just checking if a device is suitable or not and going with the first one,
//    you could also give each device a score and pick the highest one. That way you could favor a dedicated
//    graphics card by giving it ahigher score, but fall back to an integrated GPU if that's the only available one
//check pickPhysicalDevice()? from tuts

    bool res = false;
    res =  findGFXFamilyQueues(device) && checkDeviceExtensionSupport(device);

    //only if swapChainExtension supported
    if(res)
    {
        SwapChainInfo info = getSwapChainInfo(device);
        res = !info.formats.empty() && !info.presentModes.empty();
    }


    return res;
    ;
    //The support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR) can be queried using vkGetPhysicalDeviceFeatures:
    
    return true;//can check for various features and select based on the requirement;//just selecting first gpu now
    
}

bool KSVulkan::selectPhysicalDevice()
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

bool KSVulkan::findGFXFamilyQueues(VkPhysicalDevice device)
{
    /*
     *  almost every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue.
     *  There are different types of queues that originate
     *  from different queue families and each family of queues allows only a subset of commands.
     *  For example, there could be a queue family that only allows processing
     *  of compute commands or one that only allows memory transfer related commands.
     */

    //Right only going to look for a queue that supports graphics commands later compute?

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    /*
     * The VkQueueFamilyProperties struct contains some details about the queue family, including the type of operations that are supported and the number of queues that
     * can be created based on that family. We need to find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
     */



    bool found = false;
    int i = 0;
    bool bGraphicsFam = false;
    bool bPresentFam = false;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
           // return true;
           bGraphicsFam = true;

        }

        // queue family that has the capability of presenting to our window surface.//Surface should be created here
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkSurface, &presentSupport);
        if(presentSupport)
        {
            bPresentFam = true;
            indices.presentFamily = i;
        }

        i++;

        /*Note that it's very likely that these end up being the same queue family after all, but throughout the program we will treat them as if they were separate queues for a uniform approach
        Nevertheless, you could add logic to explicitly prefer a physical device that supports drawing and presentation in the same queue for improved performance.
         */
        if(bGraphicsFam && bPresentFam)
            break;
    }

    found = bGraphicsFam && bPresentFam;//all required families are found
    return found;
}



bool KSVulkan::createLogicalDevice()
{
   //can select multiple logical devices from the same physical device for varying requirements.

   /*
    * The creation of a logical device involves specifying a bunch of details in structs again, of which the first one will be VkDeviceQueueCreateInfo.
    * This structure describes the number of queues we want for a single queue family. Right now we're only interested in a queue with graphics capabilities.
    */

    assert(findGFXFamilyQueues(vkGpu));//multiple times
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    //below values should be valid and indited properly before this in findQueuefams
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};//TODO validity

    //    Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution
    //    //If the queue families are the same, then we only need to pass its index once. Finally, add a call to retrieve the queue handle:
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        Logger::debug("KSVulkan::createLogicalDevice","%u quefamilyIndex",queueFamily);
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    //Logical Device Creation
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;//TODO before creating

    //device extensions
    deviceCreateInfo.enabledExtensionCount = 1;
    const char* deviceExtension = "VK_KHR_portability_subset";//mac err

    //TODO check may not be required
    if (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

   // Using a swapchain requires enabling the VK_KHR_swapchain extension
    std::vector<const char*>  deviceExt;//fromCreateInstace
    deviceExt.push_back("VK_KHR_portability_subset");
    for(auto ext : deviceExtensions)
    {
        deviceExt.push_back(ext);
    }
    //deviceExt.push_back(deviceExtensions.)
    deviceCreateInfo.enabledExtensionCount = deviceExt.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExt.data();

    if (vkCreateDevice(vkGpu, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
        Logger::error(LOGTAG,"create logical device failed");//reason print
        return false;
    }

    /*
     * The queues are automatically created along with the logical device, but we don't have a handle to interface
     * with them yet.Device queues are implicitly cleaned up when the device is destroyed
     */

    //We can use the vkGetDeviceQueue function to retrieve queue handles for each queue family
    //TODO indices.graphicsFamily && presentFam should be inited correctly ,default intialize might have value that might be valid but not correct
    vkGetDeviceQueue(vkDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(vkDevice,indices.presentFamily,0,&presentQueue);

    //Now Graphics card can be used to do amazing stuff but not quite enough :)

    return true;
}




bool KSVulkan::createSurface(void *window)
{
  // if(glfwCreate)
    return false;
}


void KSVulkan::release()
{

    vkDestroyPipeline(vkDevice, graphicsPipeline, nullptr);

    vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);

    vkDestroyRenderPass(vkDevice, renderPass, nullptr);

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(vkDevice, imageView, nullptr);
    }

    if (enableValidationLayers)
    {
      DestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
    }

    vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
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

bool KSVulkan::createSwapChain()
{
    SwapChainInfo swapChainSupport = getSwapChainInfo(vkGpu);

    VkSurfaceFormatKHR surfaceFormat = surfaceChooseSwapFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;//atlest more than one minImage count
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = vkSurface;

    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

   // specify how to handle swap chain images that will be used across multiple queue families
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily,indices.presentFamily};
   if(indices.graphicsFamily != indices.presentFamily)//Should be set properly findingqueuefamilies
   {
      // An image is owned by one queue family at a time and ownership must be
      // explicitly transferred before using it in another queue family. This option offers the best performance.
      //concurrent mode requires you to specify at least two distinct queue families.
       swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
       swapChainCreateInfo.queueFamilyIndexCount = 2;
       swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
   }
   else
   {
       //Images can be used across multiple queue families without explicit ownership transfers.
       swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
       swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
       swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
   }

   //any tranform like 90 degrees
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;

   //The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
   //ignoring here
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;//clipping enabled

    //when window resized or other swapchain needs recreated specifying old one for now null
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &vkSwapChain) != VK_SUCCESS) {
        Logger::error("CreateSwapChain","failed");
        return false;
    }


    //retreive swapchain images

    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, swapChainImages.data());

    Logger::info("SwapChain Create Success","imageCount %u",imageCount);
    //validate if the created and selected or same later
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;


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
    
    if (CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        Logger::error(LOGTAG,"Error setup debug messenger");
    }

}

SwapChainInfo KSVulkan::getSwapChainInfo(VkPhysicalDevice device)
{
    SwapChainInfo swapChainInfo;

    //check below func variants
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,vkSurface,&swapChainInfo.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        swapChainInfo.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, swapChainInfo.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        swapChainInfo.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, swapChainInfo.presentModes.data());
    }

    return swapChainInfo;
}

VkSurfaceFormatKHR KSVulkan::surfaceChooseSwapFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    //select required format if not available use first but check based on required later
    for(auto &fmt : availableFormats)
    {
        if(fmt.format == VK_FORMAT_B8G8R8A8_SRGB && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return fmt;
    }

    return availableFormats[0];
}

VkPresentModeKHR KSVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
   // return VK_PRESENT_MODE_MAILBOX_KHR;//TODO this is better?
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D KSVulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    //The swap extent is the resolution of the swap chain images and it's almost always exactly equal to the resolution of the window that we're drawing to in pixels (

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width,height;
        vkSurfaceCreator->getFrameBufferSize(width,height);
        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        //clamp
        if(actualExtent.width < capabilities.minImageExtent.width)
            actualExtent.width = capabilities.minImageExtent.width;
        if(actualExtent.width > capabilities.maxImageExtent.width)
            actualExtent.width = capabilities.maxImageExtent.width;

        if(actualExtent.height < capabilities.minImageExtent.height)
            actualExtent.height = capabilities.minImageExtent.height;
        if(actualExtent.height > capabilities.maxImageExtent.height)
            actualExtent.height = capabilities.maxImageExtent.height;

        return actualExtent;
    }
}

bool KSVulkan::createImageViews()
{
    //TODO check working on a stereographic 3D application swapchain 3d images each image can have multiple imageViews for r,l eyes
    //To use any VkImage, including those in the swap chain, in the render pipeline we have to create a VkImageView object
    swapChainImageViews.resize(swapChainImages.size());

    for(int i = 0;i< swapChainImages.size() ; ++i)
    {
        VkImageViewCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;//1d,2d,3d,cubemaps
        createInfo.format = swapChainImageFormat;

        //no rgba swizzle like gbra
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        //The subresourceRange field describes what the image's purpose is and which part of the image should be accessed. Our images will be used as color targets without any mipmapping levels or multiple layers
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        createInfo.pNext = nullptr;
       //TODO createInfo.flags =

       if( vkCreateImageView(vkDevice,&createInfo, nullptr,&swapChainImageViews[i]) != VK_SUCCESS)
           return false;
    }


    return true;
}

VkShaderModule KSVulkan::createShaderModule(const std::vector<char>& spir)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spir.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(spir.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        assert(false);
    }

    return shaderModule;//spir can be freed
}

bool KSVulkan::createGraphicsPipeline()
{

    std::vector<char> vertShader,fragShader;
    assert(ShaderManager::loadShader("trianglefrag.spv",fragShader) && ShaderManager::loadShader("trianglevert.spv",vertShader));


  //  The compilation and linking of the SPIR-V bytecode to machine code for execution by the GPU doesn't happen until the graphics pipeline is created
    VkShaderModule vertShaderModule = createShaderModule(vertShader);
    VkShaderModule fragShaderModule = createShaderModule(fragShader);

    //Shader Stage Creation
    //To actually use the shaders we'll need to assign them to a specific pipeline stage through VkPipelineShaderStageCreateInfo

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    //vertShaderStageInfo.pSpecializationInfo

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    //vertex Input
    //since for now vertex data is directly in shader just setting to nullptr;
    /*
     * Bindings: spacing between data and whether the data is per-vertex or per-instance (see instancing)
     * Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset
     */
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;


    //Input Assembly
    /*
     * The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices and if primitive restart should be enabled.
     * The former is specified in the topology member and can have values like:
     * VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle
     */

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //ViewPort and Scissors WTF? sciss can be set as static part of pipeline or dynamic state in command buffer
    VkViewport viewport{};//FrameBuffer size which will be swapChain Images
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;//Pixels
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;//Exper later
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    //fixed pipeline
    /*
    * While most of the pipeline state needs to be baked into the pipeline state, a limited amount of the state can actually
    * be changed without recreating the pipeline at draw time.
    * Examples are the size of the viewport, line width and blend constants.
    * If you want to use dynamic state and keep these properties out,
    * then you'll have to fill in a VkPipelineDynamicStateCreateInfo structure like this:
    */

    /*
    * This will cause the configuration of these values to be ignored and you will be able (and required)
    * to specify the data at drawing time
    */

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();



    //The actual viewport(s) and scissor rectangle(s) will then later be set up at drawing time.
    // multiple viewports can be enable but requires enabling a GPU feature (see logical device creation).
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;



    //Rasterizer
    /*
     * The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader.
     * It also performs depth testing, face culling and the scissor test,
     * and it can be configured to output fragments that fill entire polygons or just the edges
     * (wireframe rendering). All this is configured using the VkPipelineRasterizationStateCreateInfo structure.
     */

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;//fill,line,point
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    /*
     * If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them.
     * This is useful in some special cases like shadow maps. Using this requires enabling a GPU feature.
     * If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage.
     * This basically disables any output to the framebuffe
     */



    //MultiSampling://TODO later for now disabled
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional


    //Depth and stencil testing TODO


    //Color blending
    /*
     * There are two types of structs to configure color blending. The first struct, VkPipelineColorBlendAttachmentState contains the configuration per attached framebuffer and the second struct, VkPipelineColorBlendStateCreateInfo contains the global color blending settings.
     * In our case we only have one framebuffer:
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    /*
     * The second structure references the array of structures for all of the framebuffers and allows you
     * to set blend constants that you can use as blend factors in the aforementioned calculations.
     */
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optiona



   // The uniform values need to be specified during pipeline creation by creating a VkPipelineLayout object and can be modified
   //without pipeline recreation .The structure also specifies push constants, which are another way of passing dynamic values to shaders
    //Pipeline Layout

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        Logger::error(LOGTAG,"createGraphics Pipeline Layout failed");
        return false;
    }


    /*//createRenderPass
     * Before we can finish creating the pipeline, we need to tell Vulkan about the framebuffer attachments
     * that will be used while rendering. We need to specify how many color and depth buffers there will be,
     * how many samples to use for each of them and how their contents should be handled throughout the rendering operations.
     * All of this information is wrapped in a render pass object,
     */


    /*
     * With all the info pipelinecreate
     * Shader stages: the shader modules that define the functionality of the programmable stages of the graphics pipeline
    Fixed-function state: all of the structures that define the fixed-function stages of the pipeline, like input assembly, rasterizer, viewport and color blending
    Pipeline layout: the uniform and push values referenced by the shader that can be updated at draw time
    Render pass: the attachments referenced by the pipeline stages and their usage
     */


    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO ;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    //structures describing the fixed-function stage.
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    //Pipeline layout a vulkan handle rather than struct pointer
    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    //Vulkan allows you to create a new graphics pipeline by deriving from an existing pipeline.
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; //

    if (vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
       Logger::error(LOGTAG,"failed to create graphics pipeline!");
        return false;
    }
    /*
     * The second parameter, for which we've passed the VK_NULL_HANDLE argument, references an optional VkPipelineCache
     * object. A pipeline cache can be used to store and reuse data relevant to pipeline creation across multiple calls to
     * vkCreateGraphicsPipelines and even across program executions if the cache is stored to a file.
     * This makes it possible to significantly speed up pipeline creation at a later time.
     */

    //TODO
    // vkDestroyShaderModule(vkDevice, fragShaderModule, nullptr);



    return true;
}

bool KSVulkan::createRenderPass()
{
    //Attachment description;//only one from swapchain images
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//no multisampling yet so

    /*
     * The loadOp and storeOp determine what to do with the data in the attachment before rendering and after rendering. We have the following choices for loadOp:

    VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
    VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
    VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
    There are only two possibilities for the storeOp:
    VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
    VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
     */

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    /*
     * Textures and framebuffers in Vulkan are represented by VkImage objects with a certain pixel format, however the layout of the pixels in memory can change based on what you're trying to do with an image.
        Some of the most common layouts are:
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
     */
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    //Subpasses and attachment references
    /*
     * A single render pass can consist of multiple subpasses. Subpasses are subsequent rendering operations that depend
     * on the contents of framebuffers in previous passes, for example a sequence of post-processing effects that are applied
     * one after another. If you group these rendering operations into one render pass, then Vulkan is able to reorder the
     * operations and conserve memory bandwidth for possibly better performance. For our very first triangle, however,
     * we'll stick to a single subpass.
     *
     * Every subpass references one or more of the attachments
     */

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    /*
     * The index of the attachment in this array is directly referenced from the fragment shader
     * with the layout(location = 0) out vec4 outColor
     * The following other types of attachments can be referenced by a subpass:
    pInputAttachments: Attachments that are read from a shader
    pResolveAttachments: Attachments used for multisampling color attachments
    pDepthStencilAttachment: Attachment for depth and stencil data
    pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be preserved
     */

  //With the above info renderpass can be created

  VkRenderPassCreateInfo renderPassInfo{};

    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        Logger::error(LOGTAG,"failed to create render pass!");
        return false;
    }
    return true;
}


