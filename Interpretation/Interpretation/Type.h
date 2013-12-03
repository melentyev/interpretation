#pragma once

#include "interpretation.h"

namespace Interpretation 
{ 
    class Type {
    public:
        Parser *owner;
        bool isComplicated, isIntegral;
        Type(Parser *_owner, bool _isIntegral = false, 
            std::function<pExprResult(const std::string &)> __valueOf = nullptr) 
            : isComplicated (false), _valueOf(__valueOf), isIntegral(_isIntegral), owner(_owner),
            functionCallOperator(nullptr), arrayIndexerOperator(nullptr), isLogicalTrue(nullptr),
            copyConstructor(nullptr), definition(nullptr), defaultConstructor(nullptr) {}
        map <pair<TokenType, pType>, std::function<pExprResult(pExprResult, pExprResult) > > binaryOperations;
        map <TokenType, std::function<pExprResult(pExprResult) > > unaryPrefixOperations; 
        map <pType, std::function<pExprResult(pExprResult) > > typeCasts;
        std::function<pExprResult(const std::string &)> _valueOf;
        std::function<pExprResult(pExprResult, const vector<pExprResult>&)> functionCallOperator;
        std::function<pExprResult(pExprResult, const vector<pExprResult>&)> arrayIndexerOperator;
        std::function<bool(pExprResult)> isLogicalTrue;  
        std::function<pExprResult(pExprResult)> copyConstructor;
        std::function<pExprResult()> defaultConstructor;
        pExprResult valueOf(const std::string &s);
        pExprResult getObjectMember(pExprResult, const std::string &);
        pStatement definition;
    };
}