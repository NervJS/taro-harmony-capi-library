//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void HarmonyStyleSetter::setPointerEvents(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {}
/**
 * 目前无论是NODE_HIT_TEST_BEHAVIOR还是NODE_RESPONSE_REGION都只能让本身的事件不响应，子组件挡不住，
 * 如果要跟w3c的标准一致，需要增加递归让所有子组件的响应配置同步
 * @param node
 * @param val
 */
void HarmonyStyleSetter::setPointerEvents(const ArkUI_NodeHandle& node, const int& val) {

    if (auto value = static_cast<PropertyType::PointerEvents>(val); value == PropertyType::PointerEvents::None) {

//         setFloat(node, NODE_RESPONSE_REGION, 0, 0, 0, 0);
        setInt(node, NODE_HIT_TEST_BEHAVIOR, ARKUI_HIT_TEST_MODE_NONE);
    } else {
        TARO_LOG_DEBUG("setPointerEvents", "=+= setPointerEvents 2" );
//         NativeNodeApi::getInstance()->resetAttribute(node, NODE_RESPONSE_REGION);
        setFloat(node, NODE_HIT_TEST_BEHAVIOR, ARKUI_HIT_TEST_MODE_DEFAULT);
    }
}
void HarmonyStyleSetter::setPointerEvents(const ArkUI_NodeHandle& node, const Optional<PropertyType::PointerEvents>& val) {
    if (val.has_value()) {
        setPointerEvents(node, static_cast<int>(val.value()));
    }
}

void HarmonyStyleSetter::setPointerEvents(const ArkUI_NodeHandle& node) {
//     NativeNodeApi::getInstance()->resetAttribute(node, NODE_RESPONSE_REGION);
    setInt(node, NODE_HIT_TEST_BEHAVIOR, ARKUI_HIT_TEST_MODE_DEFAULT);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet