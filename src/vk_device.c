/* Implements vk_device.h */
#include <vk_device.h>

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <vk_instance.h>

/* State */
static uint32_t num_extensions = 0;
static const char **extensions = NULL;
static uint32_t num_layers = 0;
static const char **layers = NULL;
/* TODO: VkDevice VkPhysicalDevice, queue families and queues */

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
/* Check for a specific validation layer on a specific physical device */
bool vk_device_check_for_layer(
    VkPhysicalDevice device, const char *name
) {
  uint32_t num_layers_supported = 0;
  VkLayerProperties *layers_supported = NULL;
  VK_CHECK(vkEnumerateDeviceLayerProperties(
      device, &num_layers_supported, NULL
  ));
  layers_supported = (VkLayerProperties *)malloc(
      num_layers_supported * sizeof(VkLayerProperties)
  );
  VK_CHECK(vkEnumerateDeviceLayerProperties(
      device, &num_layers_supported, layers_supported
  ));
  bool found = false;
  for (uint32_t i = 0; i < num_layers_supported; i++) {
    if (!strcmp(name, layers_supported[i].layerName))
      found = true;
  }
  free(layers_supported);
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
/* Add a specific device-level validation layer */
void vk_device_add_layer(const char *name) {
  if (!layers)
    layers = malloc(sizeof(const char *));
  else layers = realloc(
        layers, (num_layers+1) * sizeof(const char *)
    );
  layers[num_layers] = name;
  num_layers++;
}

/* Create the vulkan device from a physical device */
void vk_device_create(uint32_t (*rank)(VkPhysicalDevice)) {
  uint32_t num_phys_devices = 0;
  VkPhysicalDevice *phys_devices = NULL;
  VK_CHECK(vkEnumeratePhysicalDevices(vk_instance(), &num_phys_devices, NULL));
  phys_devices = (VkPhysicalDevice *)malloc(
      num_phys_devices * sizeof(VkPhysicalDevice)
  );
  VK_CHECK(vkEnumeratePhysicalDevices(
      vk_instance(), &num_phys_devices, phys_devices
  ));
  for (size_t i = 0; i < num_phys_devices; i++) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(phys_devices[i], &properties);
    bool valid = true;
    for (size_t j = 0; j < num_extensions; j++) {
      if (!vk_device_check_for_extension(phys_devices[i], extensions[j])) {
        valid = false;
        log_msg(
            LOG_LEVEL_WARNING,
            "Device \"%s\" missing extension \"%s\" - device will be skipped",
            properties.deviceName, extensions[j]
        );
      }
    }
    if (!valid) continue;
    /* TODO */
  }

  /* TODO */
  free(extensions);
  free(layers);
  extensions = NULL;
  layers = NULL;
  log_msg(LOG_LEVEL_INFO, "Created device");
}
/* Destroy the vulkan device */
void vk_device_destroy(void) { /* TODO */ }
