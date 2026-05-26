/* Include guard */
#ifndef VK_INCLUDE_H
#define VK_INCLUDE_H

/* Includes */
#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <logging.h>

/* Check a vulkan result */
#define VK_CHECK(expr) do {\
  VkResult _expr_result = (expr);\
  if (_expr_result != VK_SUCCESS)\
    log_msg(\
        LOG_LEVEL_ERROR, "\"%s\" returned \"%s\"",\
        #expr, string_VkResult(_expr_result)\
    );\
} while (0)

#endif /* VK_INCLUDE_H */
