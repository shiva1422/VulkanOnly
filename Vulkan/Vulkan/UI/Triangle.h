//
// Created by shivaaz on 12/5/22.
//

#ifndef VULKANMAIN_TRIANGLE_H
#define VULKANMAIN_TRIANGLE_H

#include "vector"
#include "../Shaders/ShaderManager.h"
class Triangle {

protected:
    Triangle()
    {
        if(!shaderLoaded)
        {
            shaderLoaded = ShaderManager::loadShader("triangle.vert",vertShader) &&
                    ShaderManager::loadShader("triangle.frag",fragShader);
        }
    }
    bool shaderLoaded = false;//TODO static
    std::vector<char> vertShader;
    std::vector<char> fragShader;
};


#endif //VULKANMAIN_TRIANGLE_H
