#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define ASSERT_VULKAN(val) if(val != VK_SUCCESS) { __debugbreak();}
#define SUPPORTS_FEATURE(val) (val == 1) ? "true" : "false";

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
GLFWwindow* window;


void printStats(const VkPhysicalDevice & device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::cout << "Name:                    " << properties.deviceName << std::endl;
    std::cout << "API version:             " << VK_VERSION_MAJOR(properties.apiVersion) << "." <<
        VK_VERSION_MINOR(properties.apiVersion) << "." <<
        VK_VERSION_PATCH(properties.apiVersion) << std::endl;
    std::cout << "Driver version:          " << properties.driverVersion << std::endl;
    std::cout << "Vendor id:               " << properties.vendorID << std::endl;
    std::cout << "Device id:               " << properties.deviceID << std::endl;
    std::cout << "Device type:             " << properties.deviceType << std::endl;
    
    // This value is needed for the queue priorites in the deviceQueueCreateInfo
    std::cout << "DiscreteQueueProperties: " << properties.limits.discreteQueuePriorities << std::endl;


    // Load features which are supported by the device
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    auto supportsGeometryShader = SUPPORTS_FEATURE(features.geometryShader);
    std::cout << "Geometry shader:         " << supportsGeometryShader << std::endl;

    VkPhysicalDeviceMemoryProperties memProp;
    vkGetPhysicalDeviceMemoryProperties(device, &memProp);

    // Load queue families
    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr);

    VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties);

    std::cout << std::endl << "Amount of queue families:    " << amountOfQueueFamilies << std::endl;

    for (uint32_t i = 0; i < amountOfQueueFamilies; ++i)
    {
        std::cout << std::endl;
        std::cout << "Queue fammily #" << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT:       " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT:        " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT:       " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT: " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "Queue count:                 " << familyProperties[i].queueCount << std::endl;
        std::cout << "Timespan valid bits:         " << familyProperties[i].timestampValidBits << std::endl;

        uint32_t width = familyProperties[i].minImageTransferGranularity.width;
        uint32_t height = familyProperties[i].minImageTransferGranularity.height;
        uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;

        std::cout << "Image transfer granularity:  " << width << ", " << height << ", " << depth << std::endl;
    }

    delete[] familyProperties;

    std::cout << std::endl;
};

void startGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(400, 300, "Vulkan Hello World", nullptr, nullptr);
}

void startVulkan()
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "VulkanHelloWorld";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t amountOfLayers = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
    ASSERT_VULKAN(result);

    VkLayerProperties* layers = new VkLayerProperties[amountOfLayers];
    result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);
    ASSERT_VULKAN(result);

    std::cout << "Amount of instance layers: " << amountOfLayers << std::endl;

    for (uint32_t i = 0; i < amountOfLayers; ++i)
    {
        std::cout << std::endl;
        std::cout << "Name:                   " << layers[i].layerName << std::endl;
        std::cout << "Spec version:           " << layers[i].specVersion << std::endl;
        std::cout << "Implementation version: " << layers[i].implementationVersion << std::endl;
        std::cout << "Description:            " << layers[i].description << std::endl;
    }
    std::cout << std::endl;

    uint32_t amountOfExtensions = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
    ASSERT_VULKAN(result);

    VkExtensionProperties* extensions = new VkExtensionProperties[amountOfExtensions];
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions);

    std::cout << std::endl;
    std::cout << "Amount of extensions: " << amountOfExtensions << std::endl;

    for (uint32_t i = 0; i < amountOfExtensions; ++i)
    {
        std::cout << std::endl;
        std::cout << "Name:         " << extensions[i].extensionName << std::endl;
        std::cout << "Spec version: " << extensions[i].specVersion << std::endl;
    }
    std::cout << std::endl;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    uint32_t amountGLFWExtensions = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountGLFWExtensions);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = amountGLFWExtensions;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    ASSERT_VULKAN(result);

    result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    ASSERT_VULKAN(result);

    // Load amaount of devices
    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
    ASSERT_VULKAN(result);

    // Fill array with devices
    //VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
    std::vector<VkPhysicalDevice> physicalDevices; // also possible with vector
    physicalDevices.resize(amountOfPhysicalDevices);

    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
    ASSERT_VULKAN(result);

    for (uint32_t i = 0; i < amountOfPhysicalDevices; ++i) {
        printStats(physicalDevices[i]);
    }

    float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;

    // These values should be loaded dynamically from the queue families (shown in printStats)
    deviceQueueCreateInfo.queueFamilyIndex = 0; // TODO choose correct family index
    deviceQueueCreateInfo.queueCount = 4; // Todo check if amaount is possible 
    deviceQueueCreateInfo.pQueuePriorities = queuePrios;

    // Features to enable on the device
    VkPhysicalDeviceFeatures usedFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo; // Enhance if more queue families needed
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    // CREATE DEVICE
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device); //TODO: pick best device - instead of first device
    ASSERT_VULKAN(result);

    VkQueue queue;
    // Choose family index correct (look way up)
    vkGetDeviceQueue(device, 0, 0, &queue);

    delete[] layers;
    delete[] extensions;
    //delete[] physicalDevices;
}

void gameLoop()
{
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void shutdownVulkan()
{
    vkDeviceWaitIdle(device);

    // Destroy after all tasks done
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void shutdownGLFW()
{
    glfwDestroyWindow(window);
}

int main() {

    startGLFW();
    startVulkan();
    gameLoop();
    shutdownVulkan();
    shutdownGLFW();

    return 0;
}