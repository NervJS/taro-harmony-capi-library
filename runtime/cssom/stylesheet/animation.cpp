/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./animation.h"

#include "./attribute_base.h"
#include "runtime/NapiGetter.h"
#include "runtime/cssom/stylesheet/border_left_width.h"
#include "runtime/cssom/stylesheet/border_top_color.h"
#include "runtime/cssom/stylesheet/border_top_width.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        Animation::Animation(const napi_value &napiValue) {}

        void Animation::setNameFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);

            if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    item_.name = getterValue.value();
                    this->set(item_);
                }
            }
        }

        void Animation::setKeyFramesFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type != napi_object) return;

            std::vector<KeyFrame> frames;
            getter.ForEachInArray([&](const napi_value &elem, const uint32_t &_) {
                KeyFrame keyFrameItems;
                NapiGetter keyframeItemGetter(elem);
                keyframeItemGetter.GetType(type);
                if (type != napi_object) return;
                auto percentageGetter = keyframeItemGetter.GetProperty("percentage");
                auto eventGetter = keyframeItemGetter.GetProperty("event");

                // 如果百分比的字段不是数字
                percentageGetter.GetType(type);
                if (type != napi_number) return;
                // 如果event不是一个对象
                eventGetter.GetType(type);
                if (type != napi_object) return;

                // 设置 percentage
                auto value = percentageGetter.Double();
                if (value.has_value()) {
                    keyFrameItems.percent = value.value();
                }

                // 设置 event
                eventGetter.ForEachInArray([&](const napi_value &item, const uint32_t &_) {
                    NapiGetter itemGetter(item);
                    itemGetter.GetType(type);

                    auto keyGetter = itemGetter.GetProperty("0");
                    auto valueGetter = itemGetter.GetProperty("1");

                    napi_valuetype keyType, valueType;
                    keyGetter.GetType(keyType);
                    valueGetter.GetType(valueType);
                    if (keyType == napi_number && valueType != napi_undefined) {
                        auto keyOpt = keyGetter.Int32();
                        if (keyOpt.has_value()) {
                            CSSProperty::Type type =
                                static_cast<CSSProperty::Type>(keyOpt.value());
                            switch (type) {
                                case CSSProperty::Type::Left:
                                case CSSProperty::Type::Right:
                                case CSSProperty::Type::Top:
                                case CSSProperty::Type::Bottom:
                                case CSSProperty::Type::Width:
                                case CSSProperty::Type::MarginBottom:
                                case CSSProperty::Type::MarginLeft:
                                case CSSProperty::Type::MarginRight:
                                case CSSProperty::Type::MarginTop:
                                case CSSProperty::Type::PaddingBottom:
                                case CSSProperty::Type::PaddingLeft:
                                case CSSProperty::Type::PaddingRight:
                                case CSSProperty::Type::PaddingTop:
                                case CSSProperty::Type::BorderBottomWidth:
                                case CSSProperty::Type::BorderRightWidth:
                                case CSSProperty::Type::BorderLeftWidth:
                                case CSSProperty::Type::BorderTopWidth: {
                                    AttributeBase<Dimension> lengthAttr;
                                    lengthAttr.setValueFromNapi(valueGetter.GetNapiValue());
                                    if (lengthAttr.has_value()) {
                                        keyFrameItems.params.emplace_back(type, lengthAttr.value());
                                    }
                                    break;
                                }
                                case CSSProperty::Type::BorderTopLeftRadius:
                                case CSSProperty::Type::BorderTopRightRadius:
                                case CSSProperty::Type::BorderBottomLeftRadius:
                                case CSSProperty::Type::BorderBottomRightRadius:
                                case CSSProperty::Type::BackgroundPositionX:
                                case CSSProperty::Type::BackgroundPositionY: {
                                    AttributeBase<Dimension> tLenthAttr;
                                    tLenthAttr.setValueFromNapi(valueGetter.GetNapiValue());
                                    if (tLenthAttr.has_value()) {
                                        keyFrameItems.params.emplace_back(type, tLenthAttr.value());
                                    }
                                } break;
                                case CSSProperty::Type::Color:
                                case CSSProperty::Type::BackgroundColor:
                                case CSSProperty::Type::BorderTopColor:
                                case CSSProperty::Type::BorderRightColor:
                                case CSSProperty::Type::BorderBottomColor:
                                case CSSProperty::Type::BorderLeftColor: {
                                    AttributeBase<TColor> colorAttr;
                                    colorAttr.setValueFromNapi(valueGetter.GetNapiValue());
                                    if (colorAttr.has_value()) {
                                        keyFrameItems.params.emplace_back(
                                            type, static_cast<double>(colorAttr.value()));
                                    }
                                    break;
                                }

                                case CSSProperty::Type::Opacity: {
                                    AttributeBase<float> numAttr;
                                    numAttr.setValueFromNapi(valueGetter.GetNapiValue());
                                    if (numAttr.has_value()) {
                                        keyFrameItems.params.emplace_back(
                                            type, static_cast<double>(numAttr.value()));
                                    }
                                } break;

                                case CSSProperty::Type::Transform: {
                                    Transform transform;
                                    transform.setValueFromNapi(valueGetter.GetNapiValue());
                                    if (transform.has_value()) {
                                        keyFrameItems.params.emplace_back(
                                            type, std::make_shared<TransformParam>(transform.value()));
                                    }
                                }
                                    //                     default:
                                    //                         keyFrameItems.params.emplace_back(type,
                                    //                         valueGetter.GetNapiValue());
                            }
                        }
                    }
                });

                frames.emplace_back(keyFrameItems);
            });

            if (!frames.empty()) {
                item_.keyframes = std::move(frames);
                this->set(item_);
            }
        };

        void Animation::setFillModeFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);

            if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    item_.fill_mode = getterValue.value();
                    this->set(item_);
                }
            }
        }

        void Animation::setDurationFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_number) {
                auto getterValue = getter.Int32();
                if (getterValue.has_value()) {
                    item_.duration = getterValue.value();
                    this->set(item_);
                }
            }
        };
        void Animation::setTimingFunctionFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    item_.timingFunction = getterValue.value();
                    this->set(item_);
                }
            }
        };
        void Animation::setDelayFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_number) {
                auto getterValue = getter.Int32();
                if (getterValue.has_value()) {
                    item_.delay = getterValue.value();
                    this->set(item_);
                }
            }
        };
        void Animation::setIterationCountFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_number) {
                auto getterValue = getter.UInt32();
                if (getterValue.has_value()) {
                    item_.interationCount = getterValue.value();
                    this->set(item_);
                }
            }
        };

        void Animation::setIntoNode(ArkUI_NodeHandle &node) {
            if (this->has_value()) {
                staticSetValueToNode(node, this->value());
            }
        }

        void Animation::staticSetValueToNode(const ArkUI_NodeHandle &node,
                                             const AnimationOption &value) {
            //     ArkUI_NumberValue arkUI_NumberValue[] = {};
            //     ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
            //     arkUI_NumberValue[0].f32 = value;
            //     TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node,
            //     NODE_ANIMATION, item);
        }

        void Animation::staticResetValueToNode(const ArkUI_NodeHandle &node) {
            //     TaroRuntime::NativeNodeApi::getInstance()->resetAttribute(node,
            //     NODE_ANIMATION);
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
