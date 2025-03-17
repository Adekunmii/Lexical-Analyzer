Lexical Analyzer
This project is a Lexical Analyzer implemented in C++. It processes an input file, tokenizes its contents, and categorizes the tokens into identifiers, keywords, numeric constants, and string/character constants. The program also provides options to display specific types of tokens based on command-line arguments.

Features
Tokenization: Breaks down the input file into tokens (identifiers, keywords, constants, etc.).

Case-Insensitive Handling: Identifiers and keywords are treated in a case-insensitive manner.

Command-Line Options: Supports flags to display specific types of tokens:

-all: Display all tokens.

-id: Display identifiers.

-kw: Display keywords.

-num: Display numeric constants.

-str: Display string and character constants.

Error Handling: Detects and reports lexical errors (e.g., invalid identifiers, unterminated strings).

Summary Report: Provides a summary of the total lines, tokens, and counts of each token type.

How It Works
The program reads an input file and processes it character by character using the getNextToken function (defined in lex.h). It categorizes tokens into:

Identifiers: Words that are not keywords (e.g., variable names).

Keywords: Reserved words (e.g., IF, ELSE, INTEGER).

Numeric Constants: Integers and floating-point numbers.

String/Character Constants: Strings enclosed in double quotes (") and characters enclosed in single quotes (').

The program uses sets and maps to store and organize tokens, ensuring efficient lookup and sorting.

Usage
Compile the Code:

bash
Copy
g++ -o lexical_analyzer main.cpp lex.cpp
Run the Program:

bash
Copy
./lexical_analyzer [options] <input_file>
Replace <input_file> with the path to your input file.

Command-Line Options:

-all: Show all tokens.

-id: Show identifiers.

-kw: Show keywords.

-num: Show numeric constants.

-str: Show string and character constants.

Example:

bash
Copy
./lexical_analyzer -id -num input.txt
Output:

The program prints a summary of the tokens found.

If specific flags are provided, it displays the corresponding tokens.

Code Structure
main.cpp:

Contains the main logic for parsing command-line arguments, reading the input file, and categorizing tokens.

Uses sets and maps to store and organize tokens.

Implements case-insensitive comparison for identifiers and keywords.

lex.h:

Defines the LexItem class and token types (e.g., IDENT, ICONST, SCONST).

Declares the getNextToken function for tokenizing the input.

lex.cpp:

Implements the getNextToken function, which processes the input file and extracts tokens.

Dependencies
C++ Standard Library: The program uses standard C++ libraries like <iostream>, <fstream>, <set>, <map>, and <vector>.

Input File: The program requires an input file containing the source code to be analyzed.

Example
Input File (input.txt):
plaintext
Copy
INTEGER x = 10;
STRING name = "Alice";
IF x > 5 THEN
    PRINT(name);
END
Command:
bash
Copy
./lexical_analyzer -all input.txt
Output:
Copy
Lines: 4
Total Tokens: 15
Numerals: 1
Characters and Strings: 1
Identifiers: 2
Keywords: 5

IDENT: <x>
IDENT: <name>
ICONST: (10)
SCONST: "Alice"
IF
THEN
PRINT
END
