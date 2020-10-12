#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define ASSERT_VULKAN(val) if(val != VK_SUCCESS) { __debugbreak();}
#define SUPPORTS_FEATURE(val) (val == 1) ? "true" : "false";

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
GLFWwindow* window;
const uint32_t WIDTH = 400;
const uint32_t HEIGHT = 300;



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

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
    std::cout << std::endl << "Surface capabilities:" << std::endl;
    std::cout << "\tminImageCount:           " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "\tmaxImageCount:           " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "\tcurrentExtent:           " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "\tminImageExtent:          " << surfaceCapabilities.minImageExtent.width << "/" << surfaceCapabilities.minImageExtent.height << std::endl;
    std::cout << "\tmaxImageExtent:          " << surfaceCapabilities.maxImageExtent.width << "/" << surfaceCapabilities.maxImageExtent.height << std::endl;
    std::cout << "\tmaxImageArrayLayers:     " << surfaceCapabilities.maxImageArrayLayers << std::endl;
    std::cout << "\tsupportedTransforms:     " << surfaceCapabilities.supportedTransforms << std::endl;
    std::cout << "\tcurrentTransform:        " << surfaceCapabilities.currentTransform << std::endl;
    std::cout << "\tsupportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    std::cout << "\tsupportedUsageFlags:     " << surfaceCapabilities.supportedUsageFlags << std::endl;

    // COLOR FORMATS
    uint32_t amountOfFormats = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, nullptr);
    VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);
    std::cout << std::endl;
    std::cout << "Amount of formats: " << amountOfFormats << std::endl;

    for (uint32_t i = 0; i < amountOfFormats; ++i)
    {
        std::cout << "Format: " << surfaceFormats[i].format << std::endl;
    }

    uint32_t amountOfPresentationModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, nullptr);
    VkPresentModeKHR* presentModes = new VkPresentModeKHR[amountOfPresentationModes];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, presentModes);
    std::cout << std::endl;
    std::cout << "Amount of presentation modes: " << amountOfPresentationModes << std::endl;
    for (uint32_t i = 0; i < amountOfPresentationModes; ++i) {
        std::cout << "Supported presentation mode: " << presentModes[i] << std::endl;
    }

    std::cout << std::endl;
    delete[] familyProperties;
    delete[] surfaceFormats;
    delete[] presentModes;
};

void startGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Hello World", nullptr, nullptr);
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

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo; // Enhance if more queue families needed
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    // CREATE DEVICE
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device); //TODO: pick best device - instead of first device
    ASSERT_VULKAN(result);

    //CHECK IF DEVICES SUPPORTS SURFACE
    VkBool32 surfaceSupport = false;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
    ASSERT_VULKAN(result);

    if (!surfaceSupport) {
        std::cerr << "Surface not supported!" << std::endl;
        __debugbreak();
    }

    VkQueue queue;
    // Choose family index correct (look way up)
    vkGetDeviceQueue(device, 0, 0, &queue);

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 3; // TODO check dynamically for graphicscard
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; // TODO check dynamically for graphicscard
    swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR; // TODO check dynamically for graphicscard
    swapchainCreateInfo.imageExtent = VkExtent2D{WIDTH, HEIGHT};
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //TODO check
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // window is not transparent
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.clipped = VK_TRUE; // clip pixels outside of the image
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);

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
    vkDestroySwapchainKHR(device, swapchain, nullptr);
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