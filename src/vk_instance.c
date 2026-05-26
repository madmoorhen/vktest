/* Implements vk_instance.h */
#include <vk_instance.h>

/* Includes */
#include <stdlib.h>
#include <string.h>

/* Debug callback */
#ifdef DEBUG
VkBool32 debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData
) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      log_msg(
          LOG_LEVEL_WARNING,
          "Validation Layer: %s",
          pCallbackData->pMessage
      );
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      log_msg(
          LOG_LEVEL_ERROR,
          "Validation Layer: %s",
          pCallbackData->pMessage
      );
      break;
    default:
      log_msg(
          LOG_LEVEL_INFO,
          "Validation Layer: %s",
          pCallbackData->pMessage
      );
      break;
  }
        
  return VK_TRUE;
}
#endif

/* State */
static uint32_t num_extensions = 0;
static const char **extensions = NULL;
static uint32_t num_layers = 0;
static const char **layers = NULL;
static VkInstance instance = 0;
#ifdef DEBUG
static VkDebugUtilsMessengerEXT messenger = 0;
#endif

/* Get the vulkan version */
uint32_t vk_instance_get_vulkan_version(void) {
  PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion =
      (PFN_vkEnumerateInstanceVersion)(vkGetInstanceProcAddr(
          NULL, "vkEnumerateInstanceVersion"
      ));
  if (!(vkEnumerateInstanceVersion)) return VK_API_VERSION_1_0;
  uint32_t result;
  VK_CHECK(vkEnumerateInstanceVersion(&result));
  return result;
}

/* Check for a specific extension */
bool vk_instance_check_for_extension(const char *name) {
  uint32_t num_exts_supported = 0;
  VkExtensionProperties *exts_supported = NULL;
  VK_CHECK(vkEnumerateInstanceExtensionProperties(
      NULL, &num_exts_supported, NULL
  ));
  exts_supported = (VkExtensionProperties *)malloc(
      num_exts_supported * sizeof(VkExtensionProperties)
  );
  VK_CHECK(vkEnumerateInstanceExtensionProperties(
      NULL, &num_exts_supported, exts_supported
  ));
  bool found = false;
  for (uint32_t i = 0; i < num_exts_supported; i++) {
    if (!strcmp(name, exts_supported[i].extensionName))
      found = true;
  }
  free(exts_supported);
  return found;
}
/* Check for a specific validation layer */ 
bool vk_instance_check_for_layer(const char *name) {
  uint32_t num_layers_supported = 0;
  VkLayerProperties *layers_supported = NULL;
  VK_CHECK(vkEnumerateInstanceLayerProperties(&num_layers_supported, NULL));
  layers_supported = (VkLayerProperties *)malloc(
      num_layers_supported * sizeof(VkLayerProperties)
  );
  VK_CHECK(vkEnumerateInstanceLayerProperties(
      &num_layers_supported, layers_supported
  ));
  bool found = false;
  for (uint32_t i = 0; i < num_layers_supported; i++) {
    if (!strcmp(name, layers_supported[i].layerName))
      found = true;
  }
  free(layers_supported);
  return found;
}

/* Add a specific extension */
void vk_instance_add_extension(const char *name) {
  if (!extensions)
    extensions = malloc(sizeof(const char *));
  else extensions = realloc(
        extensions, (num_extensions+1) * sizeof(const char *)
    );
  extensions[num_extensions] = name;
  num_extensions++;
}
/* Add a specific validation layer */
void vk_instance_add_layer(const char *name) {
  if (!layers)
    layers = malloc(sizeof(const char *));
  else layers = realloc(
        layers, (num_layers+1) * sizeof(const char *)
    );
  layers[num_layers] = name;
  num_layers++;
}

/* Create the vulkan instance */
void vk_instance_create(const char *app_name, uint32_t app_version) {
#ifdef DEBUG
  vk_instance_add_extension("VK_EXT_debug_utils");
#endif
  for (uint32_t i = 0; i < num_extensions; i++) {
    if (!vk_instance_check_for_extension(extensions[i])) log_msg(
          LOG_LEVEL_ERROR, "Couldn't find instance extension: \"%s\"",
          extensions[i]
      );
    log_msg(LOG_LEVEL_INFO, "Using instance extension: \"%s\"", extensions[i]);
  }
#ifdef DEBUG
  for (uint32_t i = 0; i < num_layers; i++) {
    if (!vk_instance_check_for_layer(layers[i])) log_msg(
          LOG_LEVEL_ERROR, "Couldn't find instance layer: \"%s\"",
          layers[i]
      );
    log_msg(LOG_LEVEL_INFO, "Using instance layer: \"%s\"", layers[i]);
  }
#endif

#ifdef DEBUG
  VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = NULL,
    .flags = 0,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
      /*| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,*/
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debug_callback,
    .pUserData = NULL
  };
#endif
  VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = app_name,
    .applicationVersion = app_version,
    .pEngineName = "madmoorhenvk",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_3
  };
  VkInstanceCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef DEBUG
    .pNext = &messenger_create_info,
#else
    .pNext = NULL,
#endif
    .flags = 0,
    .pApplicationInfo = &app_info,
#ifdef DEBUG
    .enabledLayerCount = num_layers,
    .ppEnabledLayerNames = layers,
#else
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
#endif
    .enabledExtensionCount = num_extensions,
    .ppEnabledExtensionNames = extensions
  };
  VK_CHECK(vkCreateInstance(&create_info, NULL, &instance));
#ifdef DEBUG
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT"
      ));
  VK_CHECK(vkCreateDebugUtilsMessengerEXT(
      instance, &messenger_create_info, NULL, &messenger
  ));
#endif

  if (extensions) free(extensions);
  if (layers) free(layers);
  extensions = NULL;
  layers = NULL;
  log_msg(LOG_LEVEL_INFO, "Created instance");
}
/* Destroy the vulkan instance */
void vk_instance_destroy(void) {
#ifdef DEBUG
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT"
      ));
  vkDestroyDebugUtilsMessengerEXT(instance, messenger, NULL);
#endif
  vkDestroyInstance(instance, NULL);
  log_msg(LOG_LEVEL_INFO, "Destroyed instance");
}

/* Get the vulkan instance */
VkInstance vk_instance(void) { return instance; }
