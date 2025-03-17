#pragma once

#include "CSSMediaItem.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    // 条件组合
    class CSSMediaCondition : public CSSMediaItem {
        public:
        int add_item(const NapiGetter& getter);

        // 重新计算match值
        bool calc_match() override;

        private:
        std::vector<CSSMediaItemPtr> items_;
    };
    using CSSMediaConditionPtr = std::shared_ptr<CSSMediaCondition>;
} // namespace TaroCSSOM
} // namespace TaroRuntime
