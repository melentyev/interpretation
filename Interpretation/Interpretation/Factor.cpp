#include "interpretation.h"
#include "Type.h"

namespace Interpretation 
{ 
     pExprResult Factor::execute() 
     {
        if (isUnary) {
            if (unaryOperator != TT_UNDEFINED) {
                return innerFactor->execute()->unaryPrefixOperation(unaryOperator);
            }
            else {
                return innerFactor->execute()->typeCast(innerTypename->getType() );
            }
        }
        else {
            pExprResult res = innerSingle->execute();
            auto field = begin(innerFieldsTaking);
            for (auto op: postfixOps) 
            {
                if (op.first == TT_PARENTHESIS_OPEN) 
                {
                    res = res->functionCallOperator(resolveArgValues(op.second));// MEMORY LEAK 
                }
                else if (op.first == TT_DOT || op.first == TT_ARROW) 
                {
                    auto prevVar = res;
                    if (res->fieldValues.count(field->second) > 0) 
                    {
                        res = (res->fieldValues) [field->second];
                    }
                    else 
                    {
                        if (res->type->definition->_namespace->
                            statementsBlock->vars.count(field->second) > 0) 
                        {
                            res = res->type->definition->_namespace->
                                statementsBlock->vars[field->second];
                        }
                    }
                    res->thisPointer = prevVar;
                    field++;
                }
                else
                {
                    res = res->arrayIndexerOperator(resolveArgValues(op.second));
                }
            }
            return res;
        }
    }
    pExecutable Factor::parse() {
        bool match = false;
        if (currentToken().type == TT_MINUS 
            || currentToken().type == TT_EXCL 
            || currentToken().type == TT_ASTERISK 
            || currentToken().type == TT_AMP
            || currentToken().type == TT_TILDA
            || currentToken().type == TT_DELETE) {
                isUnary = true;   
                match = true;
                unaryOperator = currentToken().type;  
                nextToken();
                innerFactor = new_Factor()->parse();  
        }
        else if (currentToken().type == TT_PARENTHESIS_OPEN) {
            tryParse([this, &match]() {
                nextToken();
                auto tn = new_Typename()->parse();
                if (currentToken().type == TT_PARENTHESIS_CLOSE) {
                    isUnary = true;
                    innerTypename = tn;
                    nextToken();
                    innerFactor = new_Factor()->parse();
                    match = true;
                }
                else {
                    throw exception( (__FILE__ + to_string(__LINE__) ).c_str() );
                }
            },
            [this, &match](exception &e) {
                isUnary = false;
                innerTypename = nullptr;
                innerFactor = nullptr;
                match = false;
            });
        }
        if (!match) {
            innerSingle = new_Single()->parse();
            while (currentToken().type == TT_BRACKET_OPEN || currentToken().type == TT_PARENTHESIS_OPEN 
                || currentToken().type == TT_DOT || currentToken().type == TT_ARROW) 
            {
                if (currentToken().type == TT_DOT || currentToken().type == TT_ARROW) {
                    TokenType saved = currentToken().type; 
                    if (nextToken().type != TT_WORD) 
                    {
                        throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );            
                    }
                    postfixOps.push_back(make_pair(saved, vector<pExecutable>()) );
                    innerFieldsTaking.push_back(std::make_pair(saved, currentToken().strVal) );
                    nextToken();
                }
                else {
                    postfixOps.push_back(make_pair(currentToken().type, vector<pExecutable>() ) );
                    nextToken();
                    if (currentToken().type != TT_PARENTHESIS_CLOSE) {
                        postfixOps.back().second.push_back( new_SingleExpr()->parse() );
                    }
                    while (currentToken().type == TT_COMMA) {
                        nextToken();
                        postfixOps.back().second.push_back( new_SingleExpr()->parse() );
                    }
                    if (Token::matchBrackets(postfixOps.back().first, currentToken().type ) ) {
                        nextToken();
                    }
                    else {
                        throw exception( (__FILE__ + to_string(__LINE__) ).c_str() );
                    }
                }
            }
        }
        return pFactor(this);
    }
    vector<pExprResult> Factor::resolveArgValues(const vector<pExecutable> &args) const
    {
        vector<pExprResult> result;
        for (auto arg: args) 
        {
            result.push_back(arg->execute() );
        }
        return result;
    }
}