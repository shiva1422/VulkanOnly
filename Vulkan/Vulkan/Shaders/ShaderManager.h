//
// Created by shivaaz on 12/5/22.
//

#ifndef VULKANMAIN_SHADERMANAGER_H
#define VULKANMAIN_SHADERMANAGER_H
#include "vector"
#include "../Logger.h"
#include <fstream>

class ShaderManager{

public:

static bool loadShader(const char* name,std::vector<char> &shader)
{
    Logger::verbose("ShaderManager ","path : %s",(shaderPath + "/" + name).c_str());
   std::ifstream file(shaderPath + "/" + name,std::ios::ate |std::ios::binary);
   if(!file.is_open())
   {
       return false;
   }

    size_t fileSize = (size_t) file.tellg();
   shader.resize(fileSize);
   file.seekg(0);
   file.read(shader.data(),fileSize);
   file.close();
   return true;

}

private:
   static  std::string shaderPath;

};
#endif //VULKANMAIN_SHADERMANAGER_H
