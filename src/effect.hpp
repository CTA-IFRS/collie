/*
 * Interface que representa um efeito a ser aplicado.
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#ifndef CTA_EFFECT_HPP
#define CTA_EFFECT_HPP

#include <opencv2/opencv.hpp>

namespace cta {
    class Effect {
        public: 
            virtual void apply(cv::Mat& out) = 0;
    };
}
#endif
