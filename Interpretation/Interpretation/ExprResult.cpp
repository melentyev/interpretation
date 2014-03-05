#include "interpretation.h"
#include "Type.h"
#include "Parser.h"

namespace Interpretation 
{
    pExprResult ExprResult::binaryOperation(TokenType op, pExprResult rhs) 
    { 
        if (type->binaryOperations.count(make_pair(op, rhs->type) ) > 0) 
        { 
            return type->binaryOperations[make_pair(op, rhs->type) ](this, rhs); 
        }
        else 
        {
            throw exception( ("binaryOperation not defined, (" + this->type->reflectName 
                + ") " + Token::repr(op) + " (" + rhs->type->reflectName + ")").c_str());
        }
    }
    
    pExprResult ExprResult::unaryPrefixOperation(TokenType op) 
    {
        if(type->unaryPrefixOperations.count(op) > 0) 
        { 
            return type->unaryPrefixOperations[op](this); 
        }
        else 
        {
            throw exception("unaryPrefixOperation not defined");
        }
    }
    pExprResult ExprResult::functionCallOperator(const vector<pExprResult> &args) 
    {
        return type->functionCallOperator(pExprResult(this), args);
    }
    pExprResult ExprResult::arrayIndexerOperator(const vector<pExprResult> &args) 
    {
        return type->arrayIndexerOperator(pExprResult(this), args);
    }
    bool ExprResult::isLogicalTrue()
    {
        if (type->isLogicalTrue) 
        {
            return type->isLogicalTrue(pExprResult(this) );
        }
        else if(type->typeCasts.count(type->owner->basicTypes[TT_BOOL]) > 0) 
        {
            return type->typeCasts[type->owner->basicTypes[TT_BOOL]](this)->value._bool;
        }
        else 
        {
            throw exception("isLogicalTrue not defined");
        }
    }
    pExprResult ExprResult::typeCast(pType tn) 
    {
        if(type->typeCasts.count(tn) > 0) 
        { 
            return type->typeCasts[tn](this); 
        }
        else 
        {
            throw exception("typeCast not defined");
        }
    }
    pExprResult ExprResult::getMember(const string &s) 
    {
        if (type->isComplicated) 
        {
            return type->getObjectMember(this, s);
        }
        else 
        {
            throw exception("typeCast not defined");
        }
    }
    /*ExprResult::~ExprResult() 
    {
    }
    */
}