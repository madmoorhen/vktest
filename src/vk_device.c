/* Implements vk_device.h */
#include <vk_device.h>

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <vk_instance.h>

/* State */
static uint32_t num_extensions = 0;
static const char **extensions = NULL;
static VkPhysicalDevice physical_device = 0;
static VkDevice device = 0;
/* TODO: queue families and queues */

/* Check for a specific extension on a specific physical device */
bool vk_device_check_for_extension(
    VkPhysicalDevice device, const char *name
) {
  uint32_t num_exts_supported = 0;
  VkExtensionProperties *exts_supported = NULL;
  VK_CHECK(vkEnumerateDeviceExtensionProperties(
      device, NULL, &num_exts_supported, NULL
  ));
  exts_supported = (VkExtensionProperties *)malloc(
      num_exts_supported * sizeof(VkExtensionProperties)
  );
  VK_CHECK(vkEnumerateDeviceExtensionProperties(
      device, NULL, &num_exts_supported, exts_supported
  ));
  bool found = false;
  for (uint32_t i = 0; i < num_exts_supported; i++) {
    if (!strcmp(name, exts_supported[i].extensionName))
      found = true;
  }
  free(exts_supported);
  return found;
}

/* Add a specific device-level extension */
void vk_device_add_extension(const char *name) {
  if (!extensions)
    extensions = malloc(sizeof(const char *));
  else extensions = realloc(
        extensions, (num_extensions+1) * sizeof(const char *)
    );
  extensions[num_extensions] = name;
  num_extensions++;
}

/* Choose a physical device */
void vk_device_choose(uint32_t (*score)(VkPhysicalDevice)) {
  uint32_t num_phys_devices = 0;
  VkPhysicalDevice *phys_devices = NULL;
  VK_CHECK(vkEnumeratePhysicalDevices(vk_instance(), &num_phys_devices, NULL));
  phys_devices = (VkPhysicalDevice *)malloc(
      num_phys_devices * sizeof(VkPhysicalDevice)
  );
  VK_CHECK(vkEnumeratePhysicalDevices(
      vk_instance(), &num_phys_devices, phys_devices
  ));
  uint32_t max_score = 0;
  int32_t max_index = -1;
  for (uint32_t i = 0; i < num_phys_devices; i++) {
    uint32_t device_score = score(phys_devices[i]);
    if (device_score > max_score) {
      max_score = device_score;
      max_index = i;
    }
  }
  if (max_index < 0) log_msg(LOG_LEVEL_ERROR, "No device chosen");
  physical_device = phys_devices[max_index];
  free(phys_devices);
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  log_msg(LOG_LEVEL_INFO, "Chosen device \"%s\"", properties.deviceName);
}
/* Create the vulkan device from a physical device */
void vk_device_create(
    float *queue_priorities,
    VkDeviceQueueCreateInfo *queue_create_infos,
    uint32_t num_queues
) {
  for (uint32_t i = 0; i < num_extensions; i++) {
    if (!vk_device_check_for_extension(physical_device, extensions[i]))
      log_msg(
          LOG_LEVEL_ERROR, "Couldn't find device extension: \"%s\"",
          extensions[i]
      );
    log_msg(LOG_LEVEL_INFO, "Using device extension: \"%s\"", extensions[i]);
  }
  /* TODO */
  if (extensions) free(extensions);
  extensions = NULL;
  log_msg(LOG_LEVEL_INFO, "Created device");
}
/* Destroy the vulkan device */
void vk_device_destroy(void) { /* TODO */ }

/* Get the physical device */
VkPhysicalDevice vk_physical_device(void) { return physical_device; }
/* Get the logical device */
VkDevice vk_device(void) { return device; }
