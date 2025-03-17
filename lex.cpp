#include "lex.h"
#include <cctype>
#include <cstdlib>
#include <limits>

using namespace std;

// Keyword map for quick lookup. Maps string keywords to their corresponding Token values.
map<string, Token> keywords = {
    {"GET", GET}, {"INTEGER", INT}, {"FLOAT", FLOAT}, {"CHARACTER", CHAR},
    {"STRING", STRING}, {"BOOLEAN", BOOL}, {"PROCEDURE", PROCEDURE},
    {"IF", IF}, {"ELSE", ELSE}, {"ELSIF", ELSIF}, {"PUT", PUT}, {"PUTLN", PUTLN},    
    {"THEN", THEN}, {"CONST", CONST}, {"AND", AND}, {"OR", OR}, {"NOT", NOT}, {"MOD", MOD}, {"PUTLINE", PUTLN},   
    {"TRUE", TRUE}, {"FALSE", FALSE}, {"END", END}, {"IS", IS}, {"BEGIN", BEGIN},
};

// Function to get the next token from the input stream.
LexItem getNextToken(istream& in, int& linenum) {
    string lexeme;
    char ch;

    // Read characters from the input stream one by one.
    while (in.get(ch)) {
        
        // Handle single-line comments (starting with '--')
        if (ch == '-' && in.peek() == '-') {
            in.get(); // Consume the second '-'
            linenum++; // Increment line number
            in.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the rest of the line
            continue;
        } 
   
        // Handle newlines and increment the line number.
        if (ch == '\n') {
            linenum++;
            continue;
        }

        // Skip whitespace characters.
        if (isspace(ch)) {
            continue;
        }
     
        // Handle identifiers and keywords.
        if (isalpha(ch) || ch == '_') {
            bool prevUnderscore = (ch == '_');
            lexeme = ch;
            
            // Continue reading while the next character is alphanumeric or an underscore.
            while (in.get(ch) && (isalnum(ch) || ch == '_')) {
                if (prevUnderscore && ch == '_') {
                    in.putback(ch); // Prevent consecutive underscores
                    break;
                }
                
                lexeme += ch;
                prevUnderscore = (ch == '_');
            }
            
            in.putback(ch); // Put back the last character that is not part of the identifier.
            
            // Identifiers cannot start with an underscore.
            if (lexeme[0] == '_') {
                return LexItem(ERR, lexeme, linenum);
            }
            
            // Determine if the lexeme is a keyword or an identifier.
            return id_or_kw(lexeme, linenum);
        }

        // Handle integer and floating-point constants.
        if (isdigit(ch)) {
            lexeme = ch;
            bool hasDot = false, hasExponent = false;

            // Continue reading while the next character is part of a number.
            while (in.get(ch)) {
                if (ch == '.' && in.peek() == '.') {
                    in.putback(ch); // Handle double dot case (e.g., '..')
                    break;
                }
                
                if (ch == '.' && hasDot) {
                    return LexItem(ERR, lexeme + ".", linenum); // Error if multiple dots are found.
                }
                else if (ch == '.' && !hasDot && !hasExponent) {
                    hasDot = true; // Mark that a dot has been found.
                    lexeme += ch;
                }
                else if ((ch == 'E' || ch == 'e') && !hasExponent) {
                    char nextCh = in.peek();
                    bool validExponent = isdigit(nextCh);
                    
                    // Handle exponent sign (+ or -).
                    if (!validExponent && (nextCh == '+' || nextCh == '-')) {
                        in.get();
                        char digitAfterSign = in.peek();
                        in.putback(nextCh);
                        
                        validExponent = isdigit(digitAfterSign);
                    }
                    
                    if (!validExponent) {
                        in.putback(ch); // Invalid exponent format.
                        break;
                    }
                    
                    hasExponent = true;
                    lexeme += ch;
                    
                    if (in.peek() == '+' || in.peek() == '-') {
                        in.get(ch);
                        lexeme += ch;
                    }
                }
                else if (isdigit(ch)) {
                    lexeme += ch;
                }
                else {
                    in.putback(ch); // Put back the last character that is not part of the number.
                    break;
                }
            }
            
            // Return the appropriate token based on whether the number has a dot (floating-point) or not (integer).
            if (hasDot) {
                return LexItem(FCONST, lexeme, linenum);
            } else {
                return LexItem(ICONST, lexeme, linenum);
            }
        }

        // Handle string constants (enclosed in double quotes).
        if (ch == '"') {
            lexeme = "";
            bool unterminated = true;
            
            // Read characters until the closing double quote is found.
            while (in.get(ch)) {
                if (ch == '"') {
                    unterminated = false;
                    return LexItem(SCONST, lexeme, linenum);
                }
                if (ch == '\n') {
                    return LexItem(ERR, " Invalid string constant \"" + lexeme, linenum); // Error if newline is encountered before closing quote.
                }
                lexeme += ch;
            }
            
            // Error if the string is unterminated.
            if (unterminated) {
                return LexItem(ERR, " Invalid string constant \"" + lexeme, linenum);
            }
            
            return LexItem(ERR, lexeme, linenum);
        }

        // Handle character constants (enclosed in single quotes).
        if (ch == '\'') {
            lexeme = "";
            string errorContent = "";
            
            if (in.get(ch)) {
                if (ch == '\n') {
                    return LexItem(ERR, "New line is an invalid character constant.", linenum); // Error if newline is encountered.
                } else if (ch == '\'') {
                    return LexItem(ERR, "Empty character constant.", linenum); // Error if the character constant is empty.
                } else {
                    lexeme += ch;
                    errorContent += ch;
                    
                    char nextCh;
                    while (in.get(nextCh) && nextCh != '\'' && nextCh != '\n') {
                        errorContent += nextCh;
                        if (errorContent.length() < 2) {
                            errorContent += nextCh;
                        }
                    }
                    
                    if (nextCh == '\'' && errorContent.length() == 1) {
                        return LexItem(CCONST, lexeme, linenum); // Valid character constant.
                    } else if (nextCh == '\n') {
                        return LexItem(ERR, "Unterminated character constant.", linenum); // Error if newline is encountered before closing quote.
                    } else {
                        if (errorContent.length() > 2) {
                            errorContent = errorContent.substr(0, 2);
                        }
                        return LexItem(ERR, " Invalid character constant '" + errorContent + "'", linenum); // Error if the character constant is invalid.
                    }
                }
            } else {
                return LexItem(ERR, "Unterminated character constant.", linenum); // Error if the character constant is unterminated.
            }
        }

        // Handle operators and special characters.
        switch (ch) {
            case '-': return LexItem(MINUS, "-", linenum);
            case '+': return LexItem(PLUS, "+", linenum);
            case '*': 
                if (in.peek() == '*') {
                    in.get();
                    return LexItem(EXP, "**", linenum); // Handle exponentiation operator.
                }
                return LexItem(MULT, "*", linenum);
            case '|': 
                if (in.peek() == '|') {
                    in.get();
                    return LexItem(OR, "||", linenum); // Handle logical OR operator.
                }
                return LexItem(ERR, "|", linenum);    
            case '/': 
                if (in.peek() == '=') {
                    in.get();
                    return LexItem(NEQ, "/=", linenum); // Handle not equal operator.
                }
                return LexItem(DIV, "/", linenum);
            case '=': return LexItem(EQ, "=", linenum);
            case '!': 
                if (in.peek() == '=') {
                    in.get();
                    return LexItem(NEQ, "!=", linenum); // Handle not equal operator.
                }
                return LexItem(ERR, "!", linenum);
            case '>': 
                if (in.peek() == '=') {
                    in.get();
                    return LexItem(GTE, ">=", linenum); // Handle greater than or equal operator.
                }
                return LexItem(GTHAN, ">", linenum);
            case '<': 
                if (in.peek() == '=') {
                    in.get();
                    return LexItem(LTE, "<=", linenum); // Handle less than or equal operator.
                }
                return LexItem(LTHAN, "<", linenum);    
            case '&': 
                if (in.peek() == '&') {
                    in.get();
                    return LexItem(AND, "&&", linenum); // Handle logical AND operator.
                }
                return LexItem(CONCAT, "&", linenum);    
            case '%': return LexItem(MOD, "%", linenum);
            case ':': 
                if (in.peek() == '=') {
                    in.get();
                    return LexItem(ASSOP, ":=", linenum); // Handle assignment operator.
                }
                return LexItem(COLON, ":", linenum);
            case ',': return LexItem(COMMA, ",", linenum);
            case ';': return LexItem(SEMICOL, ";", linenum);
            case '(': return LexItem(LPAREN, "(", linenum);
            case ')': return LexItem(RPAREN, ")", linenum);
            case '.': 
                if (in.peek() == '.') {
                    in.get();
                    return LexItem(CONCAT, "..", linenum); // Handle concatenation operator.
                }
                return LexItem(DOT, ".", linenum);
            default:
                return LexItem(ERR, string(1, ch), linenum); // Handle unknown characters.
        }
    }

    // Return DONE token when the end of the input stream is reached.
    return LexItem(DONE, "", linenum);
}

