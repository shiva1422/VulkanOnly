//
// Created by shivaaz on 12/15/22.
//

#include "TestVKApp.h"
#include "../15_hello_triangle.cpp"
TestVKApp::~TestVKApp() {

}

void TestVKApp::run() {
    runTriangleapp();
  //  KSApplication::run();
}

void TestVKApp::onWindowInit() {
    KSApplication::onWindowInit();
}

void TestVKApp::onDestroy() {
    KSApplication::onDestroy();
}
