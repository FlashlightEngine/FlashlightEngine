#pragma once

#if !defined(VK_GLOBAL_FUNCTION)
#define VK_GLOBAL_FUNCTION( fun )
#endif

VK_GLOBAL_FUNCTION(vkCreateInstance)
VK_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties)

#if defined(FL_DEBUG)
VK_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties)
#endif

#undef VK_GLOBAL_FUNCTION