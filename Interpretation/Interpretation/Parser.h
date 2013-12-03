#pragma once

#include "interpretation.h"

namespace Interpretation 
{ 
    class Parser 
    {
    public:
        vector<Token> tokens;
        map<TokenType, pType> basicTypes;
        int tokenNumber;
        pNamespace globalNamespace;
        Parser();
        vector<pStackFrame> callStack;
        vector<pStatementsBlock> parsingStatementsBlockStack;
        void parse(ifstream &stream);
        void run();
        void Parser::splitIntoTokens(string s);
        pExprResult new_ExprResult(pType type = nullptr);
        Token &currentToken();
        Token &nextToken();
        bool hasMoreTokens() {
            return tokenNumber < ((int) tokens.size() - 1);
        }
        TokenType languageTypeByToken(const Token &t);
        ~Parser() {
            
        }
    };
}