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
#include <set>
#include <cstring>
#include <string>

const int WIDTH = 800;  ///< Width of our window
const int HEIGHT = 600; ///< Height of our window

/// Enable Validation Layers only on Debug build
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/// Names of validation layers that we would like to enable in Debug mode
const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

/// Names of extensions that we need to enable
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/// Load the Debug callback extension and call it to register our callback
VkResult CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pCallback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/// Load the destroy Debug callback extension and call it to unregister our callback
void DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

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

        std::cout << "[init] Create a " << WIDTH << " x " << HEIGHT << " window\n";
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        if (!window) {
            throw std::runtime_error("Cannot create the Window");
        }
    }

    /// Initialize the Vulkan renderer
    void initVulkan() {
        createInstance();
        setupDebugCallback();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    /// Create a Vulkan instance
    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "[init] There are " << extensionCount << " available Instance extensions:\n";
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

        const auto requiredExtensions = getRequiredExtensions();
        std::cout << "[init] GLFW requires the following " << requiredExtensions.size() << " extensions:\n";
        for (const auto& requiredExtension : requiredExtensions) {
            std::cout << "\t" << requiredExtension << std::endl;
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &instance)) {
            throw std::runtime_error("Cannot create the Vulkan instance");
        }
    }

    /// Check if our Vulkan SDK provides the validation layers we would like to use
    // cppcheck-suppress
    bool checkValidationLayerSupport() {
        bool allLayersFound = true;
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        std::cout << "[init] There are " << layerCount << " available validation layers:\n";
        for (const auto& layer : availableLayers) {
            std::cout << "\t" << layer.layerName << std::endl;
        }

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layer : availableLayers) {
                if (strcmp(layerName, layer.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                std::cerr << "[init] Missing validation layer " << layerName << std::endl;
                allLayersFound = false;
            }
        }

        return allLayersFound;
    }

    /// List required Vulkan extensions
    std::vector<const char*> getRequiredExtensions() {
        std::vector<const char*> extensions;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            extensions.push_back(glfwExtensions[i]);
        }

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }

        return extensions;
    }

    /// Vulkan debug callback
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData) {
        HelloTriangleApplication* pMySelf = reinterpret_cast<HelloTriangleApplication*>(userData);
        if (pMySelf) {
            pMySelf->debugCallback(flags, objType, obj, location, code, layerPrefix, msg);
        }

        return VK_FALSE;
    }

    /// Our member debug callback
    void debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg) {
        std::cerr << "[" << layerPrefix << "] " << msg << std::endl;
    }

    /// Enable Debug callback
    void setupDebugCallback() {
        if (enableValidationLayers) {
            VkDebugReportCallbackCreateInfoEXT createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            createInfo.pfnCallback = debugCallback;
            createInfo.pUserData = this;

            if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
                throw std::runtime_error("failed to set up debug callback!");
            }
        }
    }

    /// Create the abstract surface to present the rendered image
    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    /// Select the first GPU that meets the requirements
    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::cout << "[init] There are " << deviceCount << " available physical device(s):\n";
        for (const auto& dev : devices) {
            if (isDeviceSuitable(dev)) {
                physicalDevice = dev;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    /// Check if the GPU meets the requirements
    bool isDeviceSuitable(VkPhysicalDevice dev) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(dev, &deviceProperties);
        vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);

        std::cout << "\t" << deviceProperties.deviceName << " (type " << deviceProperties.deviceType << ")\n";

        const QueueFamilyIndices indices = findQueueFamilies(dev);

        std::cout << "\t => complete=" << indices.isComplete() << std::endl;

        const bool extensionsSupported = checkDeviceExtensionSupport(dev);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(dev);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

    //  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    /// Store indicies of queue families we are looking for
    struct QueueFamilyIndices {
        int graphicsFamily  = -1;   ///< Index of the graphic queue family (to render images)
        int presentFamily   = -1;   ///< Index of the prensentation queue family (to present rendered images)

        /// Check if the device has all required queue families
        bool isComplete() const {
            return (graphicsFamily > -1) && (presentFamily > -1);
        }
    };

    /// Enumerate all queue families to store indices of the one we are looking for
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice dev) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            std::cout << "\t - queueFamily idx " << i
                << " queueCount=" << queueFamily.queueCount
                << " flags=0x" << std::hex << queueFamily.queueFlags << std::dec << std::endl;
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    /// Check that all required extensions are supported
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        std::cout << "[init] We require the following " << extensionCount << " Device extensions:\n";
        for (const auto& extension : requiredExtensions) {
            std::cout << "\t" << extension << std::endl;
        }

        std::cout << "[init] There are " << extensionCount << " available Device extensions:\n";
        for (const auto& extension : availableExtensions) {
            std::cout << "\t" << extension.extensionName << std::endl;
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    /// Swapchain support details (capabilites, formats and presentation modes)
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities; ///< Surface (min/max number of images, min/max width and height)
        std::vector<VkSurfaceFormatKHR> formats;      ///< Surface formats (pixel format, color space)
        std::vector<VkPresentModeKHR>   presentModes; ///< Available presentation modes
    };

    /// Get swapchain support details
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice def) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(def, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(def, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(def, surface, &formatCount, details.formats.data());

            std::cout << "[init] There are " << formatCount << " available surface formats\n";
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(def, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(def, surface, &presentModeCount, details.presentModes.data());

            std::cout << "[init] There are " << presentModeCount << " available presentation modes\n";
        }

        return details;
    }

    /// Select best possible surface format for the swapchain
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            std::cout << "[init] We are free to choose the surface format: using B8G8R8A8 SRGB NONLINEAR\n";
            return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                std::cout << "[init] We have found our prefered surface format: B8G8R8A8 SRGB NONLINEAR\n";
                return availableFormat;
            }
        }

        std::cout << "[init] Just using the first surface format\n";
        return availableFormats[0];
    }

    /// Create a Logical Device to interact with the GPU through Queues
    void createLogicalDevice() {
        std::cout << "[init] Create a Logical Device with Queues\n";

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;
        for (int queueFamily : uniqueQueueFamilies) {
            std::cout << "\t queueFamily=" << queueFamily << std::endl;
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily,  0, &presentQueue);
    }

    /// Run the application and rendering event loop
    void mainLoop() {
        std::cout << "[main] running...\n";
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
        std::cout << "[main] quitting...\n";
    }

    /// Cleanup all ressources before closing
    void cleanup() {
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugReportCallbackEXT(instance, callback, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

private:
    GLFWwindow*                 window          = nullptr;          ///< Pointer to the GLWF Window
    VkInstance                  instance        = 0;                ///< Vulkan instance
    VkDebugReportCallbackEXT    callback        = 0;                ///< Debug callback
    VkSurfaceKHR                surface         = 0;                ///< Abstract surface to prense the rendered image
    VkPhysicalDevice            physicalDevice  = VK_NULL_HANDLE;   ///< Physical Device (GPU)
    VkDevice                    device          = 0;                ///< Logical Device commands the GPU with Queues
    VkQueue                     graphicsQueue   = 0;                ///< Queue to communicate with the GPU
    VkQueue                     presentQueue    = 0;                ///< Queue to present the rendered image
};
