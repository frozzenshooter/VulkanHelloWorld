#include "vulkan/vulkan.h"
#include <iostream>

#define ASSERT_VULKAN(val) if(val != VK_SUCCESS) { __debugbreak();}

#define SUPPORTS_FEATURE(val) (val == 1) ? "true" : "false";

VkInstance instance;
VkDevice device;

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
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, NULL);

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

int main() {

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "VulkanHelloWorld";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = NULL;

    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);
    ASSERT_VULKAN(result);

    // Load amaount of devices
    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, NULL);
    ASSERT_VULKAN(result);

    // Fill array with devices
    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices);
    ASSERT_VULKAN(result);

    for (uint32_t i = 0; i < amountOfPhysicalDevices; ++i) {
        printStats(physicalDevices[i]);
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = NULL;
    deviceQueueCreateInfo.flags = 0;

    // These values should be loaded dynamically from the queue families (shown in printStats)
    deviceQueueCreateInfo.queueFamilyIndex = 0; // TODO choose correct family index
    deviceQueueCreateInfo.queueCount = 4; // Todo check if amaount is possible 
    deviceQueueCreateInfo.pQueuePriorities = NULL;

    // Features to enable on the device
    VkPhysicalDeviceFeatures usedFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo; // Enhance if more queue families needed
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL; 
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    // CREATE DEVICE
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, NULL, &device); //TODO: pick best device - instead of first device
    ASSERT_VULKAN(result);


    delete[] physicalDevices;

    return 0;
}