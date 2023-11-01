/*
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#include "imgwindow.hpp"
#include <FL/fl_draw.H>

namespace cta {
    ImageWindow::ImageWindow(int x, int y, int w, int h, const char *title) :
        BaseWindow(x,y,w,h,title), threadUpdate_(&ImageWindow::threadUpdateFunc_, std::ref(*this)) {

    }

    ImageWindow::~ImageWindow() {
        threadStop_ = true;
        threadUpdate_.join();
    }

    void ImageWindow::draw() {
        BaseWindow::draw();
        drawCb_();
        fl_draw_box(FL_BORDER_FRAME, 0, 0, w(), h(), FL_WHITE);
    }

    void ImageWindow::resize(int x, int y, int w, int h) {
        if (w != this->w() || h != this->h()) {
            resizeCb_(w,h);
        }
        BaseWindow::resize(x,y,w,h);
    }

    /**
     * Controla a movimentação da janela, permitindo que seja movimentada pelo arrasto do mouse.
    */
    int ImageWindow::handle(int event) {
        static int xoff = 0;
        static int yoff = 0;
        int ret = BaseWindow::handle(event);
        switch(event) {
            case FL_PUSH: {
                mousePressedCb_(Fl::event_button());
                xoff = Fl::event_x_root() - x();
                yoff = Fl::event_y_root() - y();
                ret = 1;
            }
            case FL_DRAG: {
                position(Fl::event_x_root() - xoff, Fl::event_y_root() - yoff);
                redraw();
                ret = 1;
            }
            case FL_RELEASE: {
                show();
                ret = 1;
            }
        }
        return ret;
    }

    void ImageWindow::setLoopCallback(LoopCallback callback) {
        loopCb_ = callback;
        Fl::add_idle([](void* data){
            LoopCallback* pFunc = static_cast<LoopCallback*>(data);
            (*pFunc)();
        }, &loopCb_);
    }

    void ImageWindow::threadUpdateFunc_(ImageWindow& imgWindow) {
        while (!imgWindow.threadStop_) {
            imgWindow.updateThreadCb_();
        }
    }

}