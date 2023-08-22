#pragma once

enum TokenId{
    TokenIdLiteral,             // 12,3213,0.123,-213,0
    TokenIdVariable,            // MYVAR
    TokenIdFunction,            // MYFUNC
    TokenIdAdd,                 // +
    TokenIdSubtract,            // -
    TokenIdMultiply,            // *
    TokenIdDivide,              // /
    TokenIdMod,                 // %
    TokenIdPower,               // ^
    TokenIdLessThan,            // <
    TokenIdGreaterThan,         // >
    TokenIdLessThanOrEquals,    // <=
    TokenIdGreaterThanOrEquals, // >=
    TokenIdIsEquals,            // ==
    TokenIdNotEquals,           // !=
    TokenIdLogicalNot,          // !
    TokenIdCount,               // #
    TokenIdEquals,              // =
    TokenIdOpenCurly,           // {
    TokenIdCloseCurly,          // }
    TokenIdOpenParenthesis,     // (
    TokenIdCloseParenthesis,    // )
    TokenIdFunctionDeclaration, // f
    TokenIdIterate,             // i
    TokenIdLeftParam,           // a
    TokenIdRightParam,          // b
    TokenIdEndLine,             // '\n'
    TokenIdIf,                  // 'if'
    TokenIdLoop,                // 'do'
    TokenIdRead,                // 'r'
    TokenIdWrite                // 'w'
};
