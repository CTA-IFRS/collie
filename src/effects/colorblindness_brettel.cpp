#include "colorblindness_brettel.hpp"

/**
 * Referências: 
 *  Brettel 1997
 *  DaltonLens: https://github.com/DaltonLens/libDaltonLens
*/

namespace cta {
    static const float protanPlane1[9] = {
        0.14980f,1.19548f,-0.34528f,
        0.10764f,0.84864f,0.04372f,
        0.00384f,-0.00540f,1.00156f
    };
    static const float protanPlane2[9] = {            
        0.14570f,1.16172f,-0.30742f,
        0.10816f,0.85291f,0.03892f,
        0.00386f,-0.00524f,1.00139f
    };
    static const float protanNormal[3] = {
        0.00048f,0.00393f,-0.00441f
    };

    static const float deuterPlane1[9] = {
        0.36477f, 0.86381f, -0.22858f,
        0.26294f, 0.64245f, 0.09462f,
        -0.02006f, 0.02728f, 0.99278f,
    };
    static const float deuterPlane2[9] = {            
        0.37298f, 0.88166f, -0.25464f,
        0.25954f, 0.63506f, 0.10540f,
        -0.01980f, 0.02784f, 0.99196f
    };
    static const float deuterNormal[3] = {
        -0.00281f, -0.00611f, 0.00892f
    };

    static const float tritaPlane1[9] = {
        1.01277f, 0.13548f, -0.14826f,
        -0.01243f, 0.86812f, 0.14431f,
        0.07589f, 0.80500f, 0.11911f,
    };
    static const float tritaPlane2[9] = {            
        0.93678f, 0.18979f, -0.12657f,
        0.06154f, 0.81526f, 0.12320f,
        -0.37562f, 1.12767f, 0.24796f,
    };
    static const float tritaNormal[3] = {
        0.03901f, -0.02788f, -0.01113f
    };

    const ColorBlindness_Brettel::CBTypeMatrices ColorBlindness_Brettel::cbTypeList[3] = {
        {protanPlane1, protanPlane2, protanNormal},
        {deuterPlane1, deuterPlane2, deuterNormal},
        {tritaPlane1, tritaPlane2, tritaNormal}
    };

    ColorBlindness_Brettel::ColorBlindness_Brettel(CBType t) : selectedType_(t) {
        
    }

    void ColorBlindness_Brettel::apply(cv::Mat& out) {
        const float* const rawMatPlane1 = cbTypeList[(uint8_t)selectedType_].plane1;
        const float* const rawMatPlane2 = cbTypeList[(uint8_t)selectedType_].plane2;
        const float* const planeNormalRaw = cbTypeList[(uint8_t)selectedType_].normal;

        using Pixel = cv::Point3_<uint8_t>;
        out.forEach<Pixel>([&rawMatPlane1, &rawMatPlane2, &planeNormalRaw](Pixel& p, const int* pos) {
            float lx = sRGBTolinearRGB(p.x);
            float ly = sRGBTolinearRGB(p.y);
            float lz = sRGBTolinearRGB(p.z);
            //cv::Vec3f linearRGB {sRGBTolinearRGB(p.x), sRGBTolinearRGB(p.y), sRGBTolinearRGB(p.z)};
            float dotWithNormPlane = lx * planeNormalRaw[0] + ly * planeNormalRaw[1] + lz * planeNormalRaw[2];
            //float dotWithNormPlane = bData->planeNormal->dot(linearRGB);
            float rx = .0f;
            float ry = .0f;
            float rz = .0f;
            //cv::Vec3f result {.0f,.0f,.0f};
            if (dotWithNormPlane >= .0f) {  
                //result = Mat(*(bData->trm1) * linearRGB);
                rx = lx * rawMatPlane1[0] + ly * rawMatPlane1[1] + lz * rawMatPlane1[2];
                ry = lx * rawMatPlane1[3] + ly * rawMatPlane1[4] + lz * rawMatPlane1[5];
                rz = lx * rawMatPlane1[6] + ly * rawMatPlane1[7] + lz * rawMatPlane1[8];
            } else {
                //result = Mat(*(bData->trm2) * linearRGB);
                rx = lx * rawMatPlane2[0] + ly * rawMatPlane2[1] + lz * rawMatPlane2[2];
                ry = lx * rawMatPlane2[3] + ly * rawMatPlane2[4] + lz * rawMatPlane2[5];
                rz = lx * rawMatPlane2[6] + ly * rawMatPlane2[7] + lz * rawMatPlane2[8];
            }

            float sev = 1.0f;
            // result = result*sev + linearRGB*(1.0f - sev);
            rx = rx * sev + lx*(1.0f-sev);
            ry = ry * sev + ly*(1.0f-sev);
            rz = rz * sev + lz*(1.0f-sev);

            p.x = linearRGBTosRGB(rx);
            p.y = linearRGBTosRGB(ry);
            p.z = linearRGBTosRGB(rz);
        });
    }
}