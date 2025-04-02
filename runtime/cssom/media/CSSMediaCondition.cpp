/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "CSSMediaCondition.h"

#include "CSSMediaHelper.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    int CSSMediaCondition::add_item(const NapiGetter& getter) {
        auto item = CSSMediaHelper::createItem(getter.GetNapiValue());
        if (item != nullptr) {
            items_.push_back(item);
            return 0;
        }
        return -1;
    }

    bool CSSMediaCondition::calc_match() {
        match_ = true;
        // 没有元素，默认全匹配
        if (items_.size() == 0) {
            return match_;
        }
        switch (op_) {
            case MediaOpType::NOT: {
                items_[0]->calc_match();
                match_ = !(items_[0]->match());
            } break;
            case MediaOpType::OR: {
                match_ = false;
                for (auto& item : items_) {
                    item->calc_match();
                    if (item->match()) {
                        match_ = true;
                        break;
                    }
                }
            } break;
            case MediaOpType::AND: {
                match_ = true;
                for (auto& item : items_) {
                    item->calc_match();
                    if (!item->match()) {
                        match_ = false;
                        break;
                    }
                }
            } break;
        }
        return match_;
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
