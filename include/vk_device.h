/* Include guard */
#ifndef VK_DEVICE_H
#define VK_DEVICE_H

/* Includes */
#include <vk_include.h>

/* Check for a specific extension on a specific physical device */
extern bool vk_device_check_for_extension(
    VkPhysicalDevice device, const char *name
);

/* Add a specific device-level extension */
extern void vk_device_add_extension(const char *name);

/* Choose a physical device */
extern void vk_device_choose(uint32_t (*score)(VkPhysicalDevice));
/* Create the vulkan device from a physical device */
extern void vk_device_create(
    VkPhysicalDeviceFeatures *features,
    void *pNext,
    VkDeviceQueueCreateInfo *queue_create_infos,
    uint32_t num_queues
);
/* Destroy the vulkan device */
extern void vk_device_destroy(void);

/* Get the physical device */
extern VkPhysicalDevice vk_physical_device(void);
/* Get the logical device */
extern VkDevice vk_device(void);

#endif /* VK_DEVICE_H */
