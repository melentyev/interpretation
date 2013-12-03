#include "interpretation.h"
#include "VarDeclarationAllowDefault.h"
#include "Parser.h"

namespace Interpretation 
{  
    pDeclarativeStatement DeclarativeStatement::parse() 
    {
        Token &tok = owner->currentToken();
        if (tok.type == TT_NAMESPACE) 
        {
            nextToken();
            _namespace = new_Namespace()->parse();       
        }
        else if(tok.type == TT_TYPEDEF) 
        {
                
        }
        else if(tok.type == TT_STRUCT || tok.type == TT_CLASS) 
        {
            nextToken();
            isClass = true;
            if(tok.type == TT_STRUCT) {
                isStruct = true;
            }
            _namespace = new_Namespace()->parse();   
            if(currentToken().type != TT_SEMICOLON) 
            {
                throw exception( (string(__FILE__) + ": " + to_string(__LINE__) ).c_str() );    
            }
            nextToken();
        }
        else {
            pTypename tn = new_Typename()->parse();
            pVarDeclarationAllowDefault vd = new_VarDeclarationAllowDefault();
            vd->parse();
            if (owner->currentToken().type == TT_PARENTHESIS_OPEN) 
            {
                if (!vd->canBeFunctionDeclaration() ) 
                {
                    throw exception( (string("Exception") + to_string(__LINE__) ).c_str() );
                }
                nextToken();
                function = pFunction(new Function(owner, vd->vd->id) );
                function->args = new_FunctionArgs()->parse();
                
                currentToken();
                if (currentToken().type == TT_PARENTHESIS_CLOSE) 
                {
                    if (nextToken().type == ';') 
                    {
                        function->statementsBlock = nullptr;    
                    }
                    else {
                        function->statementsBlock = new_StatementsBlock()->parse();
                        for(auto vd: function->args->vds) {
                            function->statementsBlock->vars[vd->vd->id] = nullptr;
                        }
                    }
                }
                else {
                    throw exception( (string("Unexpeted token : ") + string(__FILE__) 
                        + ": " + to_string(__LINE__) ).c_str() ); 
                   
                }
            }
            else if(currentToken().type == TT_COMMA) 
            {
                    
            }
            else if (currentToken().type == TT_SEMICOLON)
            {
                vds.push_back(vd);
                nextToken();
            }
            else 
            {
                throw exception( (string("Unexpeted token") + to_string(__LINE__) ).c_str() );
            }
        }
        return pDeclarativeStatement(this);
    }
}