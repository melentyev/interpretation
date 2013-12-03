#include "interpretation.h"
#include "VarDeclarationAllowDefault.h"
#include "Type.h"
#include "Parser.h"

namespace Interpretation 
{
    pExprResult Function::execute(const vector<pExprResult>& argsVal, pExprResult thisPointer) 
    {
        if (specialBehavior) 
        {
            pExprResult res = specialBehavior(argsVal);
            return res;
        }
        auto itVal = argsVal.begin();
        auto statementsBlockOnStack = new_StatementsBlock();
        for (auto it = this->args->vds.begin(); it != this->args->vds.end(); it++, itVal++) 
        {
            statementsBlockOnStack->vars[(*it)->vd->id] = (*itVal);
        }
        if (thisPointer) 
        {
            statementsBlockOnStack->vars["this"] = thisPointer;
        }
        owner->callStack.push_back(new StackFrame() );        
        owner->callStack.back()->statementsBlockStack.push_back(owner->globalNamespace->statementsBlock);
        owner->callStack.back()->statementsBlockStack.push_back(statementsBlockOnStack);
        for (auto &st: statementsBlock->statements) 
        {
            st->execute();
            if (owner->callStack.back()->returnValue) 
            {
                break;
            }
        }
        auto result = owner->callStack.back()->returnValue;
        owner->callStack.back()->statementsBlockStack.pop_back();// MEMORY LEAK
        owner->callStack.back()->statementsBlockStack.pop_back();// MEMORY LEAK
        owner->callStack.pop_back(); // MEMORY LEAK
        return result;
    }
}