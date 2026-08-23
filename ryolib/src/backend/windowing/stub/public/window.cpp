#include "ryolib/windowing.h"

namespace ryo {
    struct InternalWindow {};

    Window::Window(const char* title, unsigned int width, unsigned int height) {}
    Window::~Window() {}

    Window::Window(Window&&) {}
    Window& Window::operator=(Window&&) {return *this;}

    bool Window::valid() const {return true;}

    bool Window::event_poll(Event& event) {
        event.type = EVENT_NONE;
        return false;
    }

    void* Window::nwh_get() {return nullptr;}
}