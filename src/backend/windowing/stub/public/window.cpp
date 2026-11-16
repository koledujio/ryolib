#include "ryolib/windowing.h"

namespace ryo {
    struct InternalWindow {};

    Window::Window(
        const Application& app,
        const char* title,
        unsigned int width,
        unsigned int height,
        unsigned int flags,
        unsigned int allowedOrientations
    ) {}

    Window::~Window() {}

    Window::Window(Window&&) {}
    Window& Window::operator=(Window&&) {return *this;}

    bool Window::valid() const {return false;}

    WindowID Window::id_get() const {return 0;}

    void* Window::nwh_get() const {return nullptr;}

    const char* Window::title_get() const {return nullptr;}

    bool Window::title_set(const char* title) {return false;}

    bool Window::size_get(unsigned int& outWidth, unsigned int& outHeight) const {return false;}

    bool Window::size_set(unsigned int width, unsigned int height) {return false;}

    bool Window::size_min_get(unsigned int& outWidth, unsigned int& outHeight) const {return false;}

    bool Window::size_min_set(unsigned int width, unsigned int height) {return false;}

    bool Window::size_max_get(unsigned int& outWidth, unsigned int& outHeight) const {return false;}

    bool Window::size_max_set(unsigned int width, unsigned int height) {return false;}

    float Window::aspect_ratio_get() const {return 0.0f;}

    float Window::hidpi_scale_get() const {return 0.0f;}

    SafeAreaInsets Window::safe_area_insets_get() const {return {};}

    unsigned int Window::flags_get() const {return WINDOW_FLAG_NONE;}

    bool Window::flags_set(unsigned int flags) {return false;}

    unsigned int Window::orientations_get() const {return ORIENTATION_NONE;}

    bool Window::orientations_set(unsigned int orientations) {return false;}

    unsigned int Window::orientation_get() const {return ORIENTATION_NONE;}
}