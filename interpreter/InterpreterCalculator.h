#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Interpreter.h"

class InterpreterCalculator{
public:
    static std::unique_ptr<std::vector<double>> add(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);
    
    static std::unique_ptr<std::vector<double>> subtract(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> multiply(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> divide(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

private:
    using DyadicFunctionLambda = std::function<double(double, double, bool&, IRuntimeErrorReporter*)>;

    static std::unique_ptr<std::vector<double>> dyadicFunction(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    );

};