#include "interpretation.h"

namespace Interpretation {
    pNamespace Namespace::parse() {
        if (currentToken().canBeIdentifier() ) {
            id = currentToken().strVal;
            nextToken();
            this->statementsBlock->parse();
        }
        else {
            throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() ); 
        }
        return pNamespace(this);
    }
}