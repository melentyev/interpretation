#include "interpretation.h"
#include "Parser.h"

namespace Interpretation {  
    pTypename Typename::parse() {
        bool found = false;
        switch (currentToken().type) {
        case TT_INT:
            simple = true, simpleType = TT_INT;
            type = owner->basicTypes[TT_INT];
            break;
        case TT_CHAR:
            simple = true, simpleType = TT_CHAR;
            break;
        case TT_FLOAT:
            simple = true, simpleType = TT_FLOAT;
            break;
        case TT_DOUBLE:
            simple = true, simpleType = TT_DOUBLE;
            break;
        case TT_UNSIGNED:
            simple = true;
            simpleTypeIsUnsigned = true;
            switch (nextToken().type) {
            case TT_INT:
                simpleType = TT_INT;
                break;
            case TT_CHAR:
                simpleType = TT_CHAR;
                break;
            default:
                throw exception ("Typename: undefined type");
                break;
            }
            break;
        default:
            vector<pStatementsBlock> &blocks = (owner->parsingStatementsBlockStack.empty() 
                ? owner->callStack.back()->statementsBlockStack
                : owner->parsingStatementsBlockStack );
            for (auto it = blocks.crbegin(); 
                it != blocks.crend(); it++) 
            { 
                if( (*it)->vars.count(currentToken().strVal) > 0 
                    && (*it)->vars[currentToken().strVal] != nullptr) 
                {
                    found = true;
                    auto type = (*it)->vars[currentToken().strVal];
                    if (type->evalType == ERT_TYPE) 
                    {
                        this->type = type->type;
                        break;
                    }
                    else 
                    {
                        throw exception ("Typename: undefined type");
                    }
                }
            }
            if (!found) {
                throw exception ("Typename: undefined type");
            }
            break;
        }
        nextToken();
        return pTypename(this);
    }
    
    pType Typename::getType() {
        if (this->simple) {
            return owner->basicTypes[simpleType];
        }
        else {
            return nullptr;
        }
    }
}