// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <GLFW/glfw3.h>

#include <vector>

namespace skeleton {

namespace {

constexpr char kApplicationName[] = "Skeleton";
constexpr char kEngineName[] = "Skeleton";

}  // namespace

VulkanRenderer::VulkanRenderer() {
  if (volkInitialize() != VK_SUCCESS) {
    throw RendererCreationException("Failed to initialise the Vulkan loader.");
  }
  try {
    CreateInstance();
    SelectPhysicalDevice();
    CreateLogicalDevice();
  } catch (...) {
    Cleanup();
    throw;
  }
}

VulkanRenderer::~VulkanRenderer() {
  Cleanup();
}

RendererBackend VulkanRenderer::GetBackend() const {
  return RendererBackend::kVulkan;
}

void VulkanRenderer::SetWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanRenderer::InitialiseForWindow(GLFWwindow* window) {}

void VulkanRenderer::Render() {}

VkInstance VulkanRenderer::Instance() const {
  return instance_;
}

VkPhysicalDevice VulkanRenderer::PhysicalDevice() const {
  return physical_device_;
}

VkDevice VulkanRenderer::Device() const {
  return device_;
}

VkQueue VulkanRenderer::GraphicsQueue() const {
  return graphics_queue_;
}

void VulkanRenderer::CreateInstance() {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = kApplicationName;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = kEngineName;
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw RendererCreationException("Failed to create the Vulkan instance.");
  }
  volkLoadInstance(instance_);
}

void VulkanRenderer::SelectPhysicalDevice() {
  uint32_t device_count = 0;
  if (vkEnumeratePhysicalDevices(instance_, &device_count, nullptr) !=
          VK_SUCCESS ||
      device_count == 0) {
    throw RendererCreationException("No Vulkan physical devices found.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (VkPhysicalDevice device : devices) {
    if (HasGraphicsQueueFamily(device)) {
      physical_device_ = device;
      return;
    }
  }
  throw RendererCreationException(
      "No Vulkan physical device with a graphics queue found.");
}

void VulkanRenderer::CreateLogicalDevice() {
  const float queue_priority = 1.0f;
  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = graphics_queue_family_;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = 1;
  create_info.pQueueCreateInfos = &queue_create_info;

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) !=
      VK_SUCCESS) {
    throw RendererCreationException(
        "Failed to create the Vulkan logical device.");
  }
  volkLoadDevice(device_);
  vkGetDeviceQueue(device_, graphics_queue_family_, 0, &graphics_queue_);
}

bool VulkanRenderer::HasGraphicsQueueFamily(VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());
  for (uint32_t i = 0; i < queue_families.size(); ++i) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_queue_family_ = i;
      return true;
    }
  }
  return false;
}

void VulkanRenderer::Cleanup() {
  if (device_ != VK_NULL_HANDLE) {
    vkDestroyDevice(device_, nullptr);
    device_ = VK_NULL_HANDLE;
  }
  if (instance_ != VK_NULL_HANDLE) {
    vkDestroyInstance(instance_, nullptr);
    instance_ = VK_NULL_HANDLE;
  }
}

}  // namespace skeleton
