// Copyright 2026 aljabari

#include "renderer/vulkan/vulkaninstance.h"

#include <spdlog/spdlog.h>

#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

constexpr char kApplicationName[] = "Skeleton";
constexpr char kEngineName[] = "Skeleton";

}  // namespace

VulkanInstance::VulkanInstance() {
  if (volkInitialize() != VK_SUCCESS) {
    spdlog::error("Failed to initialise the Vulkan loader.");
    throw RendererCreationException("Failed to initialise the Vulkan loader.");
  }

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
    spdlog::error("Failed to create the Vulkan instance.");
    throw RendererCreationException("Failed to create the Vulkan instance.");
  }
  volkLoadInstance(instance_);
  spdlog::debug("Created Vulkan instance.");
}

VulkanInstance::~VulkanInstance() {
  if (instance_ != VK_NULL_HANDLE) {
    vkDestroyInstance(instance_, nullptr);
  }
}

VkInstance VulkanInstance::Instance() const {
  return instance_;
}

std::vector<VkPhysicalDevice> VulkanInstance::EnumeratePhysicalDevices() const {
  uint32_t device_count = 0;
  if (vkEnumeratePhysicalDevices(instance_, &device_count, nullptr) !=
          VK_SUCCESS ||
      device_count == 0) {
    spdlog::error("No Vulkan physical devices found.");
    throw RendererCreationException("No Vulkan physical devices found.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());
  return devices;
}

}  // namespace skeleton
