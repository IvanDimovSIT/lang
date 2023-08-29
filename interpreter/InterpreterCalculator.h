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

    static std::unique_ptr<std::vector<double>> mod(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> iterate(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> logicalNot(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> count(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> sumAll(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> multiplyAll(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> power(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> lessThan(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> greaterThan(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> lessThanOrEquals(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);


    static std::unique_ptr<std::vector<double>> greaterThanOrEquals(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> equals(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> notEquals(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> findUnion(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> select(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> makeSet(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> randomize(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> findCeil(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> findFloor(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> findRound(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> sortArray(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> reverseArray(
        std::vector<double>& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> leftRotate(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<std::vector<double>> rightRotate(
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

    static bool validateInput(std::vector<double>& val, IRuntimeErrorReporter* reporter, bool& hadError);
    static void rotateToLeft(const std::vector<double>& src, std::vector<double>& dest, long long positions);

};