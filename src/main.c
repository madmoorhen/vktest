/* Includes */
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <logging.h>
#include <vk_instance.h>
#include <vk_surface.h>
#include <vk_device.h>

/* State */
static SDL_Window *window = NULL;
static bool running = false;
static uint32_t queue_family_index = 0;
static VkQueue queue = VK_NULL_HANDLE;
static VkSurfaceCapabilitiesKHR surface_capabilities;
static VkSwapchainKHR swapchain = VK_NULL_HANDLE;

/* Score the physical devices */
static uint32_t score_device(VkPhysicalDevice dev) {
  uint32_t score = 0;
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(dev, &properties);
  switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: score += 900; break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: score += 800; break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: score += 600; break;
    default: break;
  }
  return score;
}
/* Create window */
static void create_window(void) {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow(
      "vktest", 0, 0, 800, 400,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE// | SDL_WINDOW_VULKAN
  );
}
/* Log version */
static void log_version(void) {
  uint32_t version = vk_instance_get_vulkan_version();
  log_msg(
      LOG_LEVEL_INFO, "API Version: %d.%d.%d",
      VK_API_VERSION_MAJOR(version),
      VK_API_VERSION_MINOR(version),
      VK_API_VERSION_PATCH(version)
  );
}
/* Create instance */
static void create_instance(void) {
  uint32_t num_required_exts = 0;
  const char **required_exts = NULL;
  SDL_Vulkan_GetInstanceExtensions(window, &num_required_exts, NULL);
  required_exts = malloc(num_required_exts * sizeof(const char *));
  SDL_Vulkan_GetInstanceExtensions(window, &num_required_exts, required_exts);
  for (uint32_t i = 0; i < num_required_exts; i++) {
    vk_instance_add_extension(required_exts[i]);
  }
  vk_instance_add_layer("VK_LAYER_KHRONOS_validation");
  vk_instance_create("vktest", VK_MAKE_VERSION(1, 0, 0));
}
/* Create device */
static void create_device(void) {
  vk_device_choose(score_device);
  vk_device_add_extension("VK_KHR_swapchain");
  vk_device_add_extension("VK_KHR_buffer_device_address");
  int64_t queue_family = vk_device_get_queue_family(
      vk_physical_device(), VK_QUEUE_GRAPHICS_BIT, 1, true
  );
  if (queue_family < 0)
    log_msg(LOG_LEVEL_ERROR, "Failed to find suitable queue family");
  queue_family_index = (uint32_t)queue_family;
  const float queue_priorities[] = { 1.0f };
  VkDeviceQueueCreateInfo queue_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = queue_family_index,
    .queueCount = 1,
    .pQueuePriorities = queue_priorities
  };
  VkPhysicalDeviceFeatures features = { .samplerAnisotropy = VK_TRUE };
  VkPhysicalDeviceVulkan12Features features12 = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
    .pNext = NULL,
    .descriptorIndexing = VK_TRUE,
    .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
    .descriptorBindingVariableDescriptorCount = VK_TRUE,
    .runtimeDescriptorArray = VK_TRUE,
    .bufferDeviceAddress = VK_TRUE
  };
  VkPhysicalDeviceVulkan13Features features13 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .pNext = &features12,
      .synchronization2 = VK_TRUE,
      .dynamicRendering = VK_TRUE,
  };
  vk_device_create(&features, &features13, &queue_create_info, 1);
  vkGetDeviceQueue(vk_device(), queue_family_index, 0, &queue);
}
/* Create swapchain */
static void create_swapchain(void) {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      vk_physical_device(), vk_surface(), &surface_capabilities
  ));
  VkSwapchainCreateInfoKHR create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    .surface = vk_surface(),
    .minImageCount = surface_capabilities.minImageCount,
    .imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
    .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
    .imageExtent = surface_capabilities.currentExtent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &queue_family_index,
    .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_FIFO_KHR,
    .clipped = VK_TRUE,
    .oldSwapchain = VK_NULL_HANDLE
  };
  VK_CHECK(vkCreateSwapchainKHR(
      vk_device(), &create_info, NULL, &swapchain
  ));
}

/* Main loop */
static void main_loop(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          log_msg(
              LOG_LEVEL_INFO, "resized: %d, %d",
              event.window.data1, event.window.data2
          );
        }
        break;
      default:
        break;
    }
  }
}
/* Cleanup */
static void cleanup(void) {
  vkDestroySwapchainKHR(vk_device(), swapchain, NULL);
  vk_device_destroy();
  vk_surface_destroy();
  vk_instance_destroy();

  SDL_DestroyWindow(window);
  SDL_Quit();
}

/* Entry point */
int main(int argc, char *argv[]) {
  create_window();
  log_version();
  create_instance();
  vk_surface_create(window);
  create_device();

  running = true;
  while (running) { main_loop(); }

  cleanup();
  return 0;
}
