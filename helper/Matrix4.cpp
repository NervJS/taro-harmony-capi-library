/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./Matrix4.h"
namespace TaroHelper {
Matrix4::Matrix4() {
    for (int i = 0; i < 16; ++i) {
        data[i] = (i % 5 == 0) ? 1.0f : 0.0f; // 创建一个单位矩阵
    }
}
Matrix4::Matrix4(const Matrix4& other) {
    data = other.data;
}
Matrix4::Matrix4(std::initializer_list<float> list) {
    std::copy(list.begin(), list.end(), data.begin());
}
Matrix4 Matrix4::fromValues(float a, float b, float c, float d, float e,
                            float f) {
    return {a, b, 0.0f, 0.0f, c, d, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, e, f, 0.0f, 1.0f};
}

Matrix4 Matrix4::fromValues(float m00, float m01, float m02, float m03,
                            float m10, float m11, float m12, float m13,
                            float m20, float m21, float m22, float m23,
                            float m30, float m31, float m32, float m33) {
    return {m00, m01, m02, m03, m10, m11, m12, m13,
            m20, m21, m22, m23, m30, m31, m32, m33};
}

Matrix4 Matrix4::fromVector(const std::vector<float>& vec) {
    if (vec.size() == 6) {
        return fromValues(vec[0], vec[1], vec[2], vec[3], vec[4], vec[5]);
    } else if (vec.size() == 16) {
        return fromValues(vec[0], vec[1], vec[2], vec[3], vec[4], vec[5], vec[6],
                          vec[7], vec[8], vec[9], vec[10], vec[11], vec[12],
                          vec[13], vec[14], vec[15]);
    } else {
        // 处理无效的向量大小的情况
        // 可以抛出异常或返回一个默认的 Matrix4 对象
        return Matrix4();
    }
}

std::vector<float> Matrix4::toVector() const {
    return std::vector<float>(data.begin(), data.end());
}

Matrix4 Matrix4::translation(float tx) {
    return translationX(tx);
}

Matrix4 Matrix4::translation(float tx, float ty) {
    return translation(tx, ty, 0);
}

Matrix4 Matrix4::translation(float tx, float ty, float tz) {
    Matrix4 result;
    result.data[12] = tx;
    result.data[13] = ty;
    result.data[14] = tz;
    return result;
}

Matrix4& Matrix4::translate(float x) {
    return translate(x, 0, 0);
}

Matrix4& Matrix4::translate(float x, float y) {
    return translate(x, y, 0);
}

Matrix4& Matrix4::translate(float x, float y, float z) {
    *this = *this * translation(x, y, z);
    return *this;
}

Matrix4 Matrix4::translationX(float tx) {
    return translation(tx, 0);
}

Matrix4 Matrix4::translationY(float ty) {
    return translation(0, ty);
}

Matrix4 Matrix4::translationZ(float tz) {
    return translation(0, 0, tz);
}

Matrix4& Matrix4::translateX(float x) {
    return translate(x, 0, 0);
}

Matrix4& Matrix4::translateY(float y) {
    return translate(0, y, 0);
}

Matrix4& Matrix4::translateZ(float z) {
    return translate(0, 0, z);
}

Matrix4 Matrix4::scaling(float sx, float sy) {
    Matrix4 result;
    result.data[0] = sx;
    result.data[5] = sy;
    return result;
}

Matrix4 Matrix4::scaling(float sx, float sy, float sz) {
    Matrix4 result;
    result.data[0] = sx;
    result.data[5] = sy;
    result.data[10] = sz;
    return result;
}

// 获取矩阵元素
float Matrix4::get(int32_t row, int32_t col) const {
    //     return data[row * 4 + col];
    return data[col * 4 + row];
}

// 设置矩阵元素
void Matrix4::set(int32_t row, int32_t col, float value) {
    //     data[row * 4 + col] = value;
    data[col * 4 + row] = value;
}

bool Matrix4::normalize() {
    if (NearZero(this->get(3, 3)))
        return false;
    float scale = 1.0f / this->get(3, 3);

    for (int i = 0; i < 16; i++) {
        data[i] = data[i] * scale;
    }

    return true;
}

Matrix4 Matrix4::scalingX(float sx) {
    return scaling(sx, 1);
}

Matrix4 Matrix4::scalingY(float sy) {
    return scaling(1, sy);
}

Matrix4 Matrix4::scalingZ(float sz) {
    return scaling(1, 1, sz);
}

Matrix4& Matrix4::scale(float x, float y) {
    *this = *this * scaling(x, y);
    return *this;
}

Matrix4& Matrix4::scale(float x, float y, float z) {
    *this = *this * scaling(x, y, z);
    return *this;
}

Matrix4& Matrix4::scaleX(float x) {
    return scale(x, 1, 1);
}

Matrix4& Matrix4::scaleY(float y) {
    return scale(1, y, 1);
}

Matrix4& Matrix4::scaleZ(float z) {
    return scale(1, 1, z);
}

Matrix4 Matrix4::rotation(float angle) {
    return rotationZ(angle);
}

Matrix4 Matrix4::rotation(float x, float y, float z, float angle) {
    Matrix4 result;
    float c = cos(angle);
    float s = sin(angle);
    float t = 1 - c;

    result.data[0] = t * x * x + c;
    result.data[1] = t * x * y + s * z;
    result.data[2] = t * x * z - s * y;

    result.data[4] = t * x * y - s * z;
    result.data[5] = t * y * y + c;
    result.data[6] = t * y * z + s * x;

    result.data[8] = t * x * z + s * y;
    result.data[9] = t * y * z - s * x;
    result.data[10] = t * z * z + c;

    return result;
}

Matrix4 Matrix4::rotationX(float angle) {
    Matrix4 result;
    float c = cos(angle);
    float s = sin(angle);
    result.data[5] = c;
    result.data[6] = s;
    result.data[9] = -s;
    result.data[10] = c;
    return result;
}

float Matrix4::staticLength3(const float v[3]) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Matrix4::staticDot(const float a[3], const float b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
};

void Matrix4::staticCombine(float* out, const float* a, const float* b,
                            float scaleA, float scaleB) {
    for (int i = 0; i < 3; i++) {
        out[i] = a[i] * scaleA + b[i] * scaleB;
    }
}

void Matrix4::staticCross(float out[3], const float a[3], const float b[3]) {
    float x = a[1] * b[2] - a[2] * b[1];
    float y = a[2] * b[0] - a[0] * b[2];
    float z = a[0] * b[1] - a[1] * b[0];
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

Matrix4 Matrix4::rotationY(float angle) {
    Matrix4 result;
    float c = cos(angle);
    float s = sin(angle);
    result.data[0] = c;
    result.data[2] = -s;
    result.data[8] = s;
    result.data[10] = c;
    return result;
}

Matrix4 Matrix4::rotationZ(float angle) {
    Matrix4 result;
    float c = cos(angle);
    float s = sin(angle);
    result.data[0] = c;
    result.data[1] = s;
    result.data[4] = -s;
    result.data[5] = c;
    return result;
}

Matrix4& Matrix4::rotate(float angle) {
    *this = *this * rotation(angle);
    return *this;
}

Matrix4& Matrix4::rotate(float x, float y, float z, float angle) {
    *this = *this * rotation(x, y, z, angle);
    return *this;
}

Matrix4& Matrix4::rotateX(float angle) {
    *this = *this * rotationX(angle);
    return *this;
}

Matrix4& Matrix4::rotateY(float angle) {
    *this = *this * rotationY(angle);
    return *this;
}

Matrix4& Matrix4::rotateZ(float angle) {
    *this = *this * rotationZ(angle);
    return *this;
}

Matrix4 Matrix4::skewMatrix(float angleX, float angleY) {
    Matrix4 result;
    result.data[4] = tan(angleX);
    result.data[1] = tan(angleY);
    return result;
}

Matrix4 Matrix4::skewXMatrix(float angle) {
    Matrix4 result;
    result.data[4] = tan(angle);
    return result;
}
Matrix4& Matrix4::skew(float angleX, float angleY) {
    *this = *this * skewMatrix(angleX, angleY);
    return *this;
}

Matrix4& Matrix4::skewX(float angle) {
    *this = *this * skewXMatrix(angle);
    return *this;
}

Matrix4& Matrix4::skewY(float angle) {
    *this = *this * skewYMatrix(angle);
    return *this;
}
Matrix4 Matrix4::skewYMatrix(float angle) {
    Matrix4 result;
    result.data[1] = tan(angle);
    return result;
}

Matrix4 Matrix4::perspectiveMatrix(float n) {
    Matrix4 result;
    result.data[11] = -1.0f / n;
    return result;
}

Matrix4 Matrix4::perspectiveMatrix(float x, float y, float z, float w) {
    Matrix4 matrix;

    matrix.set(3, 0, x);
    matrix.set(3, 1, y);
    matrix.set(3, 2, z);
    matrix.set(3, 3, w);

    return matrix;
}

Matrix4 Matrix4::quaternionMatrix(float x, float y, float z, float w) {
    Matrix4 matrix;
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    matrix.data[0] = 1.0f - (yy + zz);
    matrix.data[1] = xy + wz;
    matrix.data[2] = xz - wy;
    matrix.data[3] = 0.0f;
    matrix.data[4] = xy - wz;
    matrix.data[5] = 1.0f - (xx + zz);
    matrix.data[6] = yz + wx;
    matrix.data[7] = 0.0f;
    matrix.data[8] = xz + wy;
    matrix.data[9] = yz - wx;
    matrix.data[10] = 1.0f - (xx + yy);
    matrix.data[11] = 0.0f;
    matrix.data[12] = 0.0f;
    matrix.data[13] = 0.0f;
    matrix.data[14] = 0.0f;
    matrix.data[15] = 1.0f;

    return matrix;
}

Matrix4 Matrix4::invertMatrix(const Matrix4& matrix) {
    return fromValues(matrix.get(1, 1) * matrix.get(2, 2) * matrix.get(3, 3) -
                          matrix.get(1, 1) * matrix.get(2, 3) * matrix.get(3, 2) -
                          matrix.get(2, 1) * matrix.get(1, 2) * matrix.get(3, 3) +
                          matrix.get(2, 1) * matrix.get(1, 3) * matrix.get(3, 2) +
                          matrix.get(3, 1) * matrix.get(1, 2) * matrix.get(2, 3) -
                          matrix.get(3, 1) * matrix.get(1, 3) * matrix.get(2, 2),
                      -matrix.get(1, 0) * matrix.get(2, 2) * matrix.get(3, 3) +
                          matrix.get(1, 0) * matrix.get(2, 3) * matrix.get(3, 2) +
                          matrix.get(2, 0) * matrix.get(1, 2) * matrix.get(3, 3) -
                          matrix.get(2, 0) * matrix.get(1, 3) * matrix.get(3, 2) -
                          matrix.get(3, 0) * matrix.get(1, 2) * matrix.get(2, 3) +
                          matrix.get(3, 0) * matrix.get(1, 3) * matrix.get(2, 2),
                      matrix.get(1, 0) * matrix.get(2, 1) * matrix.get(3, 3) -
                          matrix.get(1, 0) * matrix.get(2, 3) * matrix.get(3, 1) -
                          matrix.get(2, 0) * matrix.get(1, 1) * matrix.get(3, 3) +
                          matrix.get(2, 0) * matrix.get(1, 3) * matrix.get(3, 1) +
                          matrix.get(3, 0) * matrix.get(1, 1) * matrix.get(2, 3) -
                          matrix.get(3, 0) * matrix.get(1, 3) * matrix.get(2, 1),
                      -matrix.get(1, 0) * matrix.get(2, 1) * matrix.get(3, 2) +
                          matrix.get(1, 0) * matrix.get(2, 2) * matrix.get(3, 1) +
                          matrix.get(2, 0) * matrix.get(1, 1) * matrix.get(3, 2) -
                          matrix.get(2, 0) * matrix.get(1, 2) * matrix.get(3, 1) -
                          matrix.get(3, 0) * matrix.get(1, 1) * matrix.get(2, 2) +
                          matrix.get(3, 0) * matrix.get(1, 2) * matrix.get(2, 1),
                      -matrix.get(0, 1) * matrix.get(2, 2) * matrix.get(3, 3) +
                          matrix.get(0, 1) * matrix.get(2, 3) * matrix.get(3, 2) +
                          matrix.get(2, 1) * matrix.get(0, 2) * matrix.get(3, 3) -
                          matrix.get(2, 1) * matrix.get(0, 3) * matrix.get(3, 2) -
                          matrix.get(3, 1) * matrix.get(0, 2) * matrix.get(2, 3) +
                          matrix.get(3, 1) * matrix.get(0, 3) * matrix.get(2, 2),
                      matrix.get(0, 0) * matrix.get(2, 2) * matrix.get(3, 3) -
                          matrix.get(0, 0) * matrix.get(2, 3) * matrix.get(3, 2) -
                          matrix.get(2, 0) * matrix.get(0, 2) * matrix.get(3, 3) +
                          matrix.get(2, 0) * matrix.get(0, 3) * matrix.get(3, 2) +
                          matrix.get(3, 0) * matrix.get(0, 2) * matrix.get(2, 3) -
                          matrix.get(3, 0) * matrix.get(0, 3) * matrix.get(2, 2),
                      -matrix.get(0, 0) * matrix.get(2, 1) * matrix.get(3, 3) +
                          matrix.get(0, 0) * matrix.get(2, 3) * matrix.get(3, 1) +
                          matrix.get(2, 0) * matrix.get(0, 1) * matrix.get(3, 3) -
                          matrix.get(2, 0) * matrix.get(0, 3) * matrix.get(3, 1) -
                          matrix.get(3, 0) * matrix.get(0, 1) * matrix.get(2, 3) +
                          matrix.get(3, 0) * matrix.get(0, 3) * matrix.get(2, 1),
                      matrix.get(0, 0) * matrix.get(2, 1) * matrix.get(3, 2) -
                          matrix.get(0, 0) * matrix.get(2, 2) * matrix.get(3, 1) -
                          matrix.get(2, 0) * matrix.get(0, 1) * matrix.get(3, 2) +
                          matrix.get(2, 0) * matrix.get(0, 2) * matrix.get(3, 1) +
                          matrix.get(3, 0) * matrix.get(0, 1) * matrix.get(2, 2) -
                          matrix.get(3, 0) * matrix.get(0, 2) * matrix.get(2, 1),
                      matrix.get(0, 1) * matrix.get(1, 2) * matrix.get(3, 3) -
                          matrix.get(0, 1) * matrix.get(1, 3) * matrix.get(3, 2) -
                          matrix.get(1, 1) * matrix.get(0, 2) * matrix.get(3, 3) +
                          matrix.get(1, 1) * matrix.get(0, 3) * matrix.get(3, 2) +
                          matrix.get(3, 1) * matrix.get(0, 2) * matrix.get(1, 3) -
                          matrix.get(3, 1) * matrix.get(0, 3) * matrix.get(1, 2),
                      -matrix.get(0, 0) * matrix.get(1, 2) * matrix.get(3, 3) +
                          matrix.get(0, 0) * matrix.get(1, 3) * matrix.get(3, 2) +
                          matrix.get(1, 0) * matrix.get(0, 2) * matrix.get(3, 3) -
                          matrix.get(1, 0) * matrix.get(0, 3) * matrix.get(3, 2) -
                          matrix.get(3, 0) * matrix.get(0, 2) * matrix.get(1, 3) +
                          matrix.get(3, 0) * matrix.get(0, 3) * matrix.get(1, 2),
                      matrix.get(0, 0) * matrix.get(1, 1) * matrix.get(3, 3) -
                          matrix.get(0, 0) * matrix.get(1, 3) * matrix.get(3, 1) -
                          matrix.get(1, 0) * matrix.get(0, 1) * matrix.get(3, 3) +
                          matrix.get(1, 0) * matrix.get(0, 3) * matrix.get(3, 1) +
                          matrix.get(3, 0) * matrix.get(0, 1) * matrix.get(1, 3) -
                          matrix.get(3, 0) * matrix.get(0, 3) * matrix.get(1, 1),
                      -matrix.get(0, 0) * matrix.get(1, 1) * matrix.get(3, 2) +
                          matrix.get(0, 0) * matrix.get(1, 2) * matrix.get(3, 1) +
                          matrix.get(1, 0) * matrix.get(0, 1) * matrix.get(3, 2) -
                          matrix.get(1, 0) * matrix.get(0, 2) * matrix.get(3, 1) -
                          matrix.get(3, 0) * matrix.get(0, 1) * matrix.get(1, 2) +
                          matrix.get(3, 0) * matrix.get(0, 2) * matrix.get(1, 1),
                      -matrix.get(0, 1) * matrix.get(1, 2) * matrix.get(2, 3) +
                          matrix.get(0, 1) * matrix.get(1, 3) * matrix.get(2, 2) +
                          matrix.get(1, 1) * matrix.get(0, 2) * matrix.get(2, 3) -
                          matrix.get(1, 1) * matrix.get(0, 3) * matrix.get(2, 2) -
                          matrix.get(2, 1) * matrix.get(0, 2) * matrix.get(1, 3) +
                          matrix.get(2, 1) * matrix.get(0, 3) * matrix.get(1, 2),
                      matrix.get(0, 0) * matrix.get(1, 2) * matrix.get(2, 3) -
                          matrix.get(0, 0) * matrix.get(1, 3) * matrix.get(2, 2) -
                          matrix.get(1, 0) * matrix.get(0, 2) * matrix.get(2, 3) +
                          matrix.get(1, 0) * matrix.get(0, 3) * matrix.get(2, 2) +
                          matrix.get(2, 0) * matrix.get(0, 2) * matrix.get(1, 3) -
                          matrix.get(2, 0) * matrix.get(0, 3) * matrix.get(1, 2),
                      -matrix.get(0, 0) * matrix.get(1, 1) * matrix.get(2, 3) +
                          matrix.get(0, 0) * matrix.get(1, 3) * matrix.get(2, 1) +
                          matrix.get(1, 0) * matrix.get(0, 1) * matrix.get(2, 3) -
                          matrix.get(1, 0) * matrix.get(0, 3) * matrix.get(2, 1) -
                          matrix.get(2, 0) * matrix.get(0, 1) * matrix.get(1, 3) +
                          matrix.get(2, 0) * matrix.get(0, 3) * matrix.get(1, 1),
                      matrix.get(0, 0) * matrix.get(1, 1) * matrix.get(2, 2) -
                          matrix.get(0, 0) * matrix.get(1, 2) * matrix.get(2, 1) -
                          matrix.get(1, 0) * matrix.get(0, 1) * matrix.get(2, 2) +
                          matrix.get(1, 0) * matrix.get(0, 2) * matrix.get(2, 1) +
                          matrix.get(2, 0) * matrix.get(0, 1) * matrix.get(1, 2) -
                          matrix.get(2, 0) * matrix.get(0, 2) * matrix.get(1, 1));
}

void Matrix4::mapScalars(const float src[4]) const {
    for (int32_t i = 0; i < 4; i++) {
        double item = 0.0;
        for (int32_t j = 0; j < 4; j++) {
            item = item + data[i * 4 + j] * src[j];
        }
    }
};

void Matrix4::transpose() {
    std::swap(data[0 * 4 + 1], data[1 * 4 + 0]);
    std::swap(data[0 * 4 + 2], data[2 * 4 + 0]);
    std::swap(data[0 * 4 + 3], data[3 * 4 + 0]);
    std::swap(data[1 * 4 + 2], data[2 * 4 + 1]);
    std::swap(data[1 * 4 + 3], data[3 * 4 + 1]);
    std::swap(data[2 * 4 + 3], data[3 * 4 + 2]);
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
            }
        }
    }
    return result;
}
Matrix4 Matrix4::operator*=(const Matrix4& other) {
    *this = *this * other;
    return *this;
}
bool Matrix4::operator==(const Matrix4& other) const {
    return std::equal(data.begin(), data.end(), other.data.begin());
}
bool Matrix4::operator!=(const Matrix4& other) const {
    return !(*this == other);
}
} // namespace TaroHelper
