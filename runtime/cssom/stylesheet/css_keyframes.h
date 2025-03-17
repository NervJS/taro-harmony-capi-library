#pragma once

#include <js_native_api.h>

#include "animation.h"
#include "runtime/cssom/media/CSSMedias.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
struct KeyframesInfo {
    std::string name_;
    int media_id_ = 0;
    std::vector<KeyFrame> keyframes_;
};
using KeyframesInfoPtr = std::shared_ptr<KeyframesInfo>;

class CSSKeyframes {
    public:
    CSSKeyframes() = default;
    ~CSSKeyframes() = default;

    // 解析并过滤keyframes
    int init(napi_value config, CSSMediasPtr medias);

    const KeyframesInfoPtr getAnimKeyframes(const std::string& name) const;

    uint64_t getVersion() const;

    private:
    // 解析keyframes
    KeyframesInfoPtr parseKeyframes(const napi_value& napi_keyframes);

    // 解析每个keyframe
    Optional<KeyFrame> parseKeyframe(const napi_value& napi_keyframe);

    std::unordered_map<std::string, KeyframesInfoPtr> name_to_keyframes_;

    uint64_t version_ = 0;
};
using CSSKeyframesPtr = std::shared_ptr<CSSKeyframes>;

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet