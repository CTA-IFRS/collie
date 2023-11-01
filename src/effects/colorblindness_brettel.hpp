/*
 * Representa o efeito a ser aplicado na imagem capturada. 
 * Referências: 
 *  Brettel 1997
 *  DaltonLens: https://github.com/DaltonLens/libDaltonLens
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#ifndef CTA_DALTON_PROTANOPIA_HPP
#define CTA_DALTON_PROTANOPIA_HPP

#include "../effect.hpp"

namespace cta {
    class ColorBlindness_Brettel : public Effect {
        public:
            enum class CBType : uint8_t {
                PROTANOPIA = 0,
                DEUTERANOPIA = 1,
                TRITANOPIA = 2
            };

            ColorBlindness_Brettel(CBType t = CBType::PROTANOPIA);
            void apply(cv::Mat& out) override;

        private:
            CBType selectedType_;

            struct CBTypeMatrices {
                const float* plane1;
                const float* plane2;
                const float* normal;
            };

            static const CBTypeMatrices cbTypeList[3];

            static float sRGBTolinearRGB(unsigned char v);
            static unsigned char linearRGBTosRGB(float v);
    };

    // Referência: https://github.com/DaltonLens/libDaltonLens
    inline float ColorBlindness_Brettel::sRGBTolinearRGB(unsigned char v) {
        float fv = v / 255.f;
        if (fv < 0.04045f) return fv / 12.92f;
        return powf((fv + 0.055f) / 1.055f, 2.4f);
    }

    // Referência: https://github.com/DaltonLens/libDaltonLens
    inline unsigned char ColorBlindness_Brettel::linearRGBTosRGB(float v){
        if (v <= 0.f) return 0;
        if (v >= 1.f) return 255;
        if (v < 0.0031308f) return static_cast<unsigned int>(0.5f + (v * 12.92f * 255.0f));
        return static_cast<unsigned int>(0.f + 255.f * (powf(v, 1.f / 2.4f) * 1.055f - 0.055f));
    }
   
}

#endif