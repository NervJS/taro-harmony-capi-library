#pragma once

#include "CSSMediaItem.h"
#include "runtime/cssom/dimension/dimension.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    using CSSMediaValue = std::variant<double>;

    class CSSMediaFeature : public CSSMediaItem {
        public:
        virtual ~CSSMediaFeature() = default;
        int init(const napi_value& config);

        // 重新计算match值
        bool calc_match() override;

        protected:
        // 计算value的值，并将值进行归一化处理，方便后续比较计算
        int init_value(NapiGetter& getter);

        int compare_double(double cur_val);

        MediaFeatureId feature_id_ = MediaFeatureId::Invalid;

        MediaCompType comp_type_ = MediaCompType::Equal;

        double value_;
    };

    using CSSMediaFeaturePtr = std::shared_ptr<CSSMediaFeature>;
} // namespace TaroCSSOM
} // namespace TaroRuntime
