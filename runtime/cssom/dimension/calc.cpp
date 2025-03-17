//
// Created on 2024/7/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "./calc.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>

#include "runtime/cssom/dimension/context.h"

namespace TaroRuntime {

double ExpressionEvaluator::evaluate(const std::string &expression, double base_value) {
    std::string expr = preprocess_expression(expression, base_value);
    return evaluate_expression(expr);
}

void ExpressionEvaluator::run_test_case(const std::string &expression, double expected, double base_value) {
    size_t pos = expression.find("calc(");
    std::string expr = expression;
    if (pos != std::string::npos) {
        expr = expression.substr(pos + 5, expression.size() - pos - 6);
    }

    try {
        double result = evaluate(expr, base_value);
        if (std::abs(result - expected) < 1e-6) {
            std::cout << "Test passed for expression: " << expression << std::endl;
        } else {
            std::cerr << "Test failed for expression: " << expression
                      << ". Expected: " << expected << ", but got: " << result << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error evaluating expression: " << expression
                  << ". Error: " << e.what() << std::endl;
    }
}

double ExpressionEvaluator::apply_operator(double left, double right, char op) {
    switch (op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            return left / right;
        default:
            throw std::invalid_argument("Invalid operator");
    }
}

int ExpressionEvaluator::precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double ExpressionEvaluator::parsePercent(const std::string &value, double base_value) {
    return base_value * std::stod(value) / 100.0;
}

double ExpressionEvaluator::parsePx(const std::string &value) {
    return std::stod(value) * DimensionContext::GetCurrentContext()->design_ratio_;
}
double ExpressionEvaluator::parseVw(const std::string &value) {
    return std::stod(value) * DimensionContext::GetCurrentContext()->device_width_ / 100;
}
double ExpressionEvaluator::parseVh(const std::string &value) {
    return std::stod(value) * DimensionContext::GetCurrentContext()->device_height_ / 100;
}

std::string ExpressionEvaluator::replace_units(const std::string &expr, double base_value) {
    std::string result;
    size_t i = 0;

    while (i < expr.size()) {
        if (std::isdigit(expr[i]) || expr[i] == '.') {
            std::string number;
            while (i < expr.size() && (std::isdigit(expr[i]) || expr[i] == '.')) {
                number += expr[i++];
            }
            if (i < expr.size() && expr[i] == '%') {
                result += std::to_string(parsePercent(number, base_value));
                ++i;
            } else if (i < expr.size() - 1 && expr[i] == 'p' && expr[i + 1] == 'x') {
                result += std::to_string(parsePx(number));
                i += 2;
            } else if (i < expr.size() - 1 && expr[i] == 'v' && expr[i + 1] == 'w') {
                result += std::to_string(parseVw(number));
                i += 2;
            } else if (i < expr.size() - 1 && expr[i] == 'v' && expr[i + 1] == 'h') {
                result += std::to_string(parseVh(number));
                i += 2;
            } else if (i < expr.size() - 1 && expr[i] == 'v' && expr[i + 1] == 'p') {
                result += number;
                i += 2;
            } else {
                result += number;
            }
        } else {
            result += expr[i++];
        }
    }

    return result;
}

std::string ExpressionEvaluator::preprocess_expression(const std::string &expression, double base_value) {
    size_t pos = expression.find("calc(");
    std::string expr = expression;
    if (pos != std::string::npos) {
        expr = expression.substr(pos + 5, expression.size() - pos - 6);
    }
    return replace_units(expr, base_value);
}

double ExpressionEvaluator::evaluate_expression(const std::string &expr) {
    std::stack<double> values;
    std::stack<char> ops;
    std::istringstream iss(expr);
    char c;

    while (iss >> c) {
        if (std::isdigit(c) || c == '.') {
            iss.putback(c);
            double value;
            iss >> value;
            values.push(value);
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double right = values.top();
                values.pop();
                double left = values.top();
                values.pop();
                char op = ops.top();
                ops.pop();
                values.push(apply_operator(left, right, op));
            }
            ops.pop(); // Remove '(' from ops stack
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
                double right = values.top();
                values.pop();
                double left = values.top();
                values.pop();
                char op = ops.top();
                ops.pop();
                values.push(apply_operator(left, right, op));
            }
            ops.push(c);
        }
    }

    while (!ops.empty()) {
        double right = values.top();
        values.pop();
        double left = values.top();
        values.pop();
        char op = ops.top();
        ops.pop();
        values.push(apply_operator(left, right, op));
    }

    return values.top();
}

} // namespace TaroRuntime
