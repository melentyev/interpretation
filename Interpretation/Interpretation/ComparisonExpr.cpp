#include "interpretation.h"

namespace Interpretation 
{
    pExprResult ComparisonExpr::execute() {
        pExprResult res = (*begin(inner))->execute();
        auto op_it = begin(operators);
        for (auto it = (++(begin(inner) ) ); it != end(inner); it++, op_it++) {
            res = res->binaryOperation(*op_it, (*it)->execute() ); // MEMORY LEAK
        }
        return res;
    }
    pExecutable ComparisonExpr::parse() 
    {
        inner.push_back(new_Arithmetics()->parse() );
        bool isSingle = true;
        while (currentToken().type == TT_EQUAL || currentToken().type == TT_GR_EQ 
            || currentToken().type == TT_LESS_EQ || currentToken().type == TT_GR 
            || currentToken().type == TT_LESS ) 
        {
                operators.push_back(currentToken().type);
                nextToken();
                inner.push_back(new_Arithmetics()->parse() );
                isSingle = false;
        }
        if (isSingle) {
            return inner.back();
        }
        else {
            return pComparisonExpr(this);
        }
    }
}