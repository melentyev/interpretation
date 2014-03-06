#include "interpretation.h"
#include "Parser.h"
using namespace std;

namespace Interpretation 
{
    pExprResult SingleExpr::execute() {
        if (bThrow) {
            return nullptr;        // TODO: throw!!!
        }
        else {
            return this->inner->execute();
        }
    } 
    pExecutable SingleExpr::parse() {
        if (currentToken().type == TT_THROW) {
            bThrow = true;
        }
        inner = new_ThrowableExpr()->parse();
        if (!bThrow) {
            return inner;
        }
        else {
            return pExecutable(this);
        }
    }
}

using namespace Interpretation;
int (main)(int (argc) ) 
{ 
    ifstream in = ifstream("input.txt", ios_base::in);
    Parser::get()->parse(in);
    Parser::get()->run();
    system("pause");
    return 0;
}