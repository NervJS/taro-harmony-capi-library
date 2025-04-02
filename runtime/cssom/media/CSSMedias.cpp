/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "CSSMedias.h"

#include "helper/TaroLog.h"
#include "runtime/NapiGetter.h"
#include "runtime/cssom/media/CSSMediaHelper.h"

namespace TaroRuntime {
namespace TaroCSSOM {

    int CSSMedia::init(const napi_value &config) {
        NapiGetter getter(config);
        if (getter.Type() != napi_object) {
            return -1;
        }
        auto id_getter = getter.GetProperty("id");
        if (id_getter.Type() != napi_number) {
            return -2;
        }
        id_ = id_getter.Int32Or(0);
        auto napi_conditions = getter.GetProperty("conditions");
        if (napi_conditions.Type() != napi_object) {
            return -3;
        }

        // 转换为一个OR的表达式
        condition_ = std::make_shared<CSSMediaCondition>();
        condition_->setOperate(MediaOpType::OR);
        napi_conditions.ForEachInArray([this](const napi_value &napi_val, const uint32_t &) {
            condition_->add_item(NapiGetter(napi_val));
        });

        // 因为每次切屏后style包括media重新计算，所以初始化时直接进行match
        calc_match();
        return 0;
    }

    void CSSMedia::calc_match() {
        if (condition_ == nullptr) {
            return;
        }
        condition_->calc_match();
        match_ = condition_->match();
    }

    int CSSMedias::init(const napi_value &config) {
        if (config == nullptr) {
            return -1;
        }
        NapiGetter::ForEachInArray(config, [this](const napi_value &napi_val, const uint32_t &) {
            auto media = std::make_shared<CSSMedia>();
            int ret = media->init(napi_val);
            if (ret != 0 || media->id() <= 0) {
                TARO_LOG_ERROR("CSSMedia", "init media failed, ret=%{public}d", ret);
                return;
            }
            medias_[media->id()] = media;
        });
        return 0;
    }

    const CSSMediaPtr CSSMedias::getMedia(int32_t media_id) const {
        const auto iter = medias_.find(media_id);
        if (iter == medias_.end()) {
            return nullptr;
        }
        return iter->second;
    }

    bool CSSMedias::match(int32_t media_id) const {
        // 0为默认media
        if (media_id == 0) {
            return true;
        }
        auto media = getMedia(media_id);
        // 未匹配到media
        if (media == nullptr) {
            return false;
        }
        return media->match();
    }

    void CSSMedias::calc_match() {
        for (auto media : medias_) {
            media.second->calc_match();
        }
    }

} // namespace TaroCSSOM
} // namespace TaroRuntime
