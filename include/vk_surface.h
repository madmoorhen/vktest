/* Include guard */
#ifndef VK_SURFACE_H
#define VK_SURFACE_H

/* Includes */
#include <vk_include.h>
#include <SDL2/SDL.h>

/* Create the vulkan surface */
extern void vk_surface_create(SDL_Window *window);
/* Destroy the vulkan surface */
extern void vk_surface_destroy(void);

/* Get the vulkan surface */
extern VkSurfaceKHR vk_surface(void);

#endif /* VK_SURFACE_H */
