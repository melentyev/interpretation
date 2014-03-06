#pragma once

#include "interpretation.h"

namespace Interpretation 
{ 
    class Parser 
    {
    public:
        static Parser *_self;
        vector<Token> tokens;
        map<TokenType, pType> basicTypes;
        int tokenNumber;
        int source_line, source_line_pos;
        pNamespace globalNamespace;
        Parser();
        vector<pStackFrame> callStack;
        vector<pStatementsBlock> parsingStatementsBlockStack;
        void parse(ifstream &stream);
        void run();
        static Parser* get();
        static Parser* create();
        void Parser::splitIntoTokens(string s);
        void parsingException(string func, string file, int line, Token t, string msg = "");
        pExprResult new_ExprResult(pType type = nullptr);
        Token &currentToken();
        Token &nextToken();
        bool hasMoreTokens();
        TokenType languageTypeByToken(const Token &t);
        ~Parser() {
            
        }
    };
}