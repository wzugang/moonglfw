/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Stefano Trettel
 *
 * Software repository: MoonGLFW, https://github.com/stetre/moonglfw
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

#define VULKAN_NOT_AVAILABLE do { return luaL_error(L, "Vulkan is not available"); } while(0)


#ifdef VULKAN
static const char* ResultString(VkResult rc)
    {
    switch(rc)
        {
        case VK_SUCCESS: return "success";
        case VK_NOT_READY: return "not ready";
        case VK_TIMEOUT: return "timeout";
        case VK_EVENT_SET: return "event set";
        case VK_EVENT_RESET: return "event reset";
        case VK_INCOMPLETE: return "incomplete";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "out of host memory";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "out of device memory";
        case VK_ERROR_INITIALIZATION_FAILED: return "initialization failed";
        case VK_ERROR_DEVICE_LOST: return "device lost";
        case VK_ERROR_MEMORY_MAP_FAILED: return "memory map failed";
        case VK_ERROR_LAYER_NOT_PRESENT: return "layer not present";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "extension not present";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "feature not present";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "incompatible driver";
        case VK_ERROR_TOO_MANY_OBJECTS: return "too many objects";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "format not supported";
        case VK_ERROR_FRAGMENTED_POOL: return "fragmented pool";
        case VK_ERROR_SURFACE_LOST_KHR: return "surface lost khr";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "native window in use khr";
        case VK_SUBOPTIMAL_KHR: return "suboptimal khr";
        case VK_ERROR_OUT_OF_DATE_KHR: return "out of date khr";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "incompatible display khr";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "validation failed ext";
        case VK_ERROR_INVALID_SHADER_NV: return "invalid shader nv";
        case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "out of pool memory";

        default:
            return "unknown vulkan error code";
        }
    return NULL; /* unreachable */
    }
#endif

    
static int VulkanSupported(lua_State *L)
    {
#ifdef VULKAN
    lua_pushboolean(L, glfwVulkanSupported());
#else
    lua_pushboolean(L, 0);
#endif
    return 1;
    }


static int GetRequiredInstanceExtensions(lua_State *L)
    {
#ifdef VULKAN
    uint32_t count, i;
    const char **names = glfwGetRequiredInstanceExtensions(&count);
    lua_newtable(L);
    if((count == 0) || names==NULL)
        return 1;
    for(i=0; i< count; i++)
        {
        lua_pushstring(L, names[i]);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
#else
    VULKAN_NOT_AVAILABLE;
#endif
    }


static int GetPhysicalDevicePresentationSupport(lua_State *L)
    {
#ifdef VULKAN
    VkInstance instance = (VkInstance)checklightuserdata(L, 1);
    VkPhysicalDevice device = (VkPhysicalDevice)checklightuserdata(L, 2);
    uint32_t queuefamily = luaL_checkinteger(L, 3);
    int result = glfwGetPhysicalDevicePresentationSupport(instance, device, queuefamily);
    lua_pushboolean(L, result);
    return 1;
#else
    VULKAN_NOT_AVAILABLE;
#endif
    }


static int CreateWindowSurface(lua_State *L)
    {
#ifdef VULKAN
    VkSurfaceKHR surface;
    win_t *win = checkwindow(L, 1);
    VkInstance instance = (VkInstance)checklightuserdata(L, 2);
    VkAllocationCallbacks* allocator = (VkAllocationCallbacks*)optlightuserdata(L, 3);

    VkResult ec = glfwCreateWindowSurface(instance, win->window, allocator, &surface);

    if(ec != VK_SUCCESS)
        return luaL_error(L, ResultString(ec));
    lua_pushlightuserdata(L, surface);
    return 1;
#else
    VULKAN_NOT_AVAILABLE;
#endif
    }

static int DestroySurface(lua_State *L)
/* destroy_surface(instance_LUD, surface_LUD, allocator_LUD) */
    {
#ifdef VULKAN
    VkInstance instance = (VkInstance)checklightuserdata(L, 1);
    VkSurfaceKHR surface = (VkSurfaceKHR)checklightuserdata(L, 2);
    VkAllocationCallbacks* allocator = (VkAllocationCallbacks*)optlightuserdata(L, 3);
    vkDestroySurfaceKHR(instance, surface, allocator);
    return 0;
#else
    VULKAN_NOT_AVAILABLE;
#endif
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "vulkan_supported", VulkanSupported },
        { "get_required_instance_extensions", GetRequiredInstanceExtensions },
        { "get_physical_device_presentation_support", GetPhysicalDevicePresentationSupport },
        { "create_window_surface", CreateWindowSurface },
        { "destroy_surface", DestroySurface },
        { NULL, NULL } /* sentinel */
    };

void moonglfw_open_vulkan(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }
