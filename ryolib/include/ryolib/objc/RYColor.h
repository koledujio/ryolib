#pragma once

/**
 * @brief Represents an RGBA color with normalized 32-bit floating point components.
 * 
 * Each channel ranges from 0.0f to 1.0f.
 */
typedef struct RYColor {
    float r; /**< Red component, from 0.0f to 1.0f. */
    float g; /**< Green component, from 0.0f to 1.0f. */
    float b; /**< Blue component, from 0.0f to 1.0f. */
    float a; /**< Alpha component, from 0.0f (transparent) to 1.0f (opaque). */
} RYColor;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Constructs an RYColor structure.
 * 
 * @param r Red component (0.0f to 1.0f).
 * @param g Green component (0.0f to 1.0f).
 * @param b Blue component (0.0f to 1.0f).
 * @param a Alpha component (0.0f to 1.0f).
 * @return An initialized RYColor struct.
 */
static inline RYColor RYColorMake(float r, float g, float b, float a) {
    RYColor c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

#if defined(__cplusplus)
}
#endif
