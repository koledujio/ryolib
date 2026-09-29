#include "../private.h"

namespace ryo {
    PlatformCapabilities _platform_get_capabilities() {
        PlatformCapabilities capabilities;

#if defined(RYO_APPLE_MAC)
        capabilities.platformType = PLATFORM_MACOS;
        capabilities.deviceType = DEVICE_DESKTOP;
        capabilities.sizeManagementType = SIZE_FREE_RESIZABLE;
        capabilities.fixedDefaultOrientation = true;
        capabilities.supportsMultipleWindows = true;
        capabilities.controllerSupport = true;
        capabilities.keyboardSupport = true;
        capabilities.mouseSupport = true;
        capabilities.touchSupport = false;
        capabilities.windowTitlesSupported = true;
        capabilities.nativeWindowHandleSupported = true;
#elif defined(RYO_WIN32)
        capabilities.platformType = PLATFORM_WINDOWS;
        capabilities.deviceType = DEVICE_DESKTOP;
        capabilities.sizeManagementType = SIZE_FREE_RESIZABLE;
        capabilities.fixedDefaultOrientation = true;
        capabilities.supportsMultipleWindows = true;
        capabilities.controllerSupport = true;
        capabilities.keyboardSupport = true;
        capabilities.mouseSupport = true;
        capabilities.touchSupport = true;
        capabilities.windowTitlesSupported = true;
        capabilities.nativeWindowHandleSupported = true;
#elif defined(RYO_VITA)
        capabilities.platformType = PLATFORM_VITA;
        capabilities.deviceType = DEVICE_CONSOLE;
        capabilities.sizeManagementType = SIZE_MANAGED_FIXED;
        capabilities.fixedDefaultOrientation = true;
        capabilities.supportsMultipleWindows = false;
        capabilities.controllerSupport = true;
        capabilities.keyboardSupport = true;
        capabilities.mouseSupport = true;
        capabilities.touchSupport = true;
        capabilities.windowTitlesSupported = false;
        capabilities.nativeWindowHandleSupported = false;
#else
        #error "Capabilities not defined for target"
#endif

    return capabilities;
    }

    bool _platform_get_forced_resolution(unsigned int& outWidth, unsigned int& outHeight) {
        #if !defined(RYO_VITA)
        return false;
        #elif
        outWidth = 960;
        outHeight = 544;
        return true;
        #endif
    }
}