//
// Created on 2024/5/7.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "CSSStyleSheet.h"

#include <algorithm>
#include <string>
#include <sys/stat.h>

#include "helper/SwitchManager.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/font/CSSFont.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    CSSStyleSheet::CSSStyleSheet() {}

    CSSStyleSheet::~CSSStyleSheet() {}

    void CSSStyleSheet::InitConfig(const napi_value& config) {
        auto designRatio = NapiGetter::GetProperty(config, "designRatio").Double();
        auto dimension_context = DimensionContext::GetInstance();
        if (designRatio.has_value()) {
            dimension_context->design_ratio_ = designRatio.value();
        }
        auto densityPixels =
            NapiGetter::GetProperty(config, "densityPixels").Double();
        if (densityPixels.has_value()) {
            dimension_context->density_pixels_ = densityPixels.value();
        }
        auto deviceWidthPixels = NapiGetter::GetProperty(config, "deviceWidth").Double();
        if (deviceWidthPixels.has_value()) {
            dimension_context->device_width_ = deviceWidthPixels.value();
        }
        auto deviceHeightPixels = NapiGetter::GetProperty(config, "deviceHeight").Double();
        if (deviceHeightPixels.has_value()) {
            dimension_context->device_height_ = deviceHeightPixels.value();
        }
        auto scaledDensity = NapiGetter::GetProperty(config, "scaledDensity").Double();
        if (scaledDensity.has_value()) {
            dimension_context->scaled_density = scaledDensity.value();
        }
        auto orientation = NapiGetter::GetProperty(config, "orientation").UInt32();
        if (orientation.has_value()) {
            dimension_context->orientation = static_cast<Orientation>(orientation.value());
        }
        auto densityDPI = NapiGetter::GetProperty(config, "densityDPI").Double();
        if (densityDPI.has_value()) {
            dimension_context->density_dpi_ = densityDPI.value();
        }
        auto viewportWidth = NapiGetter::GetProperty(config, "viewportWidth").Double();
        if (viewportWidth.has_value()) {
            dimension_context->viewport_width_ = viewportWidth.value();
        }
        auto viewportHeight = NapiGetter::GetProperty(config, "viewportHeight").Double();
        if (viewportHeight.has_value()) {
            dimension_context->viewport_height_ = viewportHeight.value();
        }

        auto safeArea = NapiGetter::GetProperty(config, "safeArea");
        dimension_context->safeArea.bottom = dimension_context->viewport_height_;
        dimension_context->safeArea.right = dimension_context->viewport_width_;
        if (safeArea.Type() == napi_object) {
            dimension_context->safeArea.top = safeArea.GetProperty("top").Double().value_or(0);
            dimension_context->safeArea.left = safeArea.GetProperty("left").Double().value_or(0);
            dimension_context->safeArea.right = safeArea.GetProperty("right").Double().value_or(dimension_context->viewport_width_ * dimension_context->density_pixels_);
            dimension_context->safeArea.bottom = safeArea.GetProperty("bottom").Double().value_or(dimension_context->viewport_height_ * dimension_context->density_pixels_);
        }
    }

    void CSSStyleSheet::UpdateCurrentPageDimension(const std::string& page_path) {
        auto it = current_page_dimension_.find(page_path);
        auto instance = DimensionContext::GetInstance();
        if (it == current_page_dimension_.end()) {
            std::shared_ptr<DimensionContext> instance(new DimensionContext());
            current_page_dimension_[page_path] = instance;
        }
        auto current = current_page_dimension_[page_path];
    
        if (current) {
            current->design_ratio_ = instance->design_ratio_;
            current->density_pixels_ = instance->density_pixels_;
            current->scaled_density = instance->scaled_density;
            current->device_width_ = instance->device_width_;
            current->device_height_ = instance->device_height_;
            current->viewport_width_ = instance->viewport_width_;
            current->viewport_height_ = instance->viewport_height_;
            current->density_dpi_ = instance->density_dpi_;
            current->orientation = instance->orientation;
            current->safeArea = instance->safeArea;
        
            DimensionContext::UpdateCurrentContext(current);
        }
    }

    DimensionContextRef CSSStyleSheet::GetCurrentPageDimension(const std::string& page_path) {
        auto it = current_page_dimension_.find(page_path);

        if (it == current_page_dimension_.end()) {
            return DimensionContext::GetInstance();
        } else {
            return it->second;
        }
    }

    std::string CSSStyleSheet::getStylePagePath(std::string page_path) {
        auto it = current_page_dimension_.find(page_path);
        if (it == current_page_dimension_.end()) {
            DimensionContextRef dimension_context = DimensionContext::GetInstance();

            return page_path.append(std::to_string(std::floor(dimension_context->viewport_width_)));
        } else {
            return page_path.append(std::to_string(std::floor(it->second->viewport_width_)));
        }
    };

    void CSSStyleSheet::Init(const napi_value& router, const napi_value& styleRules, const napi_value& config) {
        // 读取 css 对应的路由
        std::string routerStr = NapiGetter(router).StringOr("");

        // 初始化配置
        InitConfig(config);
        UpdateCurrentPageDimension(routerStr);

        std::string rulesPath = getStylePagePath(routerStr);
        // 在 rulesMapping_ 用 try_emplace 构建一个对象，如果已经有值了，try_emplace 就不会构建
        if (rulesMapping_[rulesPath].size() > 0) return;
        rulesMapping_.try_emplace(rulesPath);

        // medias处理
        auto medias = std::make_shared<CSSMedias>();
        napi_value napi_medias = NapiGetter::GetPropertyFromNode(styleRules, "medias");
        medias->init(napi_medias);

        // 解析keyframes
        auto keyframes = std::make_shared<TaroStylesheet::CSSKeyframes>();
        napi_value napi_keyframes = NapiGetter::GetPropertyFromNode(styleRules, "keyframes");
        keyframes->init(napi_keyframes, medias);
        keyframes_[routerStr] = keyframes;

        // 初始化字体
        auto font = std::make_shared<CSSFont>();
        napi_value napi_fonts = NapiGetter::GetPropertyFromNode(styleRules, "fonts");
        font->init(napi_fonts);

        napi_value styles = NapiGetter::GetPropertyFromNode(styleRules, "styles");
        // 构建CSSOM
        NapiGetter::ForEachInArray(styles, [&](const napi_value& rule,
                                               const uint32_t& index) {
            std::vector<std::string> property_names;
            std::list<Combinator> combinator_list;

            auto media_getter = NapiGetter::GetProperty(rule, "media");
            auto media_id = media_getter.Int32Or(0);
            if (!medias->match(media_id)) {
                TARO_LOG_DEBUG("CSSMedia", "media %{public}d", media_id);
                return;
            }

            auto selector = NapiGetter::GetProperty(rule, "selector");
            // 单选择器
            auto single_selector = selector.String();
            if (single_selector.has_value()) {
                property_names.push_back(single_selector.value());
                combinator_list.push_back(
                    {single_selector.value(), MatchElement::Subject});
                // 插入到classname池
                ClassNamePool::SetClassName(single_selector.value());
            }
            // 嵌套选择器
            auto multiple_selector = selector.Vector();
            if (multiple_selector.has_value()) {
                auto selectors = multiple_selector.value();
                bool is_multiple = true;
                for (int i = 0; i < selectors.size(); i += 2) {
                    auto selector = selectors[i].String();
                    if (selector.has_value()) {
                        MatchElement matchElement =
                            i == 0 ? MatchElement::Subject
                                   : static_cast<MatchElement>(selectors[i - 1].Int32Or(0));
                        combinator_list.push_back({selector.value(), matchElement});

                        // 插入到classname池
                        ClassNamePool::SetClassName(selector.value());

                        // 判断是否多类选择器: .a.b .c {}, 需要将前面2个拆开
                        if (i > 0 && matchElement != MatchElement::Multiple) {
                            is_multiple = false;
                        }
                        if (property_names.size() == 0 || is_multiple) {
                            property_names.push_back(selector.value());
                        }
                    }
                }
            }

            // declaration
            std::unique_ptr<CSSStyleDeclaration> styleDeclaration =
                std::make_unique<CSSStyleDeclaration>();
            styleDeclaration->Init(
                NapiGetter::GetPropertyFromNode(rule, "declarations"));
        
            std::shared_ptr<CSSStyleRule> styleRule = std::make_shared<CSSStyleRule>(
                combinator_list, std::move(styleDeclaration));

            // 伪类
            auto pseudo = NapiGetter::GetProperty(rule, "pseudo").Int32();
            if (pseudo.has_value()) {
                styleRule->setPseudo(static_cast<PseudoType>(pseudo.value()));
                switch (styleRule->getPseudo()) {
                    case PseudoType::NthChild: {
                        auto nth_pseudo =
                            NapiGetter::GetProperty(rule, "pseudo_val").String();
                        if (nth_pseudo.has_value())
                            styleRule->setPseudoValue(nth_pseudo.value());
                        break;
                    }
                    case PseudoType::Before: {
                        break;
                    }
                    case PseudoType::After: {
                        break;
                    }
                    default:
                        break;
                }
            }

            styleRule->init();

            // 如果是多类选择器开头的，则推入多个
            for (auto& selector_string : property_names) {
                rulesMapping_[rulesPath][selector_string].push_back(styleRule);
            }

            bool has_env = NapiGetter::GetProperty(rule, "has_env").BoolOr(false);
            if (has_env) {
                styleRule->has_env = true;
            }
        });
    }

    // 获取 napi_type & napi_value
    void CSSStyleSheet::setAttribute(
        napi_value hmStyle, char* name,
        std::function<void(napi_valuetype, napi_value)> callback) {
        napi_value napiValue = NapiGetter::GetPropertyFromNode(hmStyle, name);
        if (napiValue) {
            napi_valuetype napiType;
            napi_status status;
            status = napi_typeof(NativeNodeApi::env, napiValue, &napiType);
            if (status == napi_ok) {
                callback(napiType, napiValue);
            }
        }
    }

    void CSSStyleSheet::setStylesheet(
        std::shared_ptr<TaroStylesheet::Stylesheet>& stylesheet,
        napi_value hmStyle) {
        if (hmStyle == nullptr) return;

        NapiGetter::ForEachInArray(hmStyle, [&](const napi_value& declarationArr, const uint32_t& _) {
            std::vector<napi_value> declaration =
                NapiGetter::GetVectorFromNode(declarationArr);
            NapiGetter css_property_id(declaration[0]);
            auto css_property_getter = css_property_id.Int32();
            auto css_property_value = declaration[1];
            if (css_property_getter.has_value()) {
                CSSProperty::Type css_property = static_cast<CSSProperty::Type>(css_property_getter.value());

                switch (css_property) {
                    case CSSProperty::Display:
                        stylesheet->display.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FlexDirection:
                        stylesheet->flexDirection.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FlexWrap:
                        stylesheet->flexWrap.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::AlignContent:
                        stylesheet->alignContent.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::JustifyContent:
                        stylesheet->justifyContent.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::AlignItems:
                        stylesheet->alignItems.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::AlignSelf:
                        stylesheet->alignSelf.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FlexBasis:
                        stylesheet->flexBasis.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FlexGrow:
                        stylesheet->flexGrow.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FlexShrink:
                        stylesheet->flexShrink.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Height:
                        stylesheet->height.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Width:
                        stylesheet->width.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MinHeight:
                        stylesheet->minHeight.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MinWidth:
                        stylesheet->minWidth.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MaxHeight:
                        stylesheet->maxHeight.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MaxWidth:
                        stylesheet->maxWidth.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::MarginTop:
                        stylesheet->marginTop.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MarginLeft:
                        stylesheet->marginLeft.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MarginBottom:
                        stylesheet->marginBottom.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::MarginRight:
                        stylesheet->marginRight.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::PaddingTop:
                        stylesheet->paddingTop.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::PaddingLeft:
                        stylesheet->paddingLeft.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::PaddingBottom:
                        stylesheet->paddingBottom.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::PaddingRight:
                        stylesheet->paddingRight.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundColor:
                        stylesheet->backgroundColor.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundImage:
                        stylesheet->backgroundImage.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundRepeat:
                        stylesheet->backgroundRepeat.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderTopLeftRadius:
                        stylesheet->borderTopLeftRadius.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderTopRightRadius:
                        stylesheet->borderTopRightRadius.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderBottomLeftRadius:
                        stylesheet->borderBottomLeftRadius.setValueFromNapi(
                            css_property_value);
                        break;
                    case CSSProperty::BorderBottomRightRadius:
                        stylesheet->borderBottomRightRadius.setValueFromNapi(
                            css_property_value);
                        break;
                    case CSSProperty::Opacity:
                        stylesheet->opacity.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Color:
                        stylesheet->color.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::PointerEvents:
                        stylesheet->pointerEvents.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FontSize:
                        stylesheet->fontSize.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FontStyle:
                        stylesheet->fontStyle.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FontWeight:
                        stylesheet->fontWeight.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::FontFamily:
                        stylesheet->fontFamily.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::LineHeight:
                        stylesheet->lineHeight.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::WhiteSpace:
                        stylesheet->whiteSpace.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::TextDecorationStyle:
                        stylesheet->textDecorationStyle.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::TextDecorationColor:
                        stylesheet->textDecorationColor.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::TextDecorationLine:
                        stylesheet->textDecorationLine.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Visibility:
                        stylesheet->visibility.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Overflow:
                        stylesheet->overflow.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BoxShadow:
                        stylesheet->boxShadow.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::BorderTopWidth:
                        stylesheet->borderTopWidth.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderLeftWidth:
                        stylesheet->borderLeftWidth.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderRightWidth:
                        stylesheet->borderRightWidth.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderBottomWidth:
                        stylesheet->borderBottomWidth.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::BorderTopColor:
                        stylesheet->borderTopColor.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderLeftColor:
                        stylesheet->borderLeftColor.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderRightColor:
                        stylesheet->borderRightColor.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderBottomColor:
                        stylesheet->borderBottomColor.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::BorderTopStyle:
                        stylesheet->borderTopStyle.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderLeftStyle:
                        stylesheet->borderLeftStyle.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderRightStyle:
                        stylesheet->borderRightStyle.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BorderBottomStyle:
                        stylesheet->borderBottomStyle.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::Position:
                        stylesheet->position.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Top:
                        stylesheet->top.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Left:
                        stylesheet->left.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Right:
                        stylesheet->right.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Bottom:
                        stylesheet->bottom.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::ZIndex:
                        stylesheet->zIndex.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundSize:
                        stylesheet->backgroundSize.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundPositionX:
                        stylesheet->backgroundPositionX.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::BackgroundPositionY:
                        stylesheet->backgroundPositionY.setValueFromNapi(css_property_value);
                        break;

                    case CSSProperty::TransformOrigin:
                        stylesheet->transformOrigin.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::Transform:
                        stylesheet->transform.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::TextAlign:
                        stylesheet->textAlign.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::LetterSpacing:
                        stylesheet->letterSpacing.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::WordBreak:
                        stylesheet->wordBreak.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::TextOverflow:
                        stylesheet->textOverflow.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::VerticalAlign:
                        stylesheet->verticalAlign.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::WebkitLineClamp:
                        stylesheet->webkitLineClamp.setValueFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationDelay:
                        stylesheet->animationMulti.setDelayFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationDuration:
                        stylesheet->animationMulti.setDurationFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationFillMode:
                        stylesheet->animationMulti.setFillModeFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationIterationCount:
                        stylesheet->animationMulti.setIterationCountFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationName:
                        stylesheet->animationMulti.setNameFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationKeyFrames:
                        stylesheet->animationMulti.setKeyFramesFromNapi(css_property_value);
                        break;
                    case CSSProperty::AnimationTimingFunction:
                        stylesheet->animationMulti.setTimingFunctionFromNapi(css_property_value);
                        break;
                    case CSSProperty::TransitionProperty:
                        stylesheet->transition.setPropertyFromNapi(css_property_value);
                        break;
                    case CSSProperty::TransitionDuration:
                        stylesheet->transition.setDurationFromNapi(css_property_value);
                        break;
                    case CSSProperty::TransitionDelay:
                        stylesheet->transition.setDelayFromNapi(css_property_value);
                        break;
                    case CSSProperty::TransitionTimingFunction:
                        stylesheet->transition.setTimingFunctionFromNapi(css_property_value);
                        break;
                    case CSSProperty::BoxOrient:
                        stylesheet->boxOrient.setValueFromNapi(css_property_value);
                        break;
                    default:
                        break;
                }
            }
        });
    }

    // 嵌套选择器匹配
    std::vector<CSSStyleRuleRef> CSSStyleSheet::MatchNestingSelector(
        const std::shared_ptr<TaroDOM::TaroElement>& element,
        const TaroDOM::ClassList& classList, const std::string& router) {
        std::vector<CSSStyleRuleRef> element_rules;

        for (std::string className : classList) {
            if (string::trim(className) == "") {
                continue;
            }
            for (const CSSStyleRuleRef rule : rulesMapping_[router][className]) {
                // 匹配规则
                std::shared_ptr<TaroDOM::TaroElement> currentElement = element;
                bool matchFlag = true;

                for (auto combinator : rule->combinator_) {
                    if (!matchFlag) break;
                    switch (combinator.matchElement) {
                        // 普通选择器
                        case MatchElement::Subject: {
                            // .child
                            if (std::find(currentElement->class_list_.begin(),
                                          currentElement->class_list_.end(),
                                          combinator.className) !=
                                currentElement->class_list_.end()) {
                                continue;
                            }
                            matchFlag = false;
                            break;
                        }
                        // 直接后代选择器
                        case MatchElement::Parent: {
                            currentElement = std::static_pointer_cast<TaroDOM::TaroElement>(
                                currentElement->GetParentNode());
                            // .parent > .child
                            if (currentElement) {
                                if (std::find(currentElement->class_list_.begin(),
                                              currentElement->class_list_.end(),
                                              combinator.className) !=
                                    currentElement->class_list_.end()) {
                                    continue;
                                }
                            }
                            matchFlag = false;
                            break;
                        }
                        // 后代选择器
                        case MatchElement::Ancestor: {
                            currentElement = std::static_pointer_cast<TaroDOM::TaroElement>(
                                currentElement->GetParentNode());
                            // .parent .child
                            while (currentElement) {
                                // 匹配选择器
                                if (std::find(currentElement->class_list_.begin(),
                                              currentElement->class_list_.end(),
                                              combinator.className) !=
                                    currentElement->class_list_.end()) {
                                    break;
                                }
                                // 继续父级
                                currentElement = std::static_pointer_cast<TaroDOM::TaroElement>(
                                    currentElement->GetParentNode());
                                if (!currentElement) {
                                    matchFlag = false;
                                }
                            }
                            break;
                        }
                        // 多类选择器
                        case MatchElement::Multiple: {
                            // .childA.childB
                            if (currentElement) {
                                if (std::find(currentElement->class_list_.begin(),
                                              currentElement->class_list_.end(),
                                              combinator.className) !=
                                    currentElement->class_list_.end()) {
                                    continue;
                                }
                            }
                            matchFlag = false;
                            break;
                        }
                    }
                }
                // 匹配成功
                if (matchFlag) {
                    element_rules.push_back(rule);
                }
            }
        }
        return element_rules;
    }

    // 匹配伪类
    bool applyIfNeed(const std::shared_ptr<TaroDOM::TaroElement>& element,
                     const CSSStyleRuleRef& rule) {
        switch (rule->getPseudo()) {
            case PseudoType::None: {
                return true;
            };
            case PseudoType::FirstChild: {
                auto parent_node = element->GetParentNode();
                if (parent_node) {
                    return element == std::static_pointer_cast<TaroDOM::TaroElement>(
                                          parent_node->child_nodes_.front());
                }
            }
            case PseudoType::LastChild: {
                auto parent_node = element->GetParentNode();
                if (parent_node) {
                    return element == std::static_pointer_cast<TaroDOM::TaroElement>(
                                          parent_node->child_nodes_.back());
                }
                break;
            }
            case PseudoType::Empty: {
                return element->child_nodes_.size() == 0;
            }
            case PseudoType::NthChild: {
                // 解析nth-child
                std::array<int, 2> nth_child = parseNthChild(rule->getPseudoValue());
                auto parent_node = element->GetParentNode();
                if (parent_node) {
                    auto& child_nodes = parent_node->child_nodes_;
                    auto it =
                        std::find(child_nodes.begin(), child_nodes.end(),
                                  std::static_pointer_cast<TaroDOM::TaroNode>(element));
                    if (it != child_nodes.end()) {
                        int position = std::distance(child_nodes.begin(), it) + 1;
                        if (nth_child[0] == 0) {
                            return position == nth_child[1];
                        } else {
                            return (position - nth_child[1]) % nth_child[0] == 0;
                        }
                    }
                }
                break;
            }
            case PseudoType::Before:
            case PseudoType::After: {
                return false;
            }
            default:
                break;
        }
        return false;
    }

    // 层叠样式合成
    ElementStylesheets CSSStyleSheet::CombineCascadeStyle(
        const std::shared_ptr<TaroDOM::TaroElement>& element,
        std::vector<CSSStyleRuleRef>& rules) {
        // 层叠样式表合并
        if (!rules.empty()) {
            if (rules.size() == 1) {
                // 如果只有一层，则无需合并，直接指向该指针
                auto first_rule = *rules.begin();
                element->SetMatchEmptySelector(first_rule->getPseudo() == PseudoType::Empty);
                if (applyIfNeed(element, first_rule)) {
                    return ElementStylesheets{stylesheet : first_rule->declaration_->styles_ptr_};
                } else {
                    const PseudoType& pseudoType = first_rule->getPseudo();
                    return ElementStylesheets{
                        stylesheet : std::make_shared<TaroStylesheet::Stylesheet>(),
                        before_stylesheet : pseudoType == PseudoType::Before
                            ? first_rule->declaration_->styles_ptr_
                            : nullptr,
                        after_stylesheet : pseudoType == PseudoType::After
                            ? first_rule->declaration_->styles_ptr_
                            : nullptr,
                    };
                }
            } else {
                // 如果有多个层级，需要进行合并操作
                auto style = std::make_shared<TaroStylesheet::Stylesheet>();
                std::shared_ptr<TaroStylesheet::Stylesheet> before_stylesheet = nullptr;
                std::shared_ptr<TaroStylesheet::Stylesheet> after_stylesheet = nullptr;

                std::sort(rules.begin(), rules.end(),
                          [](const std::shared_ptr<CSSStyleRule>& a, const std::shared_ptr<CSSStyleRule>& b) {
                              return a->combinator_.size() < b->combinator_.size();
                          });
                element->SetMatchEmptySelector(false);
                for (std::shared_ptr<CSSStyleRule> rule : rules) {
                    if (applyIfNeed(element, rule)) {
                        style = TaroStylesheet::Stylesheet::assign(style, rule->declaration_->styles_ptr_);
                    } else {
                        const PseudoType& pseudoType = rule->getPseudo();
                        if (pseudoType == PseudoType::Before) {
                            if (!before_stylesheet) {
                                before_stylesheet = std::make_shared<TaroStylesheet::Stylesheet>();
                            }
                            before_stylesheet = TaroStylesheet::Stylesheet::assign(before_stylesheet, rule->declaration_->styles_ptr_);
                        } else if (pseudoType == PseudoType::After) {
                            if (!after_stylesheet) {
                                after_stylesheet = std::make_shared<TaroStylesheet::Stylesheet>();
                            }
                            after_stylesheet = TaroStylesheet::Stylesheet::assign(after_stylesheet, rule->declaration_->styles_ptr_);
                        }
                    }
                    element->SetMatchEmptySelector(element->GetMatchEmptySelector() || rule->getPseudo() == PseudoType::Empty);
                }
                return ElementStylesheets{
                    stylesheet : style,
                    before_stylesheet : before_stylesheet,
                    after_stylesheet : after_stylesheet
                };
            }
        }
        return ElementStylesheets{
            stylesheet : std::make_shared<TaroStylesheet::Stylesheet>(), // 建立一个空的style
            before_stylesheet : nullptr,
            after_stylesheet : nullptr
        };
    }

    ElementStylesheets CSSStyleSheet::getStylesheetByClassNames(
        const std::shared_ptr<TaroDOM::TaroElement>& element,
        const TaroDOM::ClassList& classList, const std::string& router) {
        element->rule_list_ = MatchNestingSelector(element, classList, router);
        return CombineCascadeStyle(element, element->rule_list_);
    }

    const TaroStylesheet::KeyframesInfoPtr CSSStyleSheet::getAnimKeyframes(const std::string& name, const std::string& router) const {
        const auto iter = keyframes_.find(router);
        if (iter == keyframes_.end() || iter->second == nullptr) {
            return nullptr;
        }

        return iter->second->getAnimKeyframes(name);
    }

    TaroStylesheet::CSSKeyframesPtr CSSStyleSheet::getAnimKeyframesPtr(const std::string& router) {
        const auto& keyframes = GetInstance()->keyframes_;
        const auto iter = keyframes.find(router);
        if (iter == keyframes.end()) {
            return nullptr;
        }
        return iter->second;
    }

} // namespace TaroCSSOM
} // namespace TaroRuntime
