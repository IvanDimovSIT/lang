#include "OperatorsHelp.h"

const std::map<TokenId, OperatorsInformation> OperatorsHelp::descriptions = {
    {TokenIdAdd,{
        "+ Adds two values",
        {
            "1 + 3 -> 4",
            "1,2,3 + 1 -> 2,3,4",
            "1,2,3 + 1,5 -> 2,7,4"
        }
    }},
    {TokenIdSubtract,{
        "- Subtracts the two values",
        {
            "3 - 1 -> 2",
            "3,3,2 - 1 -> 2,2,1"
        }
    }},
    {TokenIdMultiply,{
        "* Multiplies two values",
        {
            "2 * 4 -> 8",
        }
    }},
    {TokenIdDivide,{
        "/ Divides two values, causes an error if any number in the second value is zero",
        {
            "6 / 3 -> 2",
            "-3 / 2 -> -1.5",
            "1 / 0 -> ERROR"
        }
    }},
    {TokenIdMod,{
        "% Returns the remainder after division",
        {
            "5 % 2 -> 1"
        }
    }},
    {TokenIdPower,{
        "** Raises the first value to the power of the second value",
        {
            "2 ** 3 -> 8",
            "4 ** 0.5 -> 2"
        }
    }},
    {TokenIdSine,{
        "s Calculates the sine of the given value in radians",
        {
            "3.14159 s -> 0"
        }
    }},
    {TokenIdLessThan,{
        "< Checks if the first value is less than the second value, if so returns 1",
        {
            "1 < 5 -> 1",
            "5 < 5 -> 0",
            "6 < 5 -> 0"
        }
    }},
    {TokenIdGreaterThan,{
        "> Checks if the first value is greater than the second value, if so returns 1",
        {
            "1 > 5 -> 0",
            "5 > 5 -> 0",
            "6 > 5 -> 1"
        }
    }},
    {TokenIdLessThanOrEquals,{
        "<= Checks if the first value is less than or equal to the second value, if so returns 1",
        {
            "3 <= 4 -> 1",
            "3 <= 3 -> 1",
            "3 <= 2 -> 0"
        }
    }},
    {TokenIdGreaterThanOrEquals,{
        ">= Checks if the first value is greater than or equal to the second value, if so returns 1",
        {
            "4 >= 3 -> 1",
            "4 >= 4 -> 1",
            "4 >= 5 -> 0"
        }
    }},
    {TokenIdIsEquals,{
        "== Checks if the two values are equal, if so returns 1",
        {
            "2 == 2 -> 1"
        }
    }},
    {TokenIdNotEquals,{
        "!= Checks if the two values are not equal, if so returns 1",
        {
            "2 != 2 -> 0"
        }
    }},
    {TokenIdLogicalNot,{
        "!! Performs logical NOT operation",
        {
            "1 ! -> 0",
            "0 ! -> 1",
            "513,12,0 ! -> 0,0,1"
        }
    }},
    {TokenIdCount,{
        "# Returns the count of numbers in the value to the left",
        {
            "1,2,3,4 -> 4"
        }
    }},
    {TokenIdCountEach,{
        "## Returns the count of numbers in the value to the left that match each number in the right (duplicate values ignored)",
        {
            "3,3,4 ## 2,3,4 -> 0,2,1"
        }
    }},
    {TokenIdSumAll,{
        "#+ Sums all numbers in the value to the left",
        {
            "1,2,3,4 #+ -> 10"
        }
    }},
    {TokenIdMultiplyAll,{
        "#* Multiplies all numbers in the value to the left",
        {
            "1,2,3,4 #* -> 24"
        }
    }},
    {TokenIdMakeSet,{
        "& Returns all unique numbers in the value to the left",
        {
            "1,2,2,3 & -> 1,2,3"
        }
    }},
    {TokenIdUnion,{
        "| Combines two values into one",
        {
            "1,2 | 3,4 = 1,2,3,4"
        }
    }},
    {TokenIdSelect,{
        ": Selects elements from the first values based on the numbers used as indexes in the second",
        {
            "4,5,6 : 2,1 -> 5,4",
            "4,5,6 : 2,2,3,10,1 -> 5,5,6,4",
            "1,2,3 : 4 -> 0"
        }
    }},
    {TokenIdRandom,{
        "? Generates a random number within up to the value to the left",
        {
            "100 ? -> 59.23"
        }
    }},
    {TokenIdCeil,{
        "^ Rounds up to the nearest integer",
        {
            "1.3 ^ -> 2"
        }
    }},
    {TokenIdFloor,{
        "_ Rounds down to the nearest integer",
        {
            "2.7 _ -> 2"
        }
    }},
    {TokenIdRound,{
        "~ Rounds to the nearest integer",
        {
            "2.7 ~ -> 3"
        }
    }},
    {TokenIdSort,{
        "$ Sorts a value in ascending order",
        {
            "2,6,1 $ -> 1,2,6"
        }
    }},
    {TokenIdReverse,{
        "@ Reverses the order of numbers in a value",
        {
            "1,2,3 @ -> 3,2,1"
        }
    }},
    {TokenIdEquals,{
        "= Sets the variable to the left equal to the value from the expression to the right",
        {
            "VAR = 5 -> VAR == 5"
        }
    }},
    {TokenIdFunctionDeclaration,{
        "f Declares a function (use \"a\" and \"b\" as left and right parameters in the function)",
        {
            "f FUNC { a + b } -> 4 FUNC 2 -> 6"
        }
    }},
    {TokenIdIterate,{
        "i Iterates from 1 up to the value to the left",
        {
            "4 i -> 1,2,3,4",
            "3,2 i -> 1,2,3,1,2"
        }
    }},
    {TokenIdIf,{
        "if Conditional statement for branching (non-zero = true, zero = false)",
        {
            "if 2 < 3 { VAR = 3 } -> VAR == 3"
        }
    }},
    {TokenIdLoop,{
        "do Loops through a block of code as long as the condition is true",
        {
            "do I < 10 { I = I + 1 }"
        }
    }},
    {TokenIdRead,{
        "r Reads user input as numbers",
        {
            "VAR = r"
        }
    }},
    {TokenIdWrite,{
        "w Writes output to the console",
        {
            "w 1,2,3 -> 1,2,3"
        }
    }},
    {TokenIdReadText,{
        "g Reads text from the user",
        {
            "VAR = g"
        }
    }},
    {TokenIdWriteText,{
        "t Writes text to the console",
        {
            "t \"Hello World!\""
        }
    }},
    {TokenIdConvert,{
        "c Converts from numbers to a string",
        {
            "1 c -> 49 (\"1\")"
        }
    }},
    {TokenIdApplyToEach,{
        "\\ Applies a function or operator to each number in a value",
        {
            "1,2,3 \\FUNC"
        }
    }},
    {TokenIdLeftRotate,{
        "<< Rotates numbers in the left value to the left based on the value passed from the right parameter",
        {
            "1,2,3 << 1 -> 2,3,1",
            "1,2,3 << 3 -> 1,2,3",
            "1,2,3 << -1 -> 3,2,1"
        }
    }},
    {TokenIdRightRotate,{
        ">> Rotates numbers in the left value to the right based on the value passed from the right parameter",
        {
            "1,2,3 >> 1 -> 3,1,2",
            "1,2,3 >> 3 -> 1,2,3",
            "1,2,3 >> -1 -> 2,3,1"
        }
    }},
    {TokenIdRemove,{
        "-> Removes numbers from the left value that are contained in the right value (if none remain returns 0)",
        {
            "1,2,3 -> 1,3 -> 2",
            "1,2 -> 1,2 -> 0"
        }
    }},
    {TokenIdRemain,{
        "<- Removes numbers from the left value that are not contained in the right value (if none remain returns 0)",
        {
            "1,2,3 <- 1,3 -> 1,3",
            "1,2,3 <- 4 -> 0"
        }
    }},
    {TokenIdAsyncStart,{
        "[ Starts an asynchronous block",
        {
            "[ ..."
        }
    }},
    {TokenIdAsyncEnd,{
        "] Ends an asynchronous block",
        {
            "... ]"
        }
    }},
    {TokenIdAsyncJoin,{
        "!! Waits for all asynchronous operations to complete",
        {
            "[ ... ] !!"
        }
    }},
};
