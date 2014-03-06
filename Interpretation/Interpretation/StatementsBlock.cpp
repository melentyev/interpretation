#include "interpretation.h"
#include "Parser.h"

namespace Interpretation { 
    pStatementsBlock StatementsBlock::parse() {
        if (currentToken().type != TT_BRACE_OPEN) 
        {
            Parser::get()->parsingException(__FUNCTION__, __FILE__, __LINE__, currentToken() );
        }
        owner->parsingStatementsBlockStack.push_back(pStatementsBlock(this) );
        nextToken();
        while (currentToken().type != TT_BRACE_CLOSE) {
            statements.push_back(new_Statement()->parse() );
            statements.back()->execute();
        }
        nextToken();
        owner->parsingStatementsBlockStack.pop_back();
        return pStatementsBlock(this);
    }
}