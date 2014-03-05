#include "interpretation.h"
#include "VarDeclarationAllowDefault.h"
#include "Type.h"
#include "Parser.h"

namespace Interpretation 
{ 
    pExprResult Statement::execute() 
    {
        if (isClass) 
        {
            pType type = owner->parsingStatementsBlockStack.back()->vars[_namespace->id]->type;
            if ( !(type->defaultConstructor) )
            {
                type->defaultConstructor = [this, type]() 
                {
                    pExprResult res = this->owner->new_ExprResult(type);
                    for (auto it = begin(type->definition->_namespace->statementsBlock->vars);
                        it != end(type->definition->_namespace->statementsBlock->vars); it++) 
                    {
                        if (it->second && it->second->type == owner->basicTypes[TT_FUNCTION] ) {
                            continue;
                        }
                        res->fieldValues[it->first] = this->owner->new_ExprResult(this->owner->basicTypes[TT_INT]);
                    }
                    ;
                    return res;
                };
            }
        }
        else if (_namespace) 
        {
            pExprResult res = owner->new_ExprResult();
            res->evalType = ERT_NAMESPACE;
            res->value.pointer = (void*)this;
            owner->parsingStatementsBlockStack.back()->vars[_namespace->id] = res;
        }
        else if (this->function) 
        {    
            owner->parsingStatementsBlockStack.back()->vars[this->function->id] = 
                pExprResult(new ExprResult(owner->basicTypes[TT_FUNCTION]) );
            owner->parsingStatementsBlockStack.back()->vars[this->function->id]->value.pointer =
                this->function;
        }
        else if (isSpecial && owner->parsingStatementsBlockStack.empty() ) 
        {
            pExprResult res = nullptr;
            switch (specialType) 
            {
            case TT_ECHO:
                res = expr->execute();                
                cout << (res->type->printableValue 
                        ? res->type->printableValue(res) 
                        : "<" + res->type->reflectName + ">") << endl;            
                break;
            case TT_RETURN:
                owner->callStack.back()->returnValue = expr->execute();
                break;
            case TT_IF:
                res = expr->execute();
                if (res->type->isLogicalTrue) {
                    if (res->isLogicalTrue() )
                    {
                        statement1->execute();
                    }
                    else if(statement2) {
                        statement2->execute();
                    }
                }
                else {
                    // TODO EXCEPTION
                }
                break;
            case TT_FOR:
                owner->callStack.back()->statementsBlockStack.push_back(localStatementsBlock);
                statement1->execute();
                for (statement1->execute(); expr->execute()->isLogicalTrue(); expr2->execute() ) {
                    statement2->execute();
                }
                owner->callStack.back()->statementsBlockStack.pop_back();
                break;
            case TT_WHILE:
                owner->callStack.back()->statementsBlockStack.push_back(localStatementsBlock);
                while( expr->execute()->isLogicalTrue() ) 
                {
                    statement1->execute();
                }
                owner->callStack.back()->statementsBlockStack.pop_back();
                break;
            }
        }
        else if (tn) 
        {
            pStatementsBlock varLoc = (owner->parsingStatementsBlockStack.empty() 
                ? owner->callStack.back()->statementsBlockStack.back() 
                : owner->parsingStatementsBlockStack.back() );
            for (auto &vdDef: this->vds) {
                pExprResult rhs;
                if (!owner->parsingStatementsBlockStack.empty() ) {
                    varLoc->vars[vdDef->vd->id] 
                        = (tn->type->defaultConstructor ? tn->type->defaultConstructor() : nullptr);
                }
                else {
                    varLoc->vars[vdDef->vd->id] = (
                        vdDef->rhs 
                        ?
                            (rhs = vdDef->rhs->execute(), rhs->type->copyConstructor 
                            ? 
                                rhs->type->copyConstructor(rhs) 
                            : 
                                rhs) 
                        : 
                            (tn->type && tn->type->defaultConstructor 
                            ?
                                tn->type->defaultConstructor()
                            : 
                                nullptr)
                    );
                }
            }
        }
        else if(sb && owner->parsingStatementsBlockStack.empty() ) 
        {
            owner->callStack.back()->statementsBlockStack.push_back(sb);
            for (auto &st: sb->statements) {
                st->execute();
            }
            owner->callStack.back()->statementsBlockStack.pop_back();
        }
        else if(owner->parsingStatementsBlockStack.empty() ) {
            expr->execute();
        }
        return nullptr;
    }
    pStatement Statement::parse() {
        if (currentToken().type == TT_NAMESPACE) 
        {
            nextToken();
            _namespace = new_Namespace()->parse();       
        }
        else if(currentToken().type == TT_TYPEDEF) 
        {
                
        }
        else if(currentToken().type == TT_STRUCT || currentToken().type == TT_CLASS) 
        {
            isClass = true;
            if(currentToken().type == TT_STRUCT) {
                isStruct = true;
            }
            nextToken();
            _namespace = new_Namespace();
            _namespace->statementsBlock = new_StatementsBlock();
            _namespace->id = currentToken().strVal;
            pType newType = pType(new Type(owner, false) );
            pExprResult res = owner->new_ExprResult();
            res->evalType = ERT_TYPE;
            res->type = newType;
            newType->definition = this;
            owner->parsingStatementsBlockStack.back()->vars[_namespace->id] = res;
            _namespace->parse();   
            if(currentToken().type != TT_SEMICOLON) 
            {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );    
            }
            nextToken();
        }
        else if (currentToken().type == TT_BRACE_OPEN) {
            this->sb = new_StatementsBlock()->parse();
        }
        else if (currentToken().type == TT_BREAK 
            || currentToken().type == TT_RETURN 
            || currentToken().type == TT_ECHO 
            || currentToken().type == TT_CONTINUE) {
                isSpecial = true;
                specialType = currentToken().type;
                nextToken();
                if (specialType == TT_RETURN || specialType == TT_ECHO) {
                    expr = new_Expr()->parse();
                }
                if (currentToken().type != TT_SEMICOLON) {
                    throwTokenExpected(TT_SEMICOLON);
                }
                nextToken();
        }
        else if (currentToken().type == TT_IF) 
        {
            isSpecial = true;
            specialType = currentToken().type;
            nextToken();
            expr = new_Expr()->parse();
            statement1 = new_Statement()->parse();
            if (currentToken().type == TT_ELSE) 
            {
                nextToken();
                statement2 = new_Statement()->parse();
            }
        }
        else if (currentToken().type == TT_WHILE) 
        {
            localStatementsBlock = new_StatementsBlock();
            owner->parsingStatementsBlockStack.push_back(localStatementsBlock);
            isSpecial = true;
            specialType = currentToken().type;
            if (nextToken().type != TT_PARENTHESIS_OPEN) 
            {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
            nextToken();
            expr = new_Expr()->parse();
            if (currentToken().type != TT_PARENTHESIS_CLOSE) 
            {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
            nextToken();
            statement1 = new_Statement()->parse();
            owner->parsingStatementsBlockStack.pop_back();
        }
        else if (currentToken().type == TT_FOR) {
            localStatementsBlock = new_StatementsBlock();
            owner->parsingStatementsBlockStack.push_back(localStatementsBlock);
            isSpecial = true;
            specialType = currentToken().type;
            if (nextToken().type != TT_PARENTHESIS_OPEN) {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
            nextToken();
            statement1 = new_Statement()->parse();
            expr = new_Expr()->parse();
            if (currentToken().type != TT_SEMICOLON) {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
            nextToken();
            expr2 = new_Expr()->parse();
            if (currentToken().type != TT_PARENTHESIS_CLOSE) {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );
            }
            nextToken();
            statement2 = new_Statement()->parse();
            owner->parsingStatementsBlockStack.pop_back();
        }
        else {
            tryParse([this]() {  
                tn = new_Typename()->parse();
                pVarDeclarationAllowDefault vd = new_VarDeclarationAllowDefault()->parse();
                if (owner->currentToken().type == TT_PARENTHESIS_OPEN) 
                {
                    if (!vd->canBeFunctionDeclaration() ) 
                    {
                        throw exception( (string("Exception") + to_string(__LINE__) ).c_str() );
                    }
                    nextToken();
                    this->function = pFunction(new Function(owner, vd->vd->id) );
                    this->function->args = new_FunctionArgs()->parse();
                
                    if (currentToken().type == TT_PARENTHESIS_CLOSE) 
                    {
                        if (nextToken().type == ';') 
                        {
                            this->function->statementsBlock = nullptr;    
                        }
                        else 
                        {
                            this->function->statementsBlock = new_StatementsBlock()->parse();
                            for (auto vd: this->function->args->vds) 
                            {
                                this->function->statementsBlock->vars[vd->vd->id] = nullptr;
                            }
                        }
                    }
                    else 
                    {
                        throw exception( (string("Unexpeted token : ") + string(__FILE__) 
                            + ": " + to_string(__LINE__) ).c_str() ); 
                    }
                }
                else 
                {
                    if (currentToken().type == TT_COMMA || currentToken().type == TT_SEMICOLON) 
                    {
                        vds.push_back(vd);   
                    }
                    while (currentToken().type == TT_COMMA) 
                    {
                        nextToken();
                        vds.push_back(new_VarDeclarationAllowDefault()->parse() );
                    }
                    if (currentToken().type != TT_SEMICOLON) 
                    {
                        throw exception(to_string(__LINE__).c_str() );
                    }
                    nextToken();
                }
            },
            [this](exception &e) {
                vds.clear();
                tn = nullptr;
                expr = new_Expr()->parse();
                this->function = nullptr;
                if (currentToken().type != TT_SEMICOLON) {
                    throw exception(to_string(__LINE__).c_str() );
                }
                nextToken();
            });
        }
        return pStatement(this);
    }
}