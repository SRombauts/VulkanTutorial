/**
 * @file    Main.cpp
 * @ingroup VulkanTest
 * @brief   Following the Vulkan Tutorial https://vulkan-tutorial.com.
 *
 * Copyright (c) 2017 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
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

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    /// Initialize the Vulkan renderer
    void initVulkan() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::cout << extensionCount << " extensions supported" << std::endl;
    }

    /// Run the application and rendering event loop
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    /// Cleanup all ressources before closing
    void cleanup() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }

private:
    GLFWAPI GLFWwindow* window = nullptr; ///< Pointer to the GLWF Window

    const int WIDTH = 800;  ///< Witdh of our window
    const int HEIGHT = 600; ///< Height of our window
};


/**
 * Entry point of the application
 * 
 * @return 0
 */
int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
