/* Include guard */
#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

/* Includes */
#include <vk_include.h>

/* Get the vulkan version */
extern uint32_t vk_instance_get_vulkan_version(void);

/* Check for a specific extension */
extern bool vk_instance_check_for_extension(const char *name);
/* Check for a specific validation layer */ 
extern bool vk_instance_check_for_layer(const char *name);

/* Add a specific instance-level extension */
extern void vk_instance_add_extension(const char *name);
/* Add a specific instance-level validation layer */
extern void vk_instance_add_layer(const char *name);

/* Create the vulkan instance */
extern void vk_instance_create(const char *app_name, uint32_t app_version);
/* Destroy the vulkan instance */
extern void vk_instance_destroy(void);

/* Get the vulkan instance */
extern VkInstance vk_instance(void);

#endif /* VK_INSTANCE_H */
