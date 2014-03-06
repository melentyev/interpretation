#pragma once

#include "interpretation.h"

namespace Interpretation 
{ 
    class Type {
    public:
        Parser *owner;
        bool isComplicated, isIntegral;
        string reflectName;
        Type(Parser *_owner, 
            bool _isIntegral = false, 
            function<pExprResult(const std::string &)> __valueOf = nullptr, 
            string _reflectName = "", 
            function<string(pExprResult)> _printableValue = nullptr,
            function<pExprResult(void)> __defaultConstructor = nullptr) 
            : 
            isComplicated (false), 
            _valueOf(__valueOf), 
            printableValue(_printableValue),
            isIntegral(_isIntegral), 
            owner(_owner),
            functionCallOperator(nullptr), 
            arrayIndexerOperator(nullptr), 
            isLogicalTrue(nullptr),
            copyConstructor(nullptr), 
            definition(nullptr), 
            _defaultConstructor(__defaultConstructor), 
            reflectName(_reflectName) {}
        map <pair<TokenType, pType>, std::function<pExprResult(pExprResult, pExprResult) > > binaryOperations;
        map <TokenType, std::function<pExprResult(pExprResult) > > unaryPrefixOperations; 
        map <pType, std::function<pExprResult(pExprResult) > > typeCasts;
        function<pExprResult(const std::string &)> _valueOf;
        function<pExprResult(pExprResult, const vector<pExprResult>&)> functionCallOperator;
        function<pExprResult(pExprResult, const vector<pExprResult>&)> arrayIndexerOperator;
        function<bool(pExprResult)> isLogicalTrue;  
        function<pExprResult(pExprResult)> copyConstructor;
        function<pExprResult()> _defaultConstructor;
        pExprResult defaultConstructor();
        pExprResult valueOf(const std::string &s);
        function<string(pExprResult)> printableValue;
        pExprResult getObjectMember(pExprResult, const std::string &);
        pStatement definition;
    };
}