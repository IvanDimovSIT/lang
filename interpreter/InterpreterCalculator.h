#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Interpreter.h"

class InterpreterCalculator{
public:
    static std::unique_ptr<Value> add(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);
    
    static std::unique_ptr<Value> subtract(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> multiply(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> divide(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> mod(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> iterate(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> logicalNot(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> count(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> sumAll(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> multiplyAll(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> power(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> lessThan(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> greaterThan(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> lessThanOrEquals(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);


    static std::unique_ptr<Value> greaterThanOrEquals(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> equals(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> notEquals(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findUnion(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> select(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> makeSet(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> randomize(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findCeil(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findFloor(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> findRound(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> sortArray(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> reverseArray(
        Value& left,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> leftRotate(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> rightRotate(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> remove(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

    static std::unique_ptr<Value> remain(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter);

private:
    using DyadicFunctionLambda = std::function<double(double, double, bool&, IRuntimeErrorReporter*)>;

    static std::unique_ptr<Value> dyadicFunction(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    );

    static bool validateInput(Value& val, IRuntimeErrorReporter* reporter, bool& hadError);

    static void rotateToLeft(const Value& src, Value& dest, long long positions);
};