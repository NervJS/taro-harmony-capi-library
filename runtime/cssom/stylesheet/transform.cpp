//
// Created on 2024/05/28.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./transform.h"

#include <regex>
#include <linux/x25.h>

#include "./params/transform_param//transform_item.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/types/TAngle.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        Transform::Transform(const napi_value &napiValue) {
            setValueFromNapi(napiValue);
        }

        struct TransformFunction {
            std::string_view name;
            std::vector<std::string_view> params;
        };

        void Transform::setValueFromStringView(std::string_view str) {
            std::cmatch matches;
            std::vector<TransformFunction> funcs;
            const char *searchStart;

            searchStart = str.begin();
            while (std::regex_search(searchStart, str.end(), matches, std::regex(R"((matrix(?:3d)?|skew(?:X|Y)?|perspective|(?:rotate|translate|scale)(?:3d|X|Y|Z)?)\((.*?)\))"))) {
                TransformFunction func;
                func.name = std::string_view(matches[1].first, matches[1].length());
                func.params = TaroHelper::string::split(std::string_view(matches[2].first, matches[2].length()), ",");

                if (!func.params.empty()) {
                    funcs.emplace_back(func);
                }
                searchStart = matches[0].second;
            }

            if (funcs.empty()) return;

            std::vector<std::shared_ptr<TransformItemBase>> transformItemBaseList;

            for (auto &func : funcs) {
                if (func.params.empty()) continue;

                // matrix, matrix3d
                if ((func.name == "matrix" && func.params.size() == 6) || (func.name == "matrix3d" && func.params.size() == 16)) {
                    std::vector<float> floatArr(func.params.size());
                    // 字符串转浮点数
                    std::transform(func.params.begin(), func.params.end(), floatArr.begin(), [](auto &sv) {
                        return std::stof(std::string(sv));
                    });
                    std::shared_ptr<TransformMatrixItem> item;
                    item->matrix = Matrix4::fromVector(floatArr).toVector();
                    transformItemBaseList.push_back(std::move(item));
                    continue;
                }

                // translate, translate3d, translateX, translateY, translateZ
                if (func.name.find("translate") != std::string_view::npos) {
                    auto item = std::make_shared<TransformTranslateItem>();
                    if (func.name == "translateY")
                        item->y = Dimension(func.params[0]);
                    else if (func.name == "translateZ")
                        item->z = Dimension(func.params[0]);
                    else {
                        item->x = Dimension(func.params[0]);
                        if (func.params.size() > 1) item->y = Dimension(func.params[1]);
                        if (func.params.size() > 2) item->z = Dimension(func.params[2]);
                    }
                    transformItemBaseList.push_back(std::move(item));
                    continue;
                }

                // scale, scale3d, scaleX, scaleY, scaleZ
                if (func.name.find("scale") != std::string_view::npos) {
                    auto item = std::make_shared<TransformScaleItem>();
                    if (func.name == "scaleY")
                        item->y = std::stof(std::string(func.params[0]));
                    else if (func.name == "scaleZ")
                        item->z = std::stof(std::string(func.params[0]));
                    else {
                        item->x = std::stof(std::string(func.params[0]));
                        if (func.params.size() == 1 && func.name == "scale") item->y = std::stof(std::string(func.params[0]));
                        if (func.params.size() > 1) item->y = std::stof(std::string(func.params[1]));
                        if (func.params.size() > 2) item->z = std::stof(std::string(func.params[2]));
                    }
                    transformItemBaseList.push_back(std::move(item));
                    continue;
                }

                // rotate, rotate3d, rotateX, rotateY, rotateZ
                if (func.name.find("rotate") != std::string_view::npos) {
                    auto item = std::make_shared<TransformRotateItem>();
                    if (func.name == "rotate3d" && func.params.size() > 3) {
                        item->x = std::stof(std::string(func.params[0]));
                        item->y = std::stof(std::string(func.params[1]));
                        item->z = std::stof(std::string(func.params[2]));
                        item->angle = TAngle::MakeFromString(func.params[3]);
                    } else {
                        item->x = 0.0f;
                        item->y = 0.0f;
                        item->z = 0.0f;
                        item->angle = TAngle::MakeFromString(func.params[0]);
                        if (func.name == "rotateX")
                            item->x = 1.0f;
                        else if (func.name == "rotateY")
                            item->y = 1.0f;
                        else if (func.name == "rotateZ" || func.name == "rotate")
                            item->z = 1.0f;
                    }
                    transformItemBaseList.push_back(std::move(item));
                    continue;
                }

                // skew, skewX, skewY
                if (func.name.find("skew") != std::string_view::npos) {
                    auto item = std::make_shared<TransformSkewItem>();
                    if (func.name == "skewY") {
                        auto angleY = TAngle::MakeFromString(func.params[0]);
                        item->y = angleY.getRadValue();
                    } else {
                        auto angleX = TAngle::MakeFromString(func.params[0]);
                        item->x = angleX.getRadValue();
                        if (func.name == "skew" && func.params.size() > 1) {
                            auto angleY = TAngle::MakeFromString(func.params[1]);
                            item->y = angleY.getRadValue();
                        }
                    }
                    transformItemBaseList.push_back(std::move(item));
                    continue;
                }
            }

            TransformParam option;
            option.data = std::move(transformItemBaseList);
            this->set(option);
        }

        std::shared_ptr<TransformItemBase> Transform::parseTransformItem(ETransformType type, const napi_value &elem) {
            switch (type) {
                case ETransformType::MATRIX: {
                    const auto matrixValue =
                        TaroRuntime::NapiGetter::GetPropertyFromNode(elem, "matrix");
                    if (matrixValue != nullptr) {
                        std::vector<float> matrix;
                        TaroRuntime::NapiGetter::ForEachInArray(
                            matrixValue, [&](const napi_value &e, const uint32_t &i) {
                                NapiGetter getter(e);
                                float matrixItem =
                                    getter.Double((i % 5 == 0) ? 1.0f : 0.0f);
                                matrix.emplace_back(matrixItem);
                            });
                        if (matrix.size() == 16) {
                            std::shared_ptr<TransformMatrixItem> matrixItem =
                                std::make_shared<TransformMatrixItem>();
                            matrixItem->matrix = matrix;
                            return matrixItem;
                        }
                    }
                } break;
                case ETransformType::TRANSLATE: {
                    std::shared_ptr<TransformTranslateItem> item =
                        std::make_shared<TransformTranslateItem>();

                    auto xGetter = TaroRuntime::NapiGetter::GetProperty(elem, "x");
                    auto yGetter = TaroRuntime::NapiGetter::GetProperty(elem, "y");
                    auto zGetter = TaroRuntime::NapiGetter::GetProperty(elem, "z");

                    if (xGetter.GetType() == napi_string) {
                        item->x = Dimension::FromString(xGetter.StringOr(""));
                    } else {
                        item->x = Dimension{xGetter.Double(0), DimensionUnit::DESIGN_PX};
                       
                    }

                    if (yGetter.GetType() == napi_string) {
                        item->y = Dimension::FromString(yGetter.StringOr(""));
                    } else {
                        item->y = Dimension{yGetter.Double(0), DimensionUnit::DESIGN_PX};
                    }
                    if (zGetter.GetType() == napi_string) {
                        item->z = Dimension::FromString(zGetter.StringOr(""));
                    } else {
                        item->z = Dimension{zGetter.Double(0), DimensionUnit::DESIGN_PX};
                    }
                    return item;
                } break;
                case ETransformType::SCALE: {
                    std::shared_ptr<TransformScaleItem> item =
                        std::make_shared<TransformScaleItem>();
                    item->x = TaroRuntime::NapiGetter::GetProperty(elem, "x").Double(1);
                    item->y = TaroRuntime::NapiGetter::GetProperty(elem, "y").Double(1);
                    item->z = TaroRuntime::NapiGetter::GetProperty(elem, "z").Double(1);
                    return item;

                } break;
                case ETransformType::ROTATE: {
                    std::shared_ptr<TransformRotateItem> item =
                        std::make_shared<TransformRotateItem>();
                    item->x = TaroRuntime::NapiGetter::GetProperty(elem, "x").Double(0);
                    item->y = TaroRuntime::NapiGetter::GetProperty(elem, "y").Double(0);
                    item->z = TaroRuntime::NapiGetter::GetProperty(elem, "z").Double(0);
                    item->angle.setDegValue(TaroRuntime::NapiGetter::GetProperty(elem, "angle").Double(0));
                    return item;
                } break;
                case ETransformType::SKEW: {
                    std::shared_ptr<TransformSkewItem> item =
                        std::make_shared<TransformSkewItem>();
                    {
                        float xVal = TaroRuntime::NapiGetter::GetProperty(elem, "skewX").Double(0);
                        TAngle xAngle{xVal, PropertyType::AngleUnit::DEGREES};
                        item->x = xAngle.getRadValue();
                    }

                    {
                        float yVal = TaroRuntime::NapiGetter::GetProperty(elem, "skewY").Double(0);
                        TAngle yAngle{yVal, PropertyType::AngleUnit::DEGREES};
                        item->y = yAngle.getRadValue();
                    }

                    {
                        float zVal = TaroRuntime::NapiGetter::GetProperty(elem, "skewZ").Double(0);
                        TAngle zAngle{zVal, PropertyType::AngleUnit::DEGREES};
                        item->z = zAngle.getRadValue();
                    }

                    return item;
                } break;
            }
            return nullptr;
        }

        void Transform::setValueFromNapi(const napi_value &napiValue) {
            napi_valuetype type =
                TaroRuntime::NapiGetter::GetValueTypeFromNode(napiValue);

            if (type != napi_object) return;

            // TODO: 这里可以拿一下 array 的长度，先给 vector 分配一下，避免重复申请内存
            std::vector<std::shared_ptr<TransformItemBase>> transformItemBaseList;

            TaroRuntime::NapiGetter::ForEachInArray(
                napiValue, [&](const napi_value &elem, const uint32_t &index) {
                    const auto typeValue =
                        TaroRuntime::NapiGetter::GetProperty(elem, "type").Int32();

                    if (!typeValue.has_value()) return;

                    ETransformType type = static_cast<ETransformType>(typeValue.value());
                    auto item = parseTransformItem(type, elem);
                    if (item != nullptr) {
                        transformItemBaseList.emplace_back(std::move(item));
                    }
                });

            TransformParam option;
            option.data = std::move(transformItemBaseList);
            this->set(option);
        }

        void Transform::assign(const AttributeBase<TransformParam> &item) {
            if (item.has_value()) this->set(item);
        }

        bool Transform::isEqual(
            const TaroHelper::Optional<TransformParam> &otherVec) const {
            if (this->has_value() != otherVec.has_value()) return false;

            if (!this->has_value()) return true;

            const std::vector<std::shared_ptr<TransformItemBase>> &current =
                this->value().data;
            const std::vector<std::shared_ptr<TransformItemBase>> &other =
                otherVec.value().data;

            if (current.size() != other.size()) return false;

            return std::equal(
                current.begin(), current.end(), other.begin(),
                [](std::shared_ptr<TransformItemBase> a,
                   std::shared_ptr<TransformItemBase> b) { return a == b; });
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
