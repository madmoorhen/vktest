/* Include guard */
#ifndef VK_DEVICE_H
#define VK_DEVICE_H

/* Includes */
#include <vk_include.h>

/* Check for a specific extension on a specific physical device */
extern bool vk_device_check_for_extension(
    VkPhysicalDevice device, const char *name
);
/* Check for a specific validation layer on a specific physical device */
extern bool vk_device_check_for_layer(
    VkPhysicalDevice device, const char *name
);

/* Add a specific device-level extension */
extern void vk_device_add_extension(const char *name);
/* Add a specific device-level validation layer */
extern void vk_device_add_layer(const char *name);

/* Create the vulkan device from a physical device */
extern void vk_device_create(uint32_t (*rank)(VkPhysicalDevice));
/* Destroy the vulkan device */
extern void vk_device_destroy(void);

#endif /* VK_DEVICE_H */
