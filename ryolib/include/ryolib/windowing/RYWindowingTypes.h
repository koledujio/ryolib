#pragma once

#include <stdint.h>
#import "../objc/RYObject.h"

/**
 * Additional Types.
 */

typedef uint32_t RYWindowID;

/**
 * Platform and hardware capabilities.
 */

/**
 * @brief Platform type.
 */
typedef enum {
    RYPlatformTypeUnknown = 0, ///< Unknown platform.
    RYPlatformTypeMacOS,       ///< macOS.
    RYPlatformTypeWindows,     ///< Windows.
    RYPlatformTypeVita,        ///< Playstation Vita.
    RYPlatformTypeIOS          ///< iOS/iPadOS.
} RYPlatformType;

/**
 * @brief Device type.
 */
typedef enum {
    RYDeviceTypeUnknown = 0, ///< Unknown device type.
    RYDeviceTypeDesktop,     ///< Desktop computer (macOS, Windows).
    RYDeviceTypeMobile,      ///< Mobile phone/tablet (iOS/iPadOS).
    RYDeviceTypeConsole      ///< Console (Playstation Vita).
} RYDeviceType;

/**
 * @brief Window size management type.
 */
typedef enum {
    RYSizeManagementTypeFreeFixed,       ///< Size set manually, not resizable.
    RYSizeManagementTypeFreeResizable,   ///< Size set manually, resizable (macOS, Windows).
    RYSizeManagementTypeManagedFixed,    ///< Size dictated by OS, not resizable (Vita).
    RYSizeManagementTypeManagedResizable ///< Size dictated by OS, resizable (iOS/iPadOS).
} RYSizeManagementType;

/**
 * @brief Platform capabilities.
 */
typedef struct {
    RYPlatformType platformType;             ///< Platform / operating system.
    RYDeviceType deviceType;                 ///< Device form factor.
    RYSizeManagementType sizeManagementType; ///< Sizing and resizing behavior.

    BOOL isTablet;                    ///< Tablet form factor (iPadOS).
    BOOL fixedDefaultOrientation;     ///< True if platform supports only one default orientation.
    BOOL supportsMultipleWindows;     ///< Support for multiple windows.
    BOOL controllerSupport;           ///< Specifies if the platform supports gamepads.
    BOOL keyboardSupport;             ///< Specifies if the platform supports keyboards.
    BOOL mouseSupport;                ///< Specifies if the platform supports mouse input.
    BOOL touchSupport;                ///< Specifies if the platform supports touch input.
    BOOL windowTitlesSupported;       ///< Specifies if the platform supports custom window titles.
    BOOL nativeWindowHandleSupported; ///< Specifies if the platform supports native window handles.
    BOOL quitSupported;               ///< Specifies if the platform supports program-invoked quit.
} RYPlatformCapabilities;

/**
 * Window and display configuration.
 */

/**
 * @brief Window behavior flags bitmask.
 */
typedef enum {
    RYWindowFlagNone                = 0,      ///< Default window.
    RYWindowFlagResizable           = 1 << 0, ///< Allows user resizing.
    RYWindowFlagFullscreen          = 1 << 1, ///< Spans entire screen.
    RYWindowFlagBorderless          = 1 << 2, ///< Removes window decorations.
    RYWindowFlagStrictOrientations  = 1 << 3  ///< Honors requested orientations on iPadOS.
} RYWindowFlags;

/**
 * @brief Display orientations bitmask.
 */
typedef enum {
    RYDisplayOrientationNone              = 0,
    RYDisplayOrientationLandscape         = 1 << 0,
    RYDisplayOrientationPortrait          = 1 << 1,
    RYDisplayOrientationLandscapeFlipped  = 1 << 2,
    RYDisplayOrientationPortraitFlipped   = 1 << 3,
    RYDisplayOrientationAll               = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3)
} RYDisplayOrientations;

/**
 * @brief Safe area insets.
 */
typedef struct {
    float top;
    float bottom;
    float left;
    float right;
} RYSafeAreaInsets;

static inline RYSafeAreaInsets RYSafeAreaInsetsMake(float top, float bottom, float left, float right) {
    RYSafeAreaInsets insets;
    insets.top = top;
    insets.bottom = bottom;
    insets.left = left;
    insets.right = right;
    return insets;
}

/**
 * @brief Window 2D logical size.
 */
typedef struct {
    unsigned int width;
    unsigned int height;
} RYWindowSize;

static inline RYWindowSize RYWindowSizeMake(unsigned int width, unsigned int height) {
    RYWindowSize s;
    s.width = width;
    s.height = height;
    return s;
}

/**
 * Input definitions.
 */

/**
 * @brief Keyboard key codes.
 */
