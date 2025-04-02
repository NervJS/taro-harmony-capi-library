/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_DIMENSION_H
#define HARMONY_LIBRARY_DIMENSION_H

#include <string>

#include "./utils.h"
#include "helper/Marcos.h"
#include "runtime/cssom/dimension/calc.h"
#include "runtime/cssom/dimension/context.h"

#define NEAR_ZERO(value) ((value > 0.0) ? ((value - 0.0) <= 0.000001f) : ((0.0 - value) <= 0.000001f))

namespace TaroRuntime {

enum class DimensionUnit {
    /*
     * Unit is invalid.
     */
    INVALID = -2,
    /*
     * Unit is empty.
     */
    NONE = -1,
    /*
     * Logical pixel used in Ace1.0. It's based on frontend design width.
     * For example, when a frontend with 750px design width running on a
     * device with 1080 pixels width, 1px represents 1.44 pixels.
     */
    PX = 0,
    /*
     * Density independent pixels, one vp is one pixel on a 160 dpi screen.
     */
    VP,
    /*
     * Scale independent pixels. This is like VP but will be scaled by
     * user's font size preference.
     */
    FP,
    /*
     * The percentage of either a value from the element's parent or from
     * another property of the element itself.
     */
    PERCENT,
    /*
     * logic pixels used in ACE2.0 instead of PX, and PX is the physical pixels in ACE2.0
     */
    LPX,
    /*
     * The value is calculated from the element's parent and another property of the element itself.
     */
    AUTO,
    /*
     * The value is expression.
     */
    CALC,
    /*
     * 相对于视窗宽度.
     */
    VW,
    /*
     * 相对于视窗高度.
     */
    VH,
    /*
     * 设计稿尺寸.
     */
    DESIGN_PX,
    /*
     * 安全区域尺寸.
     */
    SAFE_AREA
};

enum class SAFE_AREA_DIRECTION {
    TOP = 0,
    LEFT,
    BOTTOM,
    RIGHT
};

class CalcExpression {
    public:
    explicit CalcExpression(const std::string& expr)
        : expression_(expr) {}
    const std::string& GetExpression() const {
        return expression_;
    }

    private:
    std::string expression_;
};

class CalcExpressionPool {
    public:
    static CalcExpressionPool& GetInstance() {
        static CalcExpressionPool instance;
        return instance;
    }

    const CalcExpression* GetOrCreate(const std::string& expr) {
        auto it = expressions_.find(expr);
        if (it != expressions_.end()) {
            return it->second.get();
        }
        auto newExpr = std::make_unique<CalcExpression>(expr);
        const CalcExpression* ptr = newExpr.get();
        expressions_[expr] = std::move(newExpr);
        return ptr;
    }

    private:
    CalcExpressionPool() = default;
    std::unordered_map<std::string, std::unique_ptr<CalcExpression>> expressions_;
};

/*
 * Dimension contains a value and an unit to represent different
 * scales in one class.
 */
class Dimension {
    public:
    Dimension()
        : unit_(DimensionUnit::NONE) {};
    virtual ~Dimension() = default;
    constexpr Dimension(double value, DimensionUnit unit = DimensionUnit::VP)
        : value_(value),
          unit_(unit) {}

    Dimension(const std::string& val) {
        auto d = FromString(val);
        value_ = d.value_;
        unit_ = d.unit_;
        calcExpr_ = d.calcExpr_;
    };

    Dimension(const std::string_view& val) {
        auto d = FromString(val);
        value_ = d.value_;
        unit_ = d.unit_;
        calcExpr_ = d.calcExpr_;
    };

    Dimension(const Dimension& other)
        : value_(other.value_),
          unit_(other.unit_),
          calcExpr_(other.calcExpr_) {}

    void Reset() {
        value_ = 0.0;
        unit_ = DimensionUnit::VP;
    }

    void ResetInvalidValue() {
        if (std::isnan(value_)) {
            Reset();
        }
    }

    constexpr double Value() const {
        return value_;
    }

    void SetValue(double value) {
        value_ = value;
    }

    constexpr DimensionUnit Unit() const {
        return unit_;
    }

    void SetUnit(DimensionUnit unit) {
        unit_ = unit;
    }

    bool IsValid() const {
        return unit_ != DimensionUnit::INVALID && unit_ != DimensionUnit::INVALID;
    }

    bool IsNonZero() const {
        return GreatNotEqual(value_, 0.0);
    }

    bool IsNonNegative() const {
        return NonNegative(value_);
    }

    bool IsNonPositive() const {
        return NonPositive(value_);
    }

    bool IsNegative() const {
        return !NonNegative(value_);
    }

    // Percentage unit conversion is not supported.
    double ConvertToVp() const;
    double ConvertToVp(DimensionContextRef context) const;

