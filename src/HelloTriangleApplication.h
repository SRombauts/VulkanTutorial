/**
 * @file    HelloTriangleApplication.h
 * @ingroup VulkanTest
 * @brief   Following the Vulkan Tutorial https://vulkan-tutorial.com.
 *
 * Copyright (c) 2017 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>

/**
 * Vulkan Tutorial Application 
 */
class HelloTriangleApplication {
public:
    /// Init and run the application
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    /// Initialize the window of the application
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        std::cout << "Create a " << WIDTH << " x " << HEIGHT << " window\n";
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        if (!window) {
            throw std::runtime_error("Cannot create the Window");
        }
    }

    /// Initialize the Vulkan renderer
    void initVulkan() {
        createInstance();
    }

    /// Create a Vulkan instance
    void createInstance() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "There are " << extensionCount << " available Vulkan extensions:\n";
        for (const auto& extension : extensions) {
            std::cout << "\t" << extension.extensionName << std::endl;
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::cout << "GLFW requires the following " << glfwExtensionCount << " extensions:\n";
        for (size_t i = 0; i < glfwExtensionCount; ++i) {
            std::cout << "\t" << glfwExtensions[i] << std::endl;
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &instance)) {
            throw std::runtime_error("Cannot create the Vulkan instance");
        }
    }

    /// Run the application and rendering event loop
    void mainLoop() {
        std::cout << glfwGetTime() << ": main loop...\n";
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
        std::cout << glfwGetTime() << ": quitting...\n";
    }

    /// Cleanup all ressources before closing
    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

private:
    GLFWwindow* window      = nullptr;  ///< Pointer to the GLWF Window
    VkInstance  instance    = 0;        ///< Vulkan instance


    const int WIDTH = 800;  ///< Width of our window
    const int HEIGHT = 600; ///< Height of our window
};
