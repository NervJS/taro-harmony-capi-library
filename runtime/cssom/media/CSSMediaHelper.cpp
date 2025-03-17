#include "CSSMediaHelper.h"

#include "helper/TaroLog.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    CSSMediaItemPtr CSSMediaHelper::createItem(const napi_value& config) {
        NapiGetter getter(config);
        if (getter.GetType() != napi_object) {
            TARO_LOG_ERROR("CSSMedia", "createItem not napi_object");
            return nullptr;
        }

        auto arr_getter = getter.Vector();
        if (!arr_getter.has_value() || arr_getter.value().empty()) {
            TARO_LOG_ERROR("CSSMedia", "createItem empty napi_object");
            return nullptr;
        }
        auto& op_getter = arr_getter.value()[0];
        if (op_getter.Type() != napi_number) {
            TARO_LOG_ERROR("CSSMedia", "item op:%{public}d is not number",
                           op_getter.Type());
            return nullptr;
        }

        int op = op_getter.Int32Or(0);
        CSSMediaItemPtr ret_item = nullptr;
        // conditions
        if (op > static_cast<int>(MediaOpType::None) && op < static_cast<int>(MediaOpType::MAX)) {
            auto item = std::make_shared<CSSMediaCondition>();
            item->setOperate(static_cast<MediaOpType>(op));
            if (arr_getter.value().size() < 2) {
                TARO_LOG_ERROR("CSSMedia", "invalid size");
                return nullptr;
            }
            arr_getter.value()[1].ForEachInArray([&item](const napi_value& napi_v, const uint32_t&) {
                NapiGetter getter(napi_v);
                item->add_item(getter);
            });
            return item;
        } else if (op == static_cast<int>(MediaOpType::None) && arr_getter.value().size() >= 2) {
            auto feature = std::make_shared<CSSMediaFeature>();
            feature->init(arr_getter.value()[1].GetNapiValue());
            return feature;
        }

        return nullptr;
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
