#include "interpretation.h"
#include "Type.h"
#include "Parser.h"

namespace Interpretation { 
    pExprResult Single::execute() 
    {
        if (innerValue) 
        {
            return innerValue;
        }
        else if (innerToken.type != TT_UNDEFINED) 
        {
            TokenType langType = owner->languageTypeByToken(innerToken);
            if (owner->basicTypes.count(langType) > 0) 
            {
                pType t = owner->basicTypes[langType];
                return t->valueOf(innerToken.strVal);
            }
            else 
            {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
        }
        else if (innerExpr) 
        {
            return innerExpr->execute();
        }
        else if (innerNamespaceType)
        {
            
        }
        else if (id != "") 
        {
            for (auto it = owner->callStack.back()->statementsBlockStack.crbegin(); 
                it != owner->callStack.back()->statementsBlockStack.crend(); it++) 
            { 
                if( (*it)->vars.count(id) > 0) 
                {
                    pExprResult var = (*it)->vars[id];
                    return var;
                }
            }
            throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() ); 
        }
        else {
            throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
        }
    }
    pExecutable Single::parse() 
    {
        if (currentToken().type == TT_PARENTHESIS_OPEN) 
        {
            nextToken();
            innerExpr = new_Expr()->parse();
            if (currentToken().type == TT_PARENTHESIS_CLOSE) 
            {
                nextToken();
            }
            else {
                Parser::get()->parsingException(__FUNCTION__, __FILE__, __LINE__, currentToken() );
            }
        }
        else if (currentToken().type == TT_NUMBER || currentToken().type == TT_STRING) 
        {
            innerToken = currentToken();
            innerValue = this->execute();
            innerToken = Token(TT_UNDEFINED, "");
            nextToken();
        }
        else 
        {
            if (currentToken().type != TT_WORD && currentToken().type != TT_TRUE 
                && currentToken().type != TT_FALSE) 
            {
                Parser::get()->parsingException(__FUNCTION__, __FILE__, __LINE__, currentToken() );
            }
            id = currentToken().strVal;
            nextToken();
        }
        return pSingle(this);
    }
}