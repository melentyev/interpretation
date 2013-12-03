#include "interpretation.h"

namespace Interpretation 
{
    pExprResult Arithmetics::execute() {
        pExprResult res = (*begin(inner))->execute();
        auto op_it = begin(operators);
        for (auto it = (++(begin(inner) ) ); it != end(inner); it++, op_it++) {
            res = res->binaryOperation(*op_it, (*it)->execute() ); // MEMORY LEAK
        }
        return res;
    }
    pExecutable Arithmetics::parse() 
    {
        inner.push_back(new_Term()->parse() );
        bool isSingle = true;
        while (currentToken().type == TT_PLUS || currentToken().type == TT_MINUS) 
        {
                operators.push_back(currentToken().type);
                nextToken();
                inner.push_back(new_Term()->parse() );
                isSingle = false;
        }
        if (isSingle) {
            return inner.back();
        }
        else {
            return pArithmetics(this);
        }
    }
}