// Overloaded output operator for LexItem objects.
ostream& operator<<(ostream& out, const LexItem& tok) {
    switch (tok.GetToken()) {
        case ERR:
            out << "ERR: In line " << tok.GetLinenum() << ", Error Message {" << tok.GetLexeme() << "}" << endl;
            break;    
        case FCONST:
            out << "FCONST: (" << tok.GetLexeme() << ")" << endl;
            break;
        case SCONST:
            out << "SCONST: \"" << tok.GetLexeme() << "\"" << endl;
            break;
        case CCONST:
            out << "CCONST: '" << tok.GetLexeme() << "'" << endl;
            break;
        case BCONST:
            out << "BCONST: (" << tok.GetLexeme() << ")" << endl;
            break;        
        case IDENT:
            out << "IDENT: <" << tok.GetLexeme() << ">" << endl;
            break; 
        case ICONST:
            out << "ICONST: (" << tok.GetLexeme() << ")" << endl;
            break;    
        default:
            if (tok.GetToken() == BOOL) {
                out << "BOOL" << endl;
                return out;
            }
            if (tok.GetToken() == CHAR) {
                out << "CHAR" << endl;
                return out;
            }
            if (tok.GetToken() == INT) {
                out << "INT" << endl;
                return out;
            }
            for (const auto& kw : keywords) {
                if (tok.GetToken() == kw.second) {
                    out << kw.first << endl;
                    return out;
                }
            }
            
            // Handle other tokens.
            switch (tok.GetToken()) {
                case NEQ: out << "NEQ"; break;
                case EXP: out << "EXP"; break;
                case CONCAT: out << "CONCAT"; break;
                case GTHAN: out << "GTHAN"; break;
                case LTE: out << "LTE"; break;
                case LTHAN: out << "LTHAN"; break; 
                case GTE: out << "GTE"; break;
                case AND: out << "AND"; break;
                case OR: out << "OR"; break;
                case EQ: out << "EQ"; break;    
                case NOT: out << "NOT"; break;
                case MOD: out << "MOD"; break;
                case MINUS: out << "MINUS"; break;
                case MULT: out << "MULT"; break;
                case DIV: out << "DIV"; break;
                case ASSOP: out << "ASSOP"; break;
                case COMMA: out << "COMMA"; break;
                case RPAREN: out << "RPAREN"; break;
                case DOT: out << "DOT"; break;
                case PLUS: out << "PLUS"; break;    
                case COLON: out << "COLON"; break;
                case INT: out << "INT" << endl; break;
                case SEMICOL: out << "SEMICOL"; break;
                case LPAREN: out << "LPAREN"; break;
                default: out << "Token: " << tok.GetToken(); break;
            }
            out << endl;
            break;
    }
    return out;
}

// Function to determine if a lexeme is a keyword or an identifier.
LexItem id_or_kw(const string& lexeme, int linenum) {
    string upperLexeme = lexeme;
    for (char& c : upperLexeme) {
        c = toupper(c); // Convert lexeme to uppercase for case-insensitive comparison.
    }
    
    if (upperLexeme == "CONSTANT") {
        return LexItem(CONST, lexeme, linenum);
    }
    
    // Check if the lexeme is in the keyword map.
    auto it = keywords.find(upperLexeme);
    if (it != keywords.end()) {
        if (it->second == TRUE || it->second == FALSE) {
            return LexItem(BCONST, lexeme, linenum); // Handle boolean constants.
        }
        return LexItem(it->second, lexeme, linenum); // Return the corresponding keyword token.
    }
    return LexItem(IDENT, lexeme, linenum); // Return an identifier token if the lexeme is not a keyword.
}