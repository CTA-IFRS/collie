/*
 * Representa a janela do usuário, controla os eventos gerados.
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#ifndef CTA_WINDOW_HPP
#define CTA_WINDOW_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <opencv2/opencv.hpp>
#include <functional>
#include <thread>
#include <mutex>

namespace cta {
    using ResizeCallback = std::function<void(int,int)>;
    using LoopCallback = std::function<void(void)>;
    using DrawCallback = std::function<void(void)>;
    using UpdateThreadCallback = std::function<void(void)>;
    using MousePressedCallback = std::function<void(int)>;
    using BaseWindow = Fl_Double_Window;

    class ImageWindow : public BaseWindow {
        public:
            ImageWindow(int x, int y, int w, int h, const char *title);
            ~ImageWindow();
            void setResizeCallback(ResizeCallback callback);
            void setLoopCallback(LoopCallback callback); // Callback chamada na mesam thread
            void setDrawCallback(DrawCallback callback);
            void setUpdateThreadCallback(UpdateThreadCallback callback); // Callbask chamada em thread separada
            void setMousePressedCallback(MousePressedCallback callback);

            void draw() override;
            void resize(int x, int y, int w, int h) override;
            int handle(int event) override;

        private:
            ResizeCallback resizeCb_ = [](int,int){};
            LoopCallback loopCb_ = [](){};
            DrawCallback drawCb_ = [](){};
            UpdateThreadCallback updateThreadCb_ = [](){};
            MousePressedCallback mousePressedCb_ = [](int){};
            std::thread threadUpdate_;
            bool threadStop_ = false;

            static void threadUpdateFunc_(ImageWindow& imgWindow);
    };

    inline void ImageWindow::setResizeCallback(ResizeCallback callback) {
        resizeCb_ = callback;
    }
    inline void ImageWindow::setDrawCallback(DrawCallback callback) {
        drawCb_ = callback;
    }
    inline void ImageWindow::setMousePressedCallback(MousePressedCallback callback) {
        mousePressedCb_ = callback;
    }
    inline void ImageWindow::setUpdateThreadCallback(UpdateThreadCallback callback) {
        updateThreadCb_ = callback;
    }

}

#endif

