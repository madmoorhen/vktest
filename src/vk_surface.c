/* Implements vk_surface.h */
#include <vk_surface.h>

/* Includes */
#include <SDL2/SDL_vulkan.h>
#include <vk_instance.h>

/* State */
VkSurfaceKHR surface = VK_NULL_HANDLE;

/* Create the vulkan surface */
extern void vk_surface_create(SDL_Window *window) {
  bool res = SDL_Vulkan_CreateSurface(window, vk_instance(), &surface);
  if (!res) log_msg(
        LOG_LEVEL_ERROR, "Failed to create surface (%s)",
        SDL_GetError()
    );
  log_msg(LOG_LEVEL_INFO, "Created surface");
}
/* Destroy the vulkan surface */
extern void vk_surface_destroy(void) {
  vkDestroySurfaceKHR(vk_instance(), surface, NULL);
  log_msg(LOG_LEVEL_INFO, "Destroyed surface");
}

/* Get the vulkan surface */
extern VkSurfaceKHR vk_surface(void) { return surface; }
