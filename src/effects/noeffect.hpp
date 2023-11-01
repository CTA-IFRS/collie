#ifndef CTA_NO_EFFECT_HPP
#define CTA_NO_EFFECT_HPP

#include "../effect.hpp"

namespace cta {

class NoEffect : public Effect {
    public:
        void apply(cv::Mat& out) override;
};

}

#endif