typedef enum {
    RYKeyCodeUnknown = 0,

    RYKeyCodeA, RYKeyCodeB, RYKeyCodeC, RYKeyCodeD, RYKeyCodeE, RYKeyCodeF, RYKeyCodeG,
    RYKeyCodeH, RYKeyCodeI, RYKeyCodeJ, RYKeyCodeK, RYKeyCodeL, RYKeyCodeM, RYKeyCodeN,
    RYKeyCodeO, RYKeyCodeP, RYKeyCodeQ, RYKeyCodeR, RYKeyCodeS, RYKeyCodeT, RYKeyCodeU,
    RYKeyCodeV, RYKeyCodeW, RYKeyCodeX, RYKeyCodeY, RYKeyCodeZ,

    RYKeyCode1, RYKeyCode2, RYKeyCode3, RYKeyCode4, RYKeyCode5,
    RYKeyCode6, RYKeyCode7, RYKeyCode8, RYKeyCode9, RYKeyCode0,

    RYKeyCodeEscape, RYKeyCodeReturn, RYKeyCodeSpace, RYKeyCodeTab, RYKeyCodeBackspace,
    RYKeyCodeUp, RYKeyCodeDown, RYKeyCodeLeft, RYKeyCodeRight,
    RYKeyCodeLShift, RYKeyCodeRShift, RYKeyCodeLCtrl, RYKeyCodeRCtrl, RYKeyCodeLAlt, RYKeyCodeRAlt,

    RYKeyCodeF1, RYKeyCodeF2, RYKeyCodeF3, RYKeyCodeF4, RYKeyCodeF5, RYKeyCodeF6,
    RYKeyCodeF7, RYKeyCodeF8, RYKeyCodeF9, RYKeyCodeF10, RYKeyCodeF11, RYKeyCodeF12,
    RYKeyCodeF13, RYKeyCodeF14, RYKeyCodeF15, RYKeyCodeF16, RYKeyCodeF17, RYKeyCodeF18,
    RYKeyCodeF19, RYKeyCodeF20, RYKeyCodeF21, RYKeyCodeF22, RYKeyCodeF23, RYKeyCodeF24
} RYKeyCode;

/**
 * @brief Gamepad types.
 */
typedef enum {
    RYGamepadTypeUnknown = 0,
    RYGamepadTypeXbox360,
    RYGamepadTypeXboxOne,
    RYGamepadTypePS3,
    RYGamepadTypePS4,
    RYGamepadTypePS5,
    RYGamepadTypeSwitchPro,
    RYGamepadTypeSwitchJoyConLeft,
    RYGamepadTypeSwitchJoyConRight,
    RYGamepadTypeSwitchJoyConPair,
    RYGamepadTypeGameCube,
    RYGamepadTypeSteam
} RYGamepadType;

/**
 * @brief Gamepad controller button types.
 */
typedef enum {
    RYGamepadButtonNone = 0,

    RYGamepadButtonDpadUp,
    RYGamepadButtonDpadDown,
    RYGamepadButtonDpadLeft,
    RYGamepadButtonDpadRight,

    RYGamepadButtonUp,
    RYGamepadButtonLeft,
    RYGamepadButtonRight,
    RYGamepadButtonDown,

    RYGamepadButtonStart,
    RYGamepadButtonSelect,

    RYGamepadButtonL1,
    RYGamepadButtonR1,

    RYGamepadButtonL3,
    RYGamepadButtonR3,

    RYGamepadButtonGuide
} RYGamepadButton;

/**
 * @brief Gamepad axis types.
 */
typedef enum {
    RYGamepadAxisNone = 0,
    RYGamepadAxisLeftX,
    RYGamepadAxisLeftY,
    RYGamepadAxisRightX,
    RYGamepadAxisRightY,
    RYGamepadAxisTriggerLeft,
    RYGamepadAxisTriggerRight
} RYGamepadAxis;

/**
 * @brief Type of @ref RYEvent.
 */
typedef enum {
    RYEventTypeNone = 0,
    RYEventTypeQuit,
    RYEventTypeWindowResized,
    RYEventTypeWindowHiDPIScaleChanged,
    RYEventTypeWindowSafeAreaChanged,
    RYEventTypePadAdded,
    RYEventTypePadRemoved,
    RYEventTypePadButtonDown,
    RYEventTypePadButtonUp,
    RYEventTypePadAxisMotion,
    RYEventTypeKeyDown,
    RYEventTypeKeyUp,
    RYEventTypeTouchBegin,
    RYEventTypeTouchMove,
    RYEventTypeTouchEnd,
    RYEventTypeDeviceOrientationChanged,
    RYEventTypePlatformDesktopUnfocused,
    RYEventTypePlatformDesktopFocused,
    RYEventTypePlatformMobileWillEnterBackground,
    RYEventTypePlatformMobileWillEnterForeground,
    RYEventTypePlatformMobileLowMemory
} RYEventType;

/**
 * @brief Window and input event.
 */
typedef struct {
    RYEventType type;
    RYWindowID windowId;
    union {
        struct {
            unsigned int width;
            unsigned int height;
        } window;

        struct {
            int id;
            RYGamepadButton button;
            RYGamepadAxis axis;
            float axisValue;
        } pad;

        struct {
            RYKeyCode code;
            BOOL isRepeat;
        } key;

        RYDisplayOrientations orientation;

        struct {
            unsigned int fingerId;
            float x;
            float y;
            float deltaX;
            float deltaY;
        } touch;
    } data;
} RYEvent;