    // Percentage unit conversion is not supported.
    double ConvertToPx() const;
    double ConvertToPx(DimensionContextRef context) const;

    std::optional<double> ParseToVp() const;
    std::optional<double> ParseToVp(DimensionContextRef context) const;
    std::optional<double> ParseToVp(double base) const;
    std::optional<double> ParseToVp(DimensionContextRef context, double base) const;

    // support percentage unit conversion
    double ConvertToPxWithSize(double size) const;

    bool NormalizeToPx(double vpScale, double fpScale, double lpxScale, double parentLength, double& result) const;

    void SetCalcExpression(const std::string& expr) {
        calcExpr_ = CalcExpressionPool::GetInstance().GetOrCreate(expr);
        unit_ = DimensionUnit::CALC;
        value_ = 0.0; // 或者可以设置为一个特殊值
    }

    const std::string& GetCalcExpression() const {
        static const std::string emptyString;
        return calcExpr_ ? calcExpr_->GetExpression() : emptyString;
    }

    const double GetCalcResult(double base_value) const {
        return ExpressionEvaluator::evaluate(GetCalcExpression(), base_value);
    }

    const bool CheckCalcValid() const;

    constexpr Dimension operator*(double value) const {
        return Dimension(value_ * value, unit_);
    }

    constexpr Dimension operator/(double value) const {
        // NearZero cannot be used in a constant expression
        if (NEAR_ZERO(value)) {
            return {};
        }
        return Dimension(value_ / value, unit_);
    }

    bool operator==(const Dimension& dimension) const {
        if (
            unit_ == DimensionUnit::DESIGN_PX || dimension.unit_ == DimensionUnit::DESIGN_PX ||
            unit_ == DimensionUnit::CALC || dimension.unit_ == DimensionUnit::CALC ||
            unit_ == DimensionUnit::VW || dimension.unit_ == DimensionUnit::VW ||
            unit_ == DimensionUnit::VH || dimension.unit_ == DimensionUnit::VH ||
            unit_ == DimensionUnit::SAFE_AREA || dimension.unit_ == DimensionUnit::SAFE_AREA) {
            return false;
        }
        return (unit_ == dimension.unit_) && NearEqual(value_, dimension.value_);
    }

    bool operator!=(const Dimension& dimension) const {
        return !operator==(dimension);
    }

    /*
     * Add two dimensions using the same unit.
     */
    constexpr Dimension operator+(const Dimension& dimension) const {
        if (NEAR_ZERO(dimension.Value())) {
            return *this;
        }
        TARO_DCHECK(unit_ == dimension.unit_);
        return Dimension(value_ + dimension.value_, unit_);
    }

    /*
     * Add a new dimension to itself using same unit.
     */
    Dimension& operator+=(const Dimension& dimension) {
        TARO_DCHECK(unit_ == dimension.unit_);
        value_ += dimension.value_;
        return *this;
    }

    /*
     * Minus a dimension using the same unit.
     */
    constexpr Dimension operator-(const Dimension& dimension) const {
        if (NEAR_ZERO(dimension.Value())) {
            return *this;
        }
        TARO_DCHECK(unit_ == dimension.unit_);
        return Dimension(value_ - dimension.value_, unit_);
    }

    /*
     * The opposite of dimension.
     */
    constexpr Dimension operator-() const {
        return Dimension(-value_, unit_);
    }

    /*
     * Minus a dimension to itself using the same unit.
     */
    Dimension& operator-=(const Dimension& dimension) {
        TARO_DCHECK(unit_ == dimension.unit_);
        value_ -= dimension.value_;
        return *this;
    }

    bool operator>(const Dimension& dimension) const {
        TARO_DCHECK(unit_ == dimension.unit_);
        return (value_ > dimension.value_);
    }

    bool operator<(const Dimension& dimension) const {
        TARO_DCHECK(unit_ == dimension.unit_);
        return (value_ < dimension.value_);
    }

    std::string ToString() const;

    static Dimension FromString(const std::string& str);
    static Dimension FromString(const std::string_view& str);

    private:
    double value_ = 0.0;
    DimensionUnit unit_ = DimensionUnit::PX;
    const CalcExpression* calcExpr_;
};

// literal operators for dimension
inline constexpr Dimension operator""_vp(long double value) {
    return Dimension(static_cast<double>(value), DimensionUnit::VP);
}

inline constexpr Dimension operator""_px(long double value) {
    return Dimension(static_cast<double>(value), DimensionUnit::PX);
}

inline constexpr Dimension operator""_pct(long double value) {
    return Dimension(static_cast<double>(value), DimensionUnit::PERCENT);
}

float vp2Px(const float& val);
float px2Vp(const float& val);

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_DIMENSION_H
