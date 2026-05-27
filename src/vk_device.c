/* Implements vk_device.h */
#include <vk_device.h>

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <vk_instance.h>
#include <vk_surface.h>

/* State */
static uint32_t num_extensions = 0;
static const char **extensions = NULL;
static VkPhysicalDevice physical_device = VK_NULL_HANDLE;
static VkDevice device = VK_NULL_HANDLE;

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

/* Get the index of a queue family with the specified properties (or -1) */
int64_t vk_device_get_queue_family(
    VkPhysicalDevice device,
    VkQueueFlags flags, uint32_t min_count, bool presentation_support
) {
  uint32_t num_queue_families = 0;
  VkQueueFamilyProperties *queue_families = NULL;
  vkGetPhysicalDeviceQueueFamilyProperties(
      vk_physical_device(), &num_queue_families, NULL
  );
  queue_families = (VkQueueFamilyProperties *)malloc(
      num_queue_families * sizeof(VkQueueFamilyProperties)
  );
  vkGetPhysicalDeviceQueueFamilyProperties(
      vk_physical_device(), &num_queue_families, queue_families
  );
  int64_t queue_family_index = -1;
  for (uint32_t i = 0; i < num_queue_families; i++) {
    bool flags_valid = queue_families[i].queueFlags & flags;
    bool count_valid = queue_families[i].queueCount >= min_count;
    bool presentation_valid = true;
    if (presentation_support) {
      VkBool32 presentation_supported = false;
      VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
          physical_device, i, vk_surface(), &presentation_supported
      ));
      if (!presentation_supported) presentation_valid = false;
    }
    if (flags_valid && count_valid && presentation_valid) {
      queue_family_index = i;
      break;
    }
  }
  return queue_family_index;
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
    VkPhysicalDeviceFeatures *features,
    void *pNext,
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

  VkDeviceCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = pNext,
    .flags = 0,
    .queueCreateInfoCount = num_queues,
    .pQueueCreateInfos = queue_create_infos,
    .enabledExtensionCount = num_extensions,
    .ppEnabledExtensionNames = extensions,
    .pEnabledFeatures = features
  };
  VK_CHECK(vkCreateDevice(physical_device, &create_info, NULL, &device));

  if (extensions) free(extensions);
  extensions = NULL;
  log_msg(LOG_LEVEL_INFO, "Created device");
}
/* Destroy the vulkan device */
void vk_device_destroy(void) {
  vkDestroyDevice(device, NULL);
  log_msg(LOG_LEVEL_INFO, "Destroyed device");
}

/* Get the physical device */
VkPhysicalDevice vk_physical_device(void) { return physical_device; }
/* Get the logical device */
VkDevice vk_device(void) { return device; }
