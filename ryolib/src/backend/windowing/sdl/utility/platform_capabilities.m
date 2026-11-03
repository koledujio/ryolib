#import "../private.h"

static RYPlatformCapabilities s_capabilities;
static BOOL s_capabilitiesInitialized = NO;

const RYPlatformCapabilities *ry_platform_get_capabilities(void) {
    if (!s_capabilitiesInitialized) {
#if defined(RYO_APPLE_MAC)
        s_capabilities.platformType = RYPlatformTypeMacOS;
        s_capabilities.deviceType = RYDeviceTypeDesktop;
        s_capabilities.sizeManagementType = RYSizeManagementTypeFreeResizable;
        s_capabilities.isTablet = NO;
        s_capabilities.fixedDefaultOrientation = YES;
        s_capabilities.supportsMultipleWindows = YES;
        s_capabilities.controllerSupport = YES;
        s_capabilities.keyboardSupport = YES;
        s_capabilities.mouseSupport = YES;
        s_capabilities.touchSupport = NO;
        s_capabilities.windowTitlesSupported = YES;
        s_capabilities.nativeWindowHandleSupported = YES;
        s_capabilities.quitSupported = YES;
#elif defined(RYO_WIN32)
        s_capabilities.platformType = RYPlatformTypeWindows;
        s_capabilities.deviceType = RYDeviceTypeDesktop;
        s_capabilities.sizeManagementType = RYSizeManagementTypeFreeResizable;
        s_capabilities.isTablet = NO;
        s_capabilities.fixedDefaultOrientation = YES;
        s_capabilities.supportsMultipleWindows = YES;
        s_capabilities.controllerSupport = YES;
        s_capabilities.keyboardSupport = YES;
        s_capabilities.mouseSupport = YES;
        s_capabilities.touchSupport = YES;
        s_capabilities.windowTitlesSupported = YES;
        s_capabilities.nativeWindowHandleSupported = YES;
        s_capabilities.quitSupported = YES;
#elif defined(RYO_VITA)
        s_capabilities.platformType = RYPlatformTypeVita;
        s_capabilities.deviceType = RYDeviceTypeConsole;
        s_capabilities.sizeManagementType = RYSizeManagementTypeManagedFixed;
        s_capabilities.isTablet = NO;
        s_capabilities.fixedDefaultOrientation = YES;
        s_capabilities.supportsMultipleWindows = NO;
        s_capabilities.controllerSupport = YES;
        s_capabilities.keyboardSupport = YES;
        s_capabilities.mouseSupport = YES;
        s_capabilities.touchSupport = YES;
        s_capabilities.windowTitlesSupported = NO;
        s_capabilities.nativeWindowHandleSupported = NO;
        s_capabilities.quitSupported = YES;
#elif defined(RYO_APPLE_IOS)
        s_capabilities.platformType = RYPlatformTypeIOS;
        s_capabilities.deviceType = RYDeviceTypeMobile;
        s_capabilities.sizeManagementType = RYSizeManagementTypeManagedResizable;
        s_capabilities.isTablet = ry_platform_is_tablet();
        s_capabilities.fixedDefaultOrientation = NO;
        s_capabilities.supportsMultipleWindows = NO;
        s_capabilities.controllerSupport = YES;
        s_capabilities.keyboardSupport = YES;
        s_capabilities.mouseSupport = YES;
        s_capabilities.touchSupport = YES;
        s_capabilities.windowTitlesSupported = NO;
        s_capabilities.nativeWindowHandleSupported = YES;
        s_capabilities.quitSupported = NO;
#else
        #error "Capabilities not defined for target"
#endif
        s_capabilitiesInitialized = YES;
    }

    return &s_capabilities;
}

BOOL ry_platform_get_forced_resolution(unsigned int *outWidth, unsigned int *outHeight) {
#if !defined(RYO_VITA)
    (void)outWidth;
    (void)outHeight;
    return NO;
#else
    if (outWidth) *outWidth = 960;
    if (outHeight) *outHeight = 544;
    return YES;
#endif
}
