#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Interpreter.h"

class InterpreterCalculator{
public:
    static std::unique_ptr<Value> add(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);
    
    static std::unique_ptr<Value> subtract(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> multiply(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> divide(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> mod(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> iterate(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> logicalNot(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> count(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> sumAll(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> multiplyAll(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> power(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> lessThan(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> greaterThan(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> lessThanOrEquals(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);


    static std::unique_ptr<Value> greaterThanOrEquals(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> equals(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> notEquals(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findUnion(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> select(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> makeSet(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> randomize(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findCeil(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findFloor(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findRound(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> sortArray(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> reverseArray(
        const Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> leftRotate(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> rightRotate(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> remove(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> remain(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

private:
    using DyadicFunctionLambda = std::function<double(double, double, bool&, IRuntimeErrorReporter*)>;

    static std::unique_ptr<Value> dyadicFunction(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    );

    static bool validateInput(const Value& input, IRuntimeErrorReporter* reporter, bool& hadError);

    static void rotateToLeft(const Value& src, Value& dest, long long positions);
};