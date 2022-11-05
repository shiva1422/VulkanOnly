//
//  KSVulkan.cpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#include "KSVulkan.hpp"

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

bool KSVulkan::createInstance()
{
   
    return true;
}

bool KSVulkan::createSurface()
{
    return true;
}




