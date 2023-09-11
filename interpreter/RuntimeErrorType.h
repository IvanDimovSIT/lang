#pragma once

enum RuntimeErrorType{
    RuntimeErrorTypeEmptyData,
    RuntimeErrorTypeDivisionByZero,
    RuntimeErrorTypeZeroPowerZero,
    RuntimeErrorTypeArithmetic,
    RuntimeErrorTypeOperationAsParameter,
    RuntimeErrorTypeNoClosingParenthesis,
    RuntimeErrorTypeEvaluationError,
    RuntimeErrorTypeParameterError,
    RuntimeErrorTypeNotAnOperation,
    RuntimeErrorTypeMissingIfCondition,
    RuntimeErrorTypeInvalidIfSyntax,
    RuntimeErrorTypeMissingLoopCondition,
    RuntimeErrorTypeNoOperatorToModify,
    RuntimeErrorTypeThreadHadError,
    RuntimeErrorTypeOperatorError
};
