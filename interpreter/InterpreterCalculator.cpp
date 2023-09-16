#include "InterpreterCalculator.h"
#include "../util/RandomGenerator.h"
#include <cmath>
#include <unordered_set>
#include <algorithm>

bool InterpreterCalculator::validateInput(Value& val, IRuntimeErrorReporter* reporter, bool& hadError)
{
    if(val.size() == 0){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);
        return false;
    }

    return true;
}

std::unique_ptr<Value> InterpreterCalculator::dyadicFunction(
        Value& left,
        Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    )
{
    auto p = std::make_unique<Value>();
    const int lsize = left.size();
    const int rsize = right.size();
    if(lsize == 0 || rsize == 0){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);
    }
    int max = lsize > rsize ? lsize : rsize;

    for(int i=0; i<max; i++){
        p->push_back(lambda(left[i%lsize],right[i%rsize], hadError, reporter));
    }

    return std::move(p);
}

std::unique_ptr<Value> InterpreterCalculator::add(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a + b;
        })
    );
}


std::unique_ptr<Value> InterpreterCalculator::subtract(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a - b;
        })
    );
}


std::unique_ptr<Value> InterpreterCalculator::multiply(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a * b;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::divide(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            if(b == 0){
                if(r)
                    r->report(RuntimeErrorTypeDivisionByZero);
                err = true;
                return 0.0;
            }
            return a / b;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::mod(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            const long long llA = a;
            const long long llB = b;
            if(llB == 0){
                if(r)
                    r->report(RuntimeErrorTypeDivisionByZero);
                err = true;
                return 0.0;
            }
            return (double)(llA % llB);
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::lessThan(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a < b? 1.0 : 0.0;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::greaterThan(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a > b? 1.0 : 0.0;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::lessThanOrEquals(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a <= b? 1.0 : 0.0;
        })
    );
}


std::unique_ptr<Value> InterpreterCalculator::greaterThanOrEquals(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a >= b? 1.0 : 0.0;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::equals(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a == b? 1.0 : 0.0;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::notEquals(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            return a != b? 1.0 : 0.0;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::power(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    return std::move(
        dyadicFunction(left, right, hadError, reporter,
            [](double a, double b, bool& err, IRuntimeErrorReporter* r) -> double 
        {
            if(a == 0.0 && b == 0.0){
                if(r)
                    r->report(RuntimeErrorTypeZeroPowerZero);
                err = true;
                return 0.0;
            }
            double res = pow(a, b);
            if(std::isnan(res)){
                if(r)
                    r->report(RuntimeErrorTypeArithmetic);
                err = true;
                return 0.0;
            }
            return res;
        })
    );
}

std::unique_ptr<Value> InterpreterCalculator::iterate(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left){
        int val = floor(i);
        if(val <= 0)
            continue;

        for(int j = 1; j <= val; j++){
            result->push_back((double)j);
        }
    }
    if(result->size() == 0)
        result->push_back(0.0);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::logicalNot(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);
    
    for(const auto& i:left)
        result->push_back(i == 0.0 ? 1.0 : 0.0);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::count(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    const int size = left.size();
    if(size == 0){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);

        return result;
    }
    result->push_back((double)size);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::sumAll(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);
    
    double sum = 0.0;
    for(const auto& i: left)
        sum += i;

    result->push_back(sum);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::multiplyAll(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);
    
    double prod = 1.0;
    for(const auto& i: left)
        prod *= i;

    result->push_back(prod);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::findUnion(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if((!validateInput(left, reporter, hadError)) || (!validateInput(right, reporter, hadError)))
        return std::move(result);

    *result = left;
    for(const auto& i: right)
        result->push_back(i);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::select(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    const int leftSize = left.size();
    if(leftSize == 0 || right.size() == 0){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);
        return std::move(result);
    }

    for(const auto& i: right){
        int index = floor(i)-1;
        if(index>=0 && index<leftSize)
            result->push_back(left[index]);
    }
    if(result->size() == 0)
        result->push_back(0.0);

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::randomize(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left)
        result->push_back(i*RandomGenerator::generateRandom());

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::findCeil(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left)
        result->push_back(ceil(i));

    return std::move(result);
}

    std::unique_ptr<Value> InterpreterCalculator::findFloor(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left)
        result->push_back(floor(i));

    return std::move(result);
}

    std::unique_ptr<Value> InterpreterCalculator::findRound(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left)
        result->push_back(round(i));

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::sortArray(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    *result = left;
    std::sort(result->begin(), result->end());

    return std::move(result);
}


std::unique_ptr<Value> InterpreterCalculator::reverseArray(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    *result = left;
    std::reverse(result->begin(), result->end());

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::makeSet(
    Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    std::unordered_set<double> seen;
    for(const auto& i: left){
        if(seen.find(i) == seen.end()){
            seen.insert(i);
            result->push_back(i);
        }
    }

    return std::move(result);
}

void InterpreterCalculator::rotateToLeft(const Value& src, Value& dest, long long positions)
{
    const int size = src.size();
    if(positions < 0){
        positions = -positions;
        positions %= size;
        positions = size - positions;
    }
    else{
        positions %= size;
    }

    for(int i=0; i<size; i++){
        dest.push_back(src[(i+positions)%size]);
    }

}

std::unique_ptr<Value> InterpreterCalculator::leftRotate(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError) || !validateInput(right, reporter, hadError))
        return std::move(result);

    rotateToLeft(left, *result, (long long)right[0]);
    
    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::rightRotate(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError) || !validateInput(right, reporter, hadError))
        return std::move(result);

    rotateToLeft(left, *result, -((long long)right[0]));
    
    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::remove(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError) || !validateInput(right, reporter, hadError))
        return std::move(result);

    std::unordered_set<double> toRemove;
    for(const auto& i: right)
        toRemove.insert(i);

    for(const auto& i: left){
        if(toRemove.find(i) == toRemove.end())
            result->push_back(i);
    }

    if(result->size() <= 0)
        result->push_back(0.0);

    return std::move(result);
}

 std::unique_ptr<Value> InterpreterCalculator::remain(
    Value& left,
    Value& right,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError) || !validateInput(right, reporter, hadError))
        return std::move(result);

    std::unordered_set<double> toRemain;
    for(const auto& i: right)
        toRemain.insert(i);

    for(const auto& i: left){
        if(toRemain.find(i) != toRemain.end())
            result->push_back(i);
    }

    if(result->size() <= 0)
        result->push_back(0.0);

    return std::move(result);
}
