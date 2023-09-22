#include "InterpreterCalculator.h"
#include "../util/RandomGenerator.h"
#include "../util/StringUtil.h"
#include <cmath>
#include <unordered_set>
#include <algorithm>

bool InterpreterCalculator::validateInput(const Value& input, IRuntimeErrorReporter* reporter, bool& hadError)
{
    if(input.size() == 0){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);
        return false;
    }

    return true;
}

std::unique_ptr<Value> InterpreterCalculator::dyadicFunction(
        const Value& left,
        const Value& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    )
{
    auto result = std::make_unique<Value>();
    const int leftSize = left.size();
    const int rightSize = right.size();
    if((!validateInput(left, reporter, hadError)) || (!validateInput(right, reporter, hadError))){
        hadError = true;
        if(reporter)
            reporter->report(RuntimeErrorTypeEmptyData);
        
        return std::move(result);
    }

    const int maxSize = leftSize > rightSize ? leftSize : rightSize;

    for(int i=0; i<maxSize; i++){
        result->push_back(lambda(left[i%leftSize],right[i%rightSize], hadError, reporter));
    }

    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::add(
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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

std::unique_ptr<Value> InterpreterCalculator::sine(
    const Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    for(const auto& i: left)
        result->push_back(sin(i));
    
    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::convert(
    const Value& left,
    bool& hadError,
    IRuntimeErrorReporter* reporter)
{
    auto result = std::make_unique<Value>();
    if(!validateInput(left, reporter, hadError))
        return std::move(result);

    std::string converted;
    StringUtil::convertValueToString(left, converted);
    for(const auto& i: converted)
        result->push_back((char)i);
    
    return std::move(result);
}

std::unique_ptr<Value> InterpreterCalculator::iterate(
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
    const Value& left,
    const Value& right,
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
