#ifndef ABOUT_WINDOW_HPP
#define ABOUT_WINDOW_HPP

#include "fl-gui/aboutgui.hpp"

namespace cta {
    class AboutWindow : public FluidAboutWindow {
        public:
           using FluidAboutWindow::FluidAboutWindow;
           
           AboutWindow();
    }; 
}

#endif

