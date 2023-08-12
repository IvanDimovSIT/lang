#include "InterpreterCalculator.h"


std::unique_ptr<std::vector<double>> InterpreterCalculator::dyadicFunction(
        std::vector<double>& left,
        std::vector<double>& right,
        bool& hadError,
        IRuntimeErrorReporter* reporter,
        DyadicFunctionLambda lambda
    )
{
    auto p = std::make_unique<std::vector<double>>();
    int lsize = left.size();
    int rsize = right.size();
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

std::unique_ptr<std::vector<double>> InterpreterCalculator::add(
    std::vector<double>& left,
    std::vector<double>& right,
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


std::unique_ptr<std::vector<double>> InterpreterCalculator::subtract(
    std::vector<double>& left,
    std::vector<double>& right,
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


std::unique_ptr<std::vector<double>> InterpreterCalculator::multiply(
    std::vector<double>& left,
    std::vector<double>& right,
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

std::unique_ptr<std::vector<double>> InterpreterCalculator::divide(
    std::vector<double>& left,
    std::vector<double>& right,
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

