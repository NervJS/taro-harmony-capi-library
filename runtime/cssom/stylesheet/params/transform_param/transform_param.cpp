//
// Created on 2024/6/14.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
#include "transform_param.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        TaroHelper::Matrix4 TransformParam::toMatrix4() {
            return staticTransformItemsToMatrix4(data);
        }

        std::shared_ptr<TransformItemBase> TransformParam::staticBlendTransformItem(
            const TransformItemBase &f, const TransformItemBase &t,
            const float &progress) {
            switch (f.type) {
                case ETransformType::ROTATE: {
                    auto from = dynamic_cast<const TransformRotateItem &>(f);
                    auto to = dynamic_cast<const TransformRotateItem &>(t);
                    std::shared_ptr<TransformRotateItem> result =
                        std::make_shared<TransformRotateItem>();
                    result->angle = staticBlendAngle(from.angle, to.angle, progress);
                    result->x = to.x;
                    result->y = to.y;
                    result->z = to.z;
                    if (TaroHelper::Matrix4::NearZero(result->x) &&
                        TaroHelper::Matrix4::NearZero(result->y) &&
                        TaroHelper::Matrix4::NearZero(result->z)) {
                        result->x = from.x;
                        result->y = from.y;
                        result->z = from.z;
                    }

                    return std::move(result);
                }
                case ETransformType::SCALE: {
                    auto from = dynamic_cast<const TransformScaleItem &>(f);
                    auto to = dynamic_cast<const TransformScaleItem &>(t);
                    std::shared_ptr<TransformScaleItem> result =
                        std::make_shared<TransformScaleItem>();

                    result->x = staticBlendFloat(from.x, to.x, progress);
                    result->y = staticBlendFloat(from.y, to.y, progress);
                    result->z = staticBlendFloat(from.z, to.z, progress);
                    return std::move(result);
                }

                case ETransformType::TRANSLATE: {
                    auto from = dynamic_cast<const TransformTranslateItem &>(f);
                    auto to = dynamic_cast<const TransformTranslateItem &>(t);
                    std::shared_ptr<TransformTranslateItem> result =
                        std::make_shared<TransformTranslateItem>();

                    result->x = staticBlendTLength(from.x, to.x, progress);
                    result->y = staticBlendTLength(from.y, to.y, progress);
                    result->z = staticBlendTLength(from.z, to.z, progress);
                    return std::move(result);
                }
                case ETransformType::SKEW: {
                    auto from = dynamic_cast<const TransformSkewItem &>(f);
                    auto to = dynamic_cast<const TransformSkewItem &>(t);
                    std::shared_ptr<TransformSkewItem> result =
                        std::make_shared<TransformSkewItem>();

                    result->x = staticBlendFloat(from.x, to.x, progress);
                    result->y = staticBlendFloat(from.y, to.y, progress);
                    return std::move(result);
                }

                case ETransformType::PERSPECTIVE: {
                    auto from = dynamic_cast<const TransformPerspectiveItem &>(f);
                    auto to = dynamic_cast<const TransformPerspectiveItem &>(t);
                    std::shared_ptr<TransformPerspectiveItem> result =
                        std::make_shared<TransformPerspectiveItem>();

                    result->x = staticBlendFloat(from.x, to.x, progress);
                    result->y = staticBlendFloat(from.y, to.y, progress);
                    result->z = staticBlendFloat(from.z, to.z, progress);
                    result->w = staticBlendFloat(from.w, to.w, progress);
                    return std::move(result);
                }

                case ETransformType::QUATERNION: {
                    auto from = dynamic_cast<const TransformQuaternionItem &>(f);
                    auto to = dynamic_cast<const TransformQuaternionItem &>(t);
                    TransformQuaternionItem item =
                        TransformQuaternionItem::staticSlerp(from, to, progress);
                    return std::move(std::make_shared<TransformQuaternionItem>(item));
                }

                case ETransformType::MATRIX: {
                    auto from = dynamic_cast<const TransformMatrixItem &>(f);
                    auto to = dynamic_cast<const TransformMatrixItem &>(t);
                    std::shared_ptr<TransformMatrixItem> result =
                        std::make_shared<TransformMatrixItem>();
                    std::vector<std::shared_ptr<TransformItemBase>> fromList =
                        staticMatrixToTransformItems(
                            TaroHelper::Matrix4::fromVector(from.matrix));
                    std::vector<std::shared_ptr<TransformItemBase>> toList =
                        staticMatrixToTransformItems(
                            TaroHelper::Matrix4::fromVector(to.matrix));

                    std::vector<std::shared_ptr<TransformItemBase>> blendedList;

                    for (int i = 0; i < fromList.size(); i++) {
                        blendedList.emplace_back(
                            staticBlendTransformItem(*fromList[i], *toList[i], progress));
                    }
                    result->matrix = staticTransformItemsToMatrix4(blendedList).toVector();
                    return std::move(result);
                }
            }
            //     return std::move(result);
        }

        bool TransformParam::operator==(const TransformParam &other) const {
            const std::vector<std::shared_ptr<TransformItemBase>> &current = this->data;
            const std::vector<std::shared_ptr<TransformItemBase>> &otherData = other.data;

            if (current.size() != otherData.size()) return false;

            return std::equal(
                current.begin(), current.end(), otherData.begin(),
                [](std::shared_ptr<TransformItemBase> a,
                   std::shared_ptr<TransformItemBase> b) { return a == b; });
        }

        bool TransformParam::operator!=(const TransformParam &other) const {
            return !(other == *this);
        }

        std::vector<std::shared_ptr<TransformItemBase>>
        TransformParam::staticMatrixToTransformItems(const TaroHelper::Matrix4 &m) {
            TaroHelper::Matrix4 matrix = m;
            std::vector<std::shared_ptr<TransformItemBase>> res;

            if (!matrix.normalize()) return res;

            // 解析透视矩阵

            std::shared_ptr<TransformPerspectiveItem> perspective =
                std::make_shared<TransformPerspectiveItem>();
            TaroHelper::Matrix4 perspectiveMatrix = m;
            for (int i = 0; i < 3; i++) {
                perspectiveMatrix.set(3, i, 0.0);
            }
            perspectiveMatrix.set(3, 3, 1.0);

            // 检查是否存在透视变换
            if (!TaroHelper::Matrix4::NearZero(m.get(3, 0)) ||
                !TaroHelper::Matrix4::NearZero(m.get(3, 1)) ||
                !TaroHelper::Matrix4::NearZero(m.get(3, 2))) {
                float rhs[4] = {m.get(3, 0), m.get(3, 1), m.get(3, 2), m.get(3, 3)};

                TaroHelper::Matrix4 inversePerspectiveMatrix =
                    TaroHelper::Matrix4::invertMatrix(perspectiveMatrix);
                TaroHelper::Matrix4 transposedInversePerspectiveMatrix =
                    inversePerspectiveMatrix;

                transposedInversePerspectiveMatrix.transpose();
                transposedInversePerspectiveMatrix.mapScalars(rhs);

                perspective->x = rhs[0];
                perspective->y = rhs[1];
                perspective->z = rhs[2];
                perspective->w = rhs[3];
            } else {
                perspective->x = 0.0;
                perspective->y = 0.0;
                perspective->z = 0.0;
                perspective->w = 1.0;
            }

            // 解析 translate
            std::shared_ptr<TransformTranslateItem> translate =
                std::make_shared<TransformTranslateItem>();
            std::shared_ptr<TransformScaleItem> scale =
                std::make_shared<TransformScaleItem>();
            std::shared_ptr<TransformSkewItem> skew =
                std::make_shared<TransformSkewItem>();

            // 提取平移变换
            translate->x = matrix.get(0, 3);
            translate->y = matrix.get(1, 3);
            translate->z = matrix.get(2, 3);

            // 用一个3x3的矩阵旋转原四维矩阵的前三行列，顺便用横着写的方式表示矩阵，这个column[i]就是一个向量
            float column[3][3];
            for (int32_t i = 0; i < 3; i++) {
                for (int32_t j = 0; j < 3; j++) {
                    column[i][j] = matrix.get(j, i);
                }
            }

            // 计算第一个向量(x)的向量长度, 那么这个长度就是x向量的缩放量
            scale->x = TaroHelper::Matrix4::staticLength3(column[0]);
            // x 向量归一化
            if (!TaroHelper::Matrix4::NearZero(scale->x)) {
                column[0][0] /= scale->x;
                column[0][1] /= scale->x;
                column[0][2] /= scale->x;
            }
            // 计算x和y两个向量的cos值， 这个值是x的skew,
            skew->x = TaroHelper::Matrix4::staticDot(column[0], column[1]);
            TaroHelper::Matrix4::staticCombine(column[1], column[1], column[0], 1.0,
                                               -skew->x);

            // y向量长度
            scale->y = TaroHelper::Matrix4::staticLength3(column[1]);
            if (!TaroHelper::Matrix4::NearZero(scale->y)) {
                column[1][0] /= scale->y;
                column[1][1] /= scale->y;
                column[1][2] /= scale->y;
                skew->x /= scale->y;
            }

            skew->y = TaroHelper::Matrix4::staticDot(column[0], column[2]);
            TaroHelper::Matrix4::staticCombine(column[2], column[2], column[0], 1.0,
                                               -skew->y);

            skew->z = TaroHelper::Matrix4::staticDot(column[1], column[2]);
            TaroHelper::Matrix4::staticCombine(column[2], column[2], column[1], 1.0,
                                               -skew->z);

            scale->z = TaroHelper::Matrix4::staticLength3(column[2]);
            if (!TaroHelper::Matrix4::NearZero(scale->z)) {
                column[2][0] /= scale->z;
                column[2][1] /= scale->z;
                column[2][2] /= scale->z;
                skew->y /= scale->z;
                skew->z /= scale->z;
            }

            // 检查矩阵的行列式是否为负,如果是,则反转一个缩放因子以使行列式为正
            float pdum3[3];
            TaroHelper::Matrix4::staticCross(pdum3, column[1], column[2]);
            if (TaroHelper::Matrix4::staticDot(column[0], pdum3) < 0) {
                scale->x *= -1.0;
                scale->y *= -1.0;
                scale->z *= -1.0;

                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++) column[i][j] *= -1.0;
            }

            // 提取旋转矩阵的元素
            float q_xx = column[0][0];
            float q_xy = column[1][0];
            float q_xz = column[2][0];
            float q_yx = column[0][1];
            float q_yy = column[1][1];
            float q_yz = column[2][1];
            float q_zx = column[0][2];
            float q_zy = column[1][2];
            float q_zz = column[2][2];

            float r, s, t, x, y, z, w;
            t = q_xx + q_yy + q_zz;
            if (t > 0) {
                r = std::sqrt(1.0 + t);
                s = 0.5 / r;
                w = 0.5 * r;
                x = (q_zy - q_yz) * s;
                y = (q_xz - q_zx) * s;
                z = (q_yx - q_xy) * s;
            } else if (q_xx > q_yy && q_xx > q_zz) {
                r = std::sqrt(1.0 + q_xx - q_yy - q_zz);
                s = 0.5 / r;
                x = 0.5 * r;
                y = (q_xy + q_yx) * s;
                z = (q_xz + q_zx) * s;
                w = (q_zy - q_yz) * s;
            } else if (q_yy > q_zz) {
                r = std::sqrt(1.0 - q_xx + q_yy - q_zz);
                s = 0.5 / r;
                x = (q_xy + q_yx) * s;
                y = 0.5 * r;
                z = (q_yz + q_zy) * s;
                w = (q_xz - q_zx) * s;
            } else {
                r = std::sqrt(1.0 - q_xx - q_yy + q_zz);
                s = 0.5 / r;
                x = (q_xz + q_zx) * s;
                y = (q_yz + q_zy) * s;
                z = 0.5 * r;
                w = (q_yx - q_xy) * s;
            }

            std::shared_ptr<TransformQuaternionItem> quaternion =
                std::make_shared<TransformQuaternionItem>();

            quaternion->x = x;
            quaternion->y = y;
            quaternion->z = z;
            quaternion->w = w;

            res.emplace_back(translate);
            res.emplace_back(scale);
            res.emplace_back(skew);
            res.emplace_back(perspective);
            res.emplace_back(quaternion);

            return std::move(res);
        }

        float TransformParam::staticBlendFloat(const float form, const float to, const float progress) {
            return form + (to - form) * progress;
        }

        TAngle TransformParam::staticBlendAngle(const TAngle &form, const TAngle &to, const float progress) {
            if (form.unit == to.unit) {
                float ret_val = form.value + (to.value - form.value) * progress;
                return TAngle(ret_val, form.unit);
            } else {
                auto from_val = form.getDegValue();
                auto to_val = to.getDegValue();
                float ret_val = from_val + (to_val - from_val) * progress;
                return TAngle(ret_val, PropertyType::AngleUnit::DEGREES);
            }
        }

        Dimension TransformParam::staticBlendTLength(const Dimension form, const Dimension to, const float progress) {
            if (form.Unit() == to.Unit()) {
                Dimension res;
                res.SetUnit(form.Unit());
                res.SetValue(form.Value() + (to.Value() - form.Value()) * progress) ;
                return res;
            } else {
                auto formval = form.ParseToVp().has_value() ? form.ParseToVp().value() : 0;
                auto toval = to.ParseToVp().has_value() ? to.ParseToVp().value() : 0;
                return formval + (toval - formval) * progress;
            }
        }

        TaroHelper::Matrix4 TransformParam::staticTransformItemsToMatrix4(const TransformParam &option) {
            return staticTransformItemsToMatrix4(option.data);
        }
        TaroHelper::Matrix4 TransformParam::staticTransformItemsToMatrix4(const TransformParam &option, float widthBase, float heightBase) {
            return staticTransformItemsToMatrix4(option.data, widthBase, heightBase);
        }

        TaroHelper::Matrix4 TransformParam::staticTransformItemsToMatrix4(const std::vector<std::shared_ptr<TransformItemBase>> &value) {
            TaroHelper::Matrix4 res;
            if (value.size()) {
                for (const auto &item : value) {
                    res = item->toMatrix4() * res;
                }
            }
            return res;
        };
        TaroHelper::Matrix4 TransformParam::staticTransformItemsToMatrix4(const std::vector<std::shared_ptr<TransformItemBase>> &value, float widthBase, float heightBase) {
            TaroHelper::Matrix4 res;
            if (value.size()) {
                for (const auto &item : value) {
                    if (item->type == ETransformType::TRANSLATE) {
                        res = std::dynamic_pointer_cast<TransformTranslateItem>(item)->toMatrix4(widthBase, heightBase) * res;
                    } else {
                        res = item->toMatrix4() * res;
                    }
                }
            }
            return res;
        };
        TransformParam TransformParam::staticBlend(const TransformParam &f, const TransformParam &t, const float &progress) {
            TransformParam res;
            std::vector<std::shared_ptr<TransformItemBase>> returnTransform;
            auto from = f.data;
            auto to = t.data;
            auto fromSize = from.size();
            auto toSize = to.size();

            auto maxSize = std::max(fromSize, toSize);
            returnTransform.reserve(maxSize);

            for (int i = 0; i < maxSize; i++) {
                // 如果他们是同类型，那直接取这个类型的参数取中间值
                if (i < from.size() && i < to.size() && from[i]->type == to[i]->type) {
                    returnTransform.emplace_back(
                        staticBlendTransformItem(*from[i], *to[i], progress));
                } else {
                    if (i >= from.size() && i >= to.size()) break;

                    // 把from剩下的变成一个matrix
                    std::vector<std::shared_ptr<TransformItemBase>> otheFromList;
                    if (i < from.size()) {
                        otheFromList.reserve(from.size() - i);
                        otheFromList.insert(otheFromList.end(), from.begin() + i, from.end());
                    }
                    std::shared_ptr<TransformMatrixItem> otherFrom =
                        std::make_shared<TransformMatrixItem>();
                    otherFrom->matrix =
                        staticTransformItemsToMatrix4(otheFromList).toVector();

                    // 把to剩下的变成一个matrix
                    std::vector<std::shared_ptr<TransformItemBase>> otherToList;
                    if (i < to.size()) {
                        otherToList.reserve(to.size() - i);
                        otherToList.insert(otherToList.end(), to.begin() + i,
                                           to.end()); // 使用 insert
                    }
                    std::shared_ptr<TransformMatrixItem> otherTo =
                        std::make_shared<TransformMatrixItem>();
                    otherTo->matrix = staticTransformItemsToMatrix4(otherToList).toVector();

                    returnTransform.emplace_back(
                        staticBlendTransformItem(*otherFrom, *otherTo, progress));

                    // 如果类型不相同就统一处理，然后break掉
                    break;
                }
            }
            res.data = std::move(returnTransform);
            return std::move(res);
        }

        TransformParam TransformParam::staticTransformItemsPercent(const TransformParam &option, float widthBase, float heightBase) {
            TransformParam res;
            res.data.reserve(option.data.size());
            for (size_t idx = 0; idx < option.data.size(); idx++) {
                auto item = option.data[idx];
                if (option.data[idx]->type == ETransformType::TRANSLATE) {
                    auto translate = std::dynamic_pointer_cast<TransformTranslateItem>(option.data[idx]);
                    item = std::make_shared<TransformTranslateItem>(translate->toVp(widthBase, heightBase));
                }
                res.data.push_back(item);
            }
            return std::move(res);
        }

        std::shared_ptr<TransformParam> TransformParam::staticTransformSystemValue() {
            TaroHelper::Matrix4 matrix4;
            auto defaultTransform = std::make_shared<TaroCSSOM::TaroStylesheet::TransformParam>();
            defaultTransform->data = TaroCSSOM::TaroStylesheet::TransformParam::staticMatrixToTransformItems(matrix4);
            return defaultTransform;
        }
    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime