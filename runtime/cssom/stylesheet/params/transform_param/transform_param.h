//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_TRANSFORMOPTION_H
#define TARO_CAPI_HARMONY_DEMO_TRANSFORMOPTION_H
#include "./transform_item.h"
#include "helper/Matrix4.h"
namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        class TransformParam {
            public:
            std::vector<std::shared_ptr<TransformItemBase>> data;

            TaroHelper::Matrix4 toMatrix4();

            bool operator==(const TransformParam &other) const;
            bool operator!=(const TransformParam &other) const;

            static std::vector<std::shared_ptr<TransformItemBase>> staticMatrixToTransformItems(const TaroHelper::Matrix4 &m);
            static TaroHelper::Matrix4 staticTransformItemsToMatrix4(const std::vector<std::shared_ptr<TransformItemBase>> &value);
            static TaroHelper::Matrix4 staticTransformItemsToMatrix4(const std::vector<std::shared_ptr<TransformItemBase>> &value, float widthBase, float heightBase);
            static TaroHelper::Matrix4 staticTransformItemsToMatrix4(const TransformParam &value);
            static TaroHelper::Matrix4 staticTransformItemsToMatrix4(const TransformParam &option, float widthBase, float heightBase);
            static std::shared_ptr<TransformItemBase> staticBlendTransformItem(const TransformItemBase &f, const TransformItemBase &t, const float &progress);
            static TransformParam staticBlend(const TransformParam &, const TransformParam &, const float &);
            static float staticBlendFloat(const float form, const float to, const float progress);
            static TAngle staticBlendAngle(const TAngle &form, const TAngle &to, const float progress);
            static Dimension staticBlendTLength(const Dimension form, const Dimension to, const float progress);
            static TransformParam staticTransformItemsPercent(const TransformParam &option, float widthBase, float heightBase);
            static std::shared_ptr<TransformParam> staticTransformSystemValue();
        };
    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
#endif // TARO_CAPI_HARMONY_DEMO_TRANSFORMOPTION_H
