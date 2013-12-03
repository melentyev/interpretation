#include "interpretation.h"

namespace Interpretation 
{ 
    class VarDeclarationAllowDefault : public Nonterminal 
    {
    public:
        pVarDeclaration vd;
        pExecutable rhs;
        VarDeclarationAllowDefault(Parser *_owner) : Nonterminal(_owner), vd(nullptr), rhs(nullptr) {}
        pVarDeclarationAllowDefault parse();
        bool canBeFunctionDeclaration() 
        {
            return !(rhs);
        }
    };
}