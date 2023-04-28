//
// Created by shivaaz on 12/15/22.
//

#ifndef VULKANMAIN_TESTVKAPP_H
#define VULKANMAIN_TESTVKAPP_H

#include "../Application.hpp"
class TestVKApp : public KSApplication{
public:
    ~TestVKApp() override;

    void run() override;

protected:
    void onWindowInit() override;

    void onDestroy() override;

};


#endif //VULKANMAIN_TESTVKAPP_H
