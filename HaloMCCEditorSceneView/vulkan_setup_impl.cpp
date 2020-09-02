#include "EditorVKContext.h"

#include <memory>
#include <set>
#include <cstdint>

#include "EditorVKUtils.hpp"

const std::vector<const char*> defaultValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> requiredExtensions = {
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,

};

const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



std::shared_ptr<EditorVKContext> vkCtx;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif 


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

static VkResult CreateDebugUtilsMessengerEXT(VkInstance vkInstance, const VkDebugUtilsMessengerCreateInfoEXT* debugCreateInfo,
	const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger);

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator);


EditorVKContext::EditorVKContext(HWND hwnd, RECT rect)
	: windowHandle(hwnd), clientRect(rect), vulkanInstance(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE)
{}

void EditorVKContext::InitializeVulkan()
{
	create_vulkan_instance();

	create_debug_messenger();

	create_vulkan_surface();

	select_physical_device();
	create_logical_device();
}

void EditorVKContext::InitializeSwapchain()
{
	swapChainPtr = std::make_unique<EditorVKSwapchain>();
	swapChainPtr->SetClientRect(clientRect);
	swapChainPtr->CreateSwapchain(physicalDevice, logicalDevice, surface);
}

void EditorVKContext::InitializeGraphicsPipeline(const std::string& shaderLocation)
{
	graphicsPipeline = std::make_unique<EditorVKGraphicsPipeline>();
	graphicsPipeline->CreateRenderPass(logicalDevice, swapChainPtr->FramebufferFromat());
	graphicsPipeline->CreateGraphicsPipeline(logicalDevice, swapChainPtr.get(), shaderLocation);

	swapChainPtr->CreateFramebuffers(logicalDevice, graphicsPipeline.get());

	create_command_pool();

	
}

void EditorVKContext::Cleanup()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, nullptr);
	}

	graphicsPipeline->Cleanup(logicalDevice);
	graphicsPipeline.reset();

	swapChainPtr->Cleanup(physicalDevice, logicalDevice);
	swapChainPtr.reset();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
	vkDestroyInstance(vulkanInstance, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
}

void EditorVKContext::BeginFrame()
{
	VkResult result = VK_SUCCESS;
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VkCommandBuffer commandBuffer = commandBuffers[i];

		result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkFramebuffer targetFramebuffer = swapChainPtr->GetFramebuffer(i);

		graphicsPipeline->BeginFrame(commandBuffer, targetFramebuffer, swapChainPtr->Extent());

		result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void EditorVKContext::EndFrame()
{
}

void EditorVKContext::create_vulkan_instance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "MCC Editor Vulkan Runtime";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "(NONE)";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (enableValidationLayers && !check_validation_layers())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(defaultValidationLayers.size());
		createInfo.ppEnabledLayerNames = defaultValidationLayers.data();

		populate_debug_messenger_struct(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
}

void EditorVKContext::create_vulkan_surface()
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = windowHandle;
	createInfo.hinstance = GetModuleHandle(NULL);

	VkResult result = vkCreateWin32SurfaceKHR(vulkanInstance, &createInfo, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
}

void EditorVKContext::create_logical_device()
{
	QueueFamilyIndices indices = utils_QueryQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};


	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(defaultValidationLayers.size());
		createInfo.ppEnabledLayerNames = defaultValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

void EditorVKContext::create_command_pool()
{
	QueueFamilyIndices queueFamilyIndices = utils_QueryQueueFamilies(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional

	VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool!");
	}
	//Create comand buffers
	commandBuffers.resize(swapChainPtr->GetFramebufferCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}



void EditorVKContext::create_debug_messenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populate_debug_messenger_struct(createInfo);

	VkResult wtf = CreateDebugUtilsMessengerEXT(vulkanInstance, &createInfo, nullptr, &debugMessenger);

	if (wtf != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vulkan debug messenger!");
	}
}

void EditorVKContext::select_physical_device()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Faile to find valid GPU with vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());

	for (const VkPhysicalDevice& device : devices)
	{
		if (check_valid_device(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU device!");
	}
}

bool EditorVKContext::check_validation_layers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : defaultValidationLayers)
	{
		bool layerFound = false;
		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

//TODO(Jake): Add support for fallback integrated graphics

bool EditorVKContext::check_valid_device(VkPhysicalDevice device)
{

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	bool extensionsSupported = requiredExtensions.empty();

	QueueFamilyIndices indices = utils_QueryQueueFamilies(device, surface);

	bool validSwapChain = false;
	if (extensionsSupported)
	{
		validSwapChain = swapChainPtr->DeviceHasValidSwapchain(device, surface);
	}

	return indices.isValid() && extensionsSupported && validSwapChain;
}


void EditorVKContext::populate_debug_messenger_struct(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}


static VkResult CreateDebugUtilsMessengerEXT(VkInstance vkInstance, const VkDebugUtilsMessengerCreateInfoEXT* debugCreateInfo,
	const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(vkInstance, debugCreateInfo, allocator, debugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}