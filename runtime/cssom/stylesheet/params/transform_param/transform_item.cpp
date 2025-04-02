/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./transform_item.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

bool TransformMatrixItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformMatrixItem* otherItem =
            dynamic_cast<const TransformMatrixItem*>(&other)) {
        return this->matrix == otherItem->matrix;
    } else {
        return false;
    }
    return true;
}

bool TransformTranslateItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformTranslateItem* otherItem =
            dynamic_cast<const TransformTranslateItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z;
    } else {
        return false;
    }
    return true;
}

bool TransformScaleItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformScaleItem* otherItem =
            dynamic_cast<const TransformScaleItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z;
    } else {
        return false;
    }
    return true;
}

bool TransformRotateItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformRotateItem* otherItem =
            dynamic_cast<const TransformRotateItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z && this->angle == otherItem->angle;
    } else {
        return false;
    }
    return true;
}

bool TransformSkewItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformSkewItem* otherItem =
            dynamic_cast<const TransformSkewItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z;
    } else {
        return false;
    }
    return true;
}

bool TransformPerspectiveItem::operator==(
    const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformPerspectiveItem* otherItem =
            dynamic_cast<const TransformPerspectiveItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z && this->w == otherItem->w;
    } else {
        return false;
    }
    return true;
}

bool TransformQuaternionItem::operator==(const TransformItemBase& other) const {
    // 如果类型不对，直接返回false
    if (this->type != other.type) {
        return false;
    }

    if (const TransformQuaternionItem* otherItem =
            dynamic_cast<const TransformQuaternionItem*>(&other)) {
        return this->x == otherItem->x && this->y == otherItem->y &&
               this->z == otherItem->z && this->w == otherItem->w;
    } else {
        return false;
    }
    return true;
}

TransformQuaternionItem TransformQuaternionItem::staticSlerp(
    const TransformQuaternionItem& from, const TransformQuaternionItem& to,
    const float progress) {
    TransformQuaternionItem result;
    result.x = from.x;
    result.y = from.y;
    result.z = from.z;
    result.w = from.w;

    if (result.x == to.x && result.y == to.y && result.z == to.z &&
        result.w == to.w)
        return result;

    float cosHalfAngle =
        result.x * to.x + result.y * to.y + result.z * to.z + result.w * to.w;
    if (cosHalfAngle < 0.0) {
        // Since the half angle is > 90 degrees, the full rotation angle would
        // exceed 180 degrees. The quaternions (x, y, z, w) and (-x, -y, -z, -w)
        // represent the same rotation. Flipping the orientation of either
        // quaternion ensures that the half angle is less than 90 and that we are
        // taking the shortest path.
        result.x = -result.x;
        result.y = -result.y;
        result.z = -result.z;
        cosHalfAngle = -cosHalfAngle;
    }

    // Ensure that acos is well behaved at the boundary.
    if (cosHalfAngle > 1.0) {
        cosHalfAngle = 1.0;
    }

    double sinHalfAngle = std::sqrt(1.0 - cosHalfAngle * cosHalfAngle);
    // 边界情况
    if (sinHalfAngle < 1e-5) {
        return result;
        // Quaternions share common axis and angle.
    }

    double half_angle = std::acos(cosHalfAngle);

    double scaleA = std::sin((1.0 - progress) * half_angle) / sinHalfAngle;
    double scaleB = std::sin(progress * half_angle) / sinHalfAngle;

    result.x = scaleA * result.x + scaleB * to.x;
    result.y = scaleA * result.y + scaleB * to.y;
    result.z = scaleA * result.z + scaleB * to.z;
    result.w = scaleA * result.w + scaleB * to.w;
    return result;
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
