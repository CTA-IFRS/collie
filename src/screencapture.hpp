/*
 * Representa a entidade que irá capturar o conteúdo atrás da janela do usuário.
 * Sua definição depende do SO utilizado.
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#ifndef CTA_SCREENCAP_HPP
#define CTA_SCREENCAP_HPP

#include <opencv2/opencv.hpp>
#include <FL/Fl_Window.H>
#include <memory>

namespace cta {

    struct Rect {
        double x, y;
        double w, h;
    };

    class ScreenCapture {
        public:
            ScreenCapture(cv::Mat& buffer);
            cv::Mat& captureWindowBackContentArea(const Fl_Window* currWindow);
            ~ScreenCapture();

        private:
            struct OSImpl;
            std::unique_ptr<OSImpl> osImpl_;
    };

}

#endif

