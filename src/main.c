/* Includes */
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <logging.h>
#include <vk_instance.h>
#include <vk_device.h>

/* Score the physical devices */
uint32_t score_device(VkPhysicalDevice dev) {
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

/* Entry point */
int main(int argc, char *argv[]) {
  /* Create window */
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow(
      "vktest", 0, 0, 800, 400,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE// | SDL_WINDOW_VULKAN
  );

  /* Log version */
  uint32_t version = vk_instance_get_vulkan_version();
  log_msg(
      LOG_LEVEL_INFO, "API Version: %d.%d.%d",
      VK_API_VERSION_MAJOR(version),
      VK_API_VERSION_MINOR(version),
      VK_API_VERSION_PATCH(version)
  );
  
  /* Create instance */
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

  /* Create device */
  vk_device_choose(score_device);
  vk_device_add_extension("VK_KHR_swapchain");
  vk_device_create();

  /* Main loop */
  bool running = true;
  while (running) {
    /* Window events */
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

  /* Destroy device */
  vk_device_destroy();
  /* Destroy instance */
  vk_instance_destroy();

  /* Destroy window */
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
