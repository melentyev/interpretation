#include "interpretation.h"

namespace Interpretation {
    pExprResult Expr::execute() {
        pExprResult res = nullptr;
        for (auto it = begin(ses); it != end(ses); it++) {
            res = (*it)->execute();
            if (it != end(ses) ) {
                // TODO: DESTROY RES
            }
        }
        return res;
    }
    pExecutable Expr::parse() {
        ses.push_back(new_SingleExpr()->parse() );
        bool isSingle = true;
        while (currentToken().type == TT_COMMA) {
            nextToken();
            ses.push_back(new_SingleExpr()->parse() );
            isSingle = false;
        }
        if (isSingle) {
            return ses.back();
        }
        else {
            return pExecutable(this);
        }
    }
}