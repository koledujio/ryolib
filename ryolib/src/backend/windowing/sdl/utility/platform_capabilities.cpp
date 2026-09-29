#include "../private.h"

namespace ryo {
    const PlatformCapabilities& _platform_get_capabilities() {
        static const PlatformCapabilities capabilities = []() {
            PlatformCapabilities caps{};
#if defined(RYO_APPLE_MAC)
            caps.platformType = PLATFORM_MACOS;
            caps.deviceType = DEVICE_DESKTOP;
            caps.sizeManagementType = SIZE_FREE_RESIZABLE;
            caps.fixedDefaultOrientation = true;
            caps.supportsMultipleWindows = true;
            caps.controllerSupport = true;
            caps.keyboardSupport = true;
            caps.mouseSupport = true;
            caps.touchSupport = false;
            caps.windowTitlesSupported = true;
            caps.nativeWindowHandleSupported = true;
#elif defined(RYO_WIN32)
            caps.platformType = PLATFORM_WINDOWS;
            caps.deviceType = DEVICE_DESKTOP;
            caps.sizeManagementType = SIZE_FREE_RESIZABLE;
            caps.fixedDefaultOrientation = true;
            caps.supportsMultipleWindows = true;
            caps.controllerSupport = true;
            caps.keyboardSupport = true;
            caps.mouseSupport = true;
            caps.touchSupport = true;
            caps.windowTitlesSupported = true;
            caps.nativeWindowHandleSupported = true;
#elif defined(RYO_VITA)
            caps.platformType = PLATFORM_VITA;
            caps.deviceType = DEVICE_CONSOLE;
            caps.sizeManagementType = SIZE_MANAGED_FIXED;
            caps.fixedDefaultOrientation = true;
            caps.supportsMultipleWindows = false;
            caps.controllerSupport = true;
            caps.keyboardSupport = true;
            caps.mouseSupport = true;
            caps.touchSupport = true;
            caps.windowTitlesSupported = false;
            caps.nativeWindowHandleSupported = false;
#else
            #error "Capabilities not defined for target"
#endif
            return caps;
        }();

        return capabilities;
    }

    bool _platform_get_forced_resolution(unsigned int& outWidth, unsigned int& outHeight) {
        #if !defined(RYO_VITA)
        return false;
        #else
        outWidth = 960;
        outHeight = 544;
        return true;
        #endif
    }
}