//
//  KSVulkanApplication.hpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#ifndef KSVulkanApplication_hpp
#define KSVulkanApplication_hpp



#include <stdio.h>
#include "Application.hpp"
#include "../Vulkan/KSVulkan.hpp"

class GLFWwindow;

class KSVulkanApplication : public KSApplication{
    
public:
    
    KSVulkanApplication();
    
    virtual ~KSVulkanApplication();
    
    void run() override;
    
    void onWindowInit() override;
    
    void onDestroy() override;

    
private:
    
    bool vulkanInit();
    
private:
    KSVulkan vulkan;
    GLFWwindow* window = nullptr;
    
    const int WIDTH = 800,HEIGHT = 600;
    
};
#endif /* KSVulkanApplication_hpp */
 
