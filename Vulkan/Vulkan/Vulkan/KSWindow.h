//
// Created by shivaaz on 11/28/22.
//

#ifndef VULKANMAIN_KSWINDOW_H
#define VULKANMAIN_KSWINDOW_H

class KSWindow{

public:
    KSWindow() = delete;
   /* void* createWindow(int width,int height)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(W, HEIGHT, LOGTAG , nullptr, nullptr);
    }*/

private:
    void *instance = nullptr;
    bool created = false;
};

class KSWindowVKInterface
{
public:
  virtual bool createVKSurface() = 0;
  virtual void getFrameBufferSize(int &width,int &height) = 0;
};
#endif //VULKANMAIN_KSWINDOW_H
