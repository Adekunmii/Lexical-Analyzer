#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "lex.h"

using namespace std;

// External declaration of the keywords map from the lexer.
extern const map<string, Token> keywords;

int main(int argc, char* argv[]) {
    // Check if the input file is provided as a command-line argument.
    if (argc < 2) {
        cout << "No specified input file." << endl;
        return 1;
    }
    
    // Custom comparator for case-insensitive string comparison.
    struct CaseInsensitiveComp {
        bool operator()(const string& a, const string& b) const {
            string lowerA = a, lowerB = b;
            // Convert both strings to lowercase for comparison.
            for (size_t i = 0; i < b.length(); i++) {
                lowerB[i] = tolower(b[i]);
            }
            for (size_t i = 0; i < a.length(); i++) {
                lowerA[i] = tolower(a[i]);
            }
            return lowerB > lowerA;
        }
    };

    // Sets to store different types of constants and identifiers.
    set<string> numericConsts; // Stores numeric constants (integers and floats).
    set<string, CaseInsensitiveComp> identifiers; // Stores identifiers (case-insensitive).
    set<string> stringAndCharConsts; // Stores string and character constants.

    // Map to store keywords and their corresponding token types.
    map<Token, string> keywordsTokens; 

    // Create a lowercase version of the keywords map for case-insensitive lookup.
    map<string, Token, CaseInsensitiveComp> lowercasekeywords;
    for (const auto & kw : keywords) {
        string lowercaseKey = kw.first;
        for (char& c : lowercaseKey) {
            c = tolower(c); // Convert keyword to lowercase.
        }
        keywordsTokens[kw.second] = lowercaseKey; // Map token to lowercase keyword.
        lowercasekeywords[lowercaseKey] = kw.second; // Map lowercase keyword to token.
    }
    
    // Flags to control what information to display.
    bool showAll = false; // Show all tokens.
    bool showIds = false; // Show identifiers.
    bool showKws = false; // Show keywords.
    bool showNums = false; // Show numeric constants.
    bool showStrs = false; // Show string and character constants.
    
    string filename; // Variable to store the input file name.
    
    // Parse command-line arguments.
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-all") showAll = true; // Enable showing all tokens.
        else if (arg == "-id") showIds = true; // Enable showing identifiers.
        else if (arg == "-kw") showKws = true; // Enable showing keywords.
        else if (arg == "-num") showNums = true; // Enable showing numeric constants.
        else if (arg == "-str") showStrs = true; // Enable showing string/character constants.
        else if (filename.empty()) filename = arg; // Set the input file name.
        else if (arg.front() == '-') {
            cout << "Unrecognized flag {" << arg << "}" << endl; // Handle unrecognized flags.
            return 1;
        }
        else {
            cout << "Only one file name is allowed." << endl; // Handle multiple file names.
            return 1;
        }
    }
    
    // Open the input file.
    ifstream infile(filename);
    
    // Check if the file name is empty.
    if (filename.empty()) {
        cout << "No specified input file." << endl;
        return 1;
    }
    
    // Check if the file is empty.
    infile.peek(); 
    if (infile.eof()) {
        cout << "Empty file." << endl;
        return 0;
    }
      
    // Check if the file could not be opened.
    if (!infile) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }
    
    // Variables to track token count and line number.
    int tokenCount = 0;
    int lineNumber = 1;
    LexItem token;
    
    // Set to store found keywords.
    set<Token> foundkeywordsTokens;
    
    // Vector to store all tokens (not currently used in this code).
    vector<LexItem> allTokens;
    
    // Read tokens from the input file until the end is reached.
    while ((token = getNextToken(infile, lineNumber)).GetToken() != DONE) {
        if (token.GetToken() == ERR) {
            cout << token; // Print error token if encountered.
            return 1;
        }
        
        tokenCount++; // Increment token count.
      
        if (showAll) {
            cout << token; // Print token if -all flag is enabled.
        }
        
        Token t = token.GetToken();
        
        // Categorize tokens into identifiers, numeric constants, or string/character constants.
        if (t == IDENT) {
            identifiers.insert(token.GetLexeme()); // Store identifier.
        } 
        else if (t == ICONST || t == FCONST) {
            numericConsts.insert(token.GetLexeme()); // Store numeric constant.
        } 
        else if (t == SCONST || t == CCONST) {
            stringAndCharConsts.insert(token.GetLexeme()); // Store string/character constant.
        } 
        else {
            // Check if the token is a keyword.
            for (const auto& kw : keywords) {
                if (t == kw.second) {
                    foundkeywordsTokens.insert(t); // Store found keyword.
                    break;
                }
            }
        }
    }
    
    // Test case summary.
    cout << endl;
    cout << "Lines: " << lineNumber-1 << endl; // Print total lines processed.
    cout << "Total Tokens: " << tokenCount << endl; // Print total tokens.
    cout << "Numerals: " << numericConsts.size() << endl; // Print number of numeric constants.
    cout << "Characters and Strings : " << stringAndCharConsts.size() << endl; // Print number of string/character constants.
    cout << "Identifiers: " << identifiers.size() << endl; // Print number of identifiers.
    cout << "keywords: " << foundkeywordsTokens.size() << endl; // Print number of keywords.
    
    // Display numeric constants if -num flag is enabled.
    if (showNums && !numericConsts.empty()) {
        cout << "NUMERIC CONSTANTS:" << endl;
        
        vector<double> sortedNums;
        for (const auto& num : numericConsts) {
            double value = stod(num); // Convert string to double.
            sortedNums.push_back(value);
        }
       sort(sortedNums.begin(), sortedNums.end()); // Sort numeric constants.
       
        // Print sorted numeric constants.
        bool first = true;
        for (const auto& value : sortedNums) {
            if (!first) cout << ", ";
            if (value == static_cast<int>(value)) {
                cout << static_cast<int>(value); // Print as integer if it's a whole number.
            } else {
                cout << value; // Print as double otherwise.
            }
            first = false;
        }
        cout << endl;
    }
    
    // Display string and character constants if -str flag is enabled.
    if (showStrs && !stringAndCharConsts.empty()) {
         bool first = true;
        cout << "CHARACTERS AND STRINGS:" << endl;
        for (const auto& str : stringAndCharConsts) {
            if (!first) cout << ", ";
            first = false;
            cout << "\"" << str << "\""; // Print string/character constant.
        }
        cout << endl;
    }
    
    // Display identifiers if -id flag is enabled.
    if (showIds && !identifiers.empty()) {
        cout << "IDENTIFIERS:" << endl;
        bool first = true;
        for (const auto& id : identifiers) {
            if (!first) cout << ", ";
            cout << id; // Print identifier.
            first = false;
        }
        cout << endl;
    }
    
    // Display keywords if -kw flag is enabled.
    if (showKws && !foundkeywordsTokens.empty()) {
        cout << "keywords:" << endl;
        
        vector<string> keywordsName;
        
        // Convert found keywords to lowercase.
        for (const auto& tokenType : foundkeywordsTokens) {
            for (const auto& kw : keywords) {
                if (tokenType == kw.second) {
                    string lowerkeywords = kw.first;
                    for (char& c : lowerkeywords) {
                        c = tolower(c);
                    }
                    keywordsName.push_back(lowerkeywords);
                    break;
                }
            }
        }
        
        // Print sorted keywords.
        bool first = true;
        for (const auto& name : keywordsName) {
            if (!first) cout << ", ";
            cout << name; // Print keyword.
            first = false;
        }
        cout << endl;
    }
    
    return 0;
}