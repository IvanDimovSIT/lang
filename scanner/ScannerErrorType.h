#pragma once

enum ScannerErrorType{
    ScannerErrorTypeFunctionDefinitionError,
    ScannerErrorTypeFunctionRedefinition,
    ScannerErrorTypeFunctionNameDefinitionError,
    ScannerErrorTypeUnrecognisedToken,
    ScannerErrorTypeMoreClosingParenthesis,
    ScannerErrorTypeMoreOpenParenthesis,
    ScannerErrorTypeMoreClosingCurlyBrackets,
    ScannerErrorTypeMoreOpenCurlyBrackets,
    ScannerErrorTypeStringLiteralError
};
