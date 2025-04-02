/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include "animation.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class AnimationMultiOption {
    public:
    std::vector<std::string> names_;
    std::vector<std::vector<KeyFrame>> keyframes_;
    std::vector<int32_t> durations_;
    std::vector<std::string> timingFunctions_;
    std::vector<uint32_t> delays_;
    std::vector<int32_t> interationCounts_;
    std::vector<std::string> fill_modes_;
    std::vector<std::string> directions_;

    public:
    bool operator==(const AnimationMultiOption& other) const {
        return names_ == other.names_;
    }
    bool operator!=(const AnimationMultiOption& other) const {
        return names_ != other.names_;
    }
};

class AnimationMulti : public AttributeBase<AnimationMultiOption> {
    public:
    AnimationMulti() = default;
    void setNameFromNapi(const napi_value&);
    void setKeyFramesFromNapi(const napi_value&);
    void setDurationFromNapi(const napi_value&);
    void setTimingFunctionFromNapi(const napi_value&);
    void setFillModeFromNapi(const napi_value&);
    void setDelayFromNapi(const napi_value&);
    void setIterationCountFromNapi(const napi_value&);
    void setValueFromNapi(const napi_value& value) override {}
    void setValueFromStringView(std::string_view value) override {}
    Optional<AnimationOption> getAnimation(size_t index) const;
    Optional<AnimationOption> getAnimation(const std::string& name) const;

    void assign(const AttributeBase<AnimationMultiOption>& item) override;

    private:
    Optional<KeyFrame> parseKeyframe(const ::napi_value& napi_keyframe);
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
