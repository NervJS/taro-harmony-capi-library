//
// Created on 2024/7/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_CALC_DIMENSION_H
#define HARMONY_LIBRARY_CALC_DIMENSION_H

#include <string>
namespace TaroRuntime {

class ExpressionEvaluator {
    public:
    // Function to evaluate an expression with units
    static double evaluate(const std::string &expression, double base_value);

    // Function to run a test case
    static void run_test_case(const std::string &expression, double expected, double base_value);

    private:
    // Function to apply an operator to two values
    static double apply_operator(double left, double right, char op);

    // Function to determine the precedence of operators
    static int precedence(char op);

    // Conversion functions
    static double parsePercent(const std::string &value, double base_value);
    static double parsePx(const std::string &value);
    static double parseVw(const std::string &value);
    static double parseVh(const std::string &value);

    // Function to replace units with their parsed values
    static std::string replace_units(const std::string &expr, double base_value);

    // Function to preprocess expression by replacing units
    static std::string preprocess_expression(const std::string &expression, double base_value);

    // Function to evaluate expression with parentheses and correct operator precedence
    static double evaluate_expression(const std::string &expr);
};

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_CALC_DIMENSION_H
