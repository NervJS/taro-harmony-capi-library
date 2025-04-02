/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_MATRIX4_H
#define TARO_CAPI_HARMONY_DEMO_MATRIX4_H

#include <array>
#include <cmath>
#include <vector>
namespace TaroHelper {

class Matrix4 {
    public:
    std::array<float, 16> data;

    Matrix4();
    Matrix4(std::initializer_list<float> list);
    Matrix4(const Matrix4&);
    static Matrix4 fromValues(float, float, float, float, float, float);

    static Matrix4 fromValues(float, float, float, float, float, float, float,
                              float, float, float, float, float, float, float,
                              float, float);

    static Matrix4 fromVector(const std::vector<float>&);

    std::vector<float> toVector() const;

    // 获取矩阵元素
    float get(int32_t row, int32_t col) const;

    // 设置矩阵元素
    void set(int32_t row, int32_t col, float value);

    bool normalize();

    // 判断值是否接近零
    static bool NearZero(float value) {
        return std::abs(value) < std::numeric_limits<float>::epsilon();
    }

    static float staticLength3(const float v[3]);

    static float staticDot(const float a[3], const float b[3]);

    static void staticCross(float out[3], const float a[3], const float b[3]);
    static void staticCombine(float* out, const float* a, const float* b,
                              float scaleA, float scaleB);

    static Matrix4 translation(float);
    static Matrix4 translation(float, float);
    static Matrix4 translation(float, float, float);
    Matrix4& translate(float);
    Matrix4& translate(float, float);
    Matrix4& translate(float, float, float);

    static Matrix4 translationX(float);
    static Matrix4 translationY(float);
    static Matrix4 translationZ(float);
    Matrix4& translateX(float);
    Matrix4& translateY(float);
    Matrix4& translateZ(float);

    static Matrix4 scaling(float, float);
    static Matrix4 scaling(float, float, float);
    Matrix4& scale(float, float);
    Matrix4& scale(float, float, float);

    static Matrix4 scalingX(float);
    static Matrix4 scalingY(float);
    static Matrix4 scalingZ(float);
    Matrix4& scaleX(float);
    Matrix4& scaleY(float);
    Matrix4& scaleZ(float);

    static Matrix4 rotation(float);
    static Matrix4 rotation(float, float, float, float);
    Matrix4& rotate(float);
    Matrix4& rotate(float, float, float, float);

    static Matrix4 rotationX(float);
    static Matrix4 rotationY(float);
    static Matrix4 rotationZ(float);
    Matrix4& rotateX(float);
    Matrix4& rotateY(float);
    Matrix4& rotateZ(float);

    static Matrix4 skewMatrix(float, float);
    static Matrix4 skewXMatrix(float);
    static Matrix4 skewYMatrix(float);
    Matrix4& skew(float, float);
    Matrix4& skewX(float);
    Matrix4& skewY(float);

    static Matrix4 perspectiveMatrix(float);
    static Matrix4 perspectiveMatrix(float, float, float, float);

    static Matrix4 quaternionMatrix(float x, float y, float z, float w);

    void mapScalars(const float src[4]) const;

    static Matrix4 invertMatrix(const Matrix4&);
    void transpose();

    Matrix4 operator*(const Matrix4&) const;
    Matrix4 operator*=(const Matrix4& other);
    bool operator==(const Matrix4& other) const;
    bool operator!=(const Matrix4& other) const;
};
} // namespace TaroHelper

#endif // TARO_CAPI_HARMONY_DEMO_MATRIX4_H
