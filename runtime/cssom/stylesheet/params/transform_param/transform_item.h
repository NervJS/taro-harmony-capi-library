//
// Created on 2024/06/11.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_TRANSFORMITEM_H
#define TARO_CAPI_HARMONY_DEMO_TRANSFORMITEM_H

#include <vector>

#include "helper/Matrix4.h"
#include "helper/Optional.h"
#include "runtime/cssom/dimension/dimension.h"
#include "runtime/cssom/stylesheet/types/TAngle.h"

#include "runtime/cssom/stylesheet/utils.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        enum class ETransformType {
            MATRIX,
            TRANSLATE,
            SCALE,
            ROTATE,
            SKEW,
            PERSPECTIVE,
            QUATERNION
        };
        class TransformItemBase {
            public:
            TransformItemBase(ETransformType type)
                : type(type) {}
            ETransformType type;
            virtual TaroHelper::Matrix4 toMatrix4() = 0;
            virtual bool operator==(const TransformItemBase& other) const = 0;
            bool operator!=(const TransformItemBase& other) const {
                return !(*this == other);
            };
        };

        class TransformMatrixItem : public TransformItemBase {
            public:
            TransformMatrixItem()
                : TransformItemBase(ETransformType::MATRIX) {};
            TransformMatrixItem(const TransformMatrixItem& other)
                : TransformItemBase(ETransformType::MATRIX), matrix(other.matrix) {}
            std::vector<float> matrix;

            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::fromVector(matrix);
            }

            bool operator==(const TransformItemBase& other) const override;
        };

        class TransformTranslateItem : public TransformItemBase {
            public:
            TransformTranslateItem()
                : TransformItemBase(ETransformType::TRANSLATE) {
                x = 0;
                y = 0;
                z = 0;
            };

            Dimension x;
            Dimension y;
            Dimension z;


            
            TaroHelper::Matrix4 toMatrix4() override {
                float xf = x.ParseToVp().has_value() ? x.ParseToVp().value() : 0;
                float yf = y.ParseToVp().has_value() ? y.ParseToVp().value() : 0;
                float zf = z.ParseToVp().has_value() ? z.ParseToVp().value() : 0;
                return TaroHelper::Matrix4::translation(xf, yf, zf);
            }
            TaroHelper::Matrix4 toMatrix4(float widthBase, float heightBase) {
                float xf = x.ParseToVp(widthBase).has_value() ? x.ParseToVp(widthBase).value() : 0;
                float yf = y.ParseToVp(heightBase).has_value() ? y.ParseToVp(heightBase).value() : 0;
                float zf = z.ParseToVp(widthBase).has_value() ? z.ParseToVp(widthBase).value() : 0;
                return TaroHelper::Matrix4::translation(vp2Px(xf), vp2Px(yf), vp2Px(zf));
            }
            bool operator==(const TransformItemBase& other) const override;
            TransformTranslateItem toVp(float widthBase, float heightBase) {
                TransformTranslateItem res;
                res = *this;
                res.x = x.ParseToVp(widthBase).value_or(0);
                res.y = y.ParseToVp(heightBase).value_or(0);
                res.z = z.ParseToVp(widthBase).value_or(0);
                return std::move(res);
            }
        };

        class TransformScaleItem : public TransformItemBase {
            public:
            TransformScaleItem()
                : TransformItemBase(ETransformType::SCALE) {
                x = 1;
                y = 1;
                z = 1;
            };
            float x;
            float y;
            float z;

            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::scaling(x, y, z);
            }
            bool operator==(const TransformItemBase& other) const override;
        };

        class TransformRotateItem : public TransformItemBase {
            public:
            TransformRotateItem()
                : TransformItemBase(ETransformType::ROTATE) {
                x = 0;
                y = 0;
                z = 0;
            };
            float x;
            float y;
            float z;

            TAngle angle;
            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::rotation(x, y, z, angle.getRadValue());
            }
            bool operator==(const TransformItemBase& other) const override;
        };

        class TransformSkewItem : public TransformItemBase {
            public:
            TransformSkewItem()
                : TransformItemBase(ETransformType::SKEW) {
                x = 0;
                y = 0;
                z = 0;
            };
            float x;
            float y;
            float z;
            // TODO: 少了一个有z的情况
            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::skewMatrix(x, y);
            }
            bool operator==(const TransformItemBase& other) const override;
        };

        class TransformPerspectiveItem : public TransformItemBase {
            public:
            TransformPerspectiveItem()
                : TransformItemBase(ETransformType::PERSPECTIVE) {
                x = 0;
                y = 0;
                z = 0;
                w = 1;
            }
            float x;
            float y;
            float z;
            float w;

            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::perspectiveMatrix(x, y, z, w);
            }
            bool operator==(const TransformItemBase& other) const override;
        };

        class TransformQuaternionItem : public TransformItemBase {
            public:
            TransformQuaternionItem()
                : TransformItemBase(ETransformType::QUATERNION) {
                x = 1;
                y = 0;
                z = 0;
                w = 0;
            };
            float x;
            float y;
            float z;
            float w;

            TaroHelper::Matrix4 toMatrix4() override {
                return TaroHelper::Matrix4::quaternionMatrix(x, y, z, w);
            }
            static TransformQuaternionItem staticSlerp(
                const TransformQuaternionItem& from, const TransformQuaternionItem& to,
                const float progress);
            bool operator==(const TransformItemBase& other) const override;
        };

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_TRANSFORMITEM_H