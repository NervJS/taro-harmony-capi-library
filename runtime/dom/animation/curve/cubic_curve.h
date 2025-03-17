#pragma once

#include "curve.h"

namespace TaroRuntime {
namespace TaroAnimate {
    // Third-order bezier curve. Formula as follows:
    // B(m) = (1-m)^3*P0 + 3m(1-m)^2*P1 + 3m^2*P2 + m^3*P3，
    // where P0 = (0,0), P1 = (x0_, y0_), P2 = (x1_, y1_),  P3 = (1,1)
    // so Bx(m) = 3m(1-m)^2*x0_ + 3m^2*x1_ + m^3
    //    By(m) = 3m(1-m)^2*y0_ + 3m^2*y1_ + m^3
    class TaroCubicCurve final : public TaroCurve {
        public:
        TaroCubicCurve(float x0, float y0, float x1, float y1);
        ~TaroCubicCurve() = default;
        float moveInternal(float time) override;
        const std::string ToString() override;

        private:
        // Bx(m) or By(m) = 3m(1-m)^2*a + 3m^2*b + m^3, where a = x0_ ,b = x1_ or a =
        // y0_ ,b = y1_
        static float calculateCubic(float a, float b, float m);
        float cubic_error_bound_ = 0.001f; // Control curve accuracy
        float x0_;                         // X-axis of the first point (P1)
        float y0_;                         // Y-axis of the first point (P1)
        float x1_;                         // X-axis of the second point (P2)
        float y1_;                         // Y-axis of the second point (P2)
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
