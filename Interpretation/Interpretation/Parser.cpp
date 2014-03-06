#include "interpretation.h"
#include "Type.h"
#include "VarDeclarationAllowDefault.h"
#include "Parser.h"

#define OP_DEFINING(var_ret, var_l, var_r, TT, SIGN, type_ret, type_l, type_r) var_l->binaryOperations[make_pair(TT, var_r)] = \
    [functionType, intType, boolType, doubleType, this](pExprResult lhs, pExprResult rhs)   \
    {                                                                                       \
        pExprResult res = new_ExprResult(var_ret);                                          \
        res->value.type_ret = lhs->value.type_l SIGN rhs->value.type_r; return res; }

#define OP_DEFINING_SIMPLE(var, TT, SIGN, type) var->binaryOperations[make_pair(TT, var)] = \
    [var, this](pExprResult lhs, pExprResult rhs)                                       \
    {                                                                                   \
        pExprResult res = new_ExprResult(var);                                          \
        res->value.type = lhs->value.type SIGN rhs->value.type;                         \
        return res;                                                                     \
    }

#define OP_DEFINING_ASSIGN(type_var, type) type_var->binaryOperations[make_pair(TT_ASSIGN, type_var)] = \
            [type_var, this](pExprResult lhs, pExprResult rhs)              \
        {                                                                   \
            pExprResult res = new_ExprResult(type_var);                     \
            lhs->value.type = (rhs->value.type);                            \
            res->value.type = (rhs->value.type);                            \
            return res;                                                     \
        }

#define OP_DEFINING_TYPECAST_SIMPLE(from, to, from_f, to_f, T)                          \
    from->typeCasts[to] = [intType, doubleType, boolType, this](pExprResult operand)    \
    {                                                                                   \
        pExprResult res = new_ExprResult(to);                                           \
        res->value.to_f = (T)(operand->value.from_f);                                   \
        return res;                                                                     \
    }                                                                                   
#define FUNC_DEFINING_SIMPLE_ARG1(type, strName, f, from_f, to_f)                       \
        globalNamespace->statementsBlock->vars[strName] = new_ExprResult(functionType); \
        globalNamespace->statementsBlock->vars[strName]->value.pointer                  \
            = new Function(this, strName, [this, type](const vector<pExprResult> &args) \
        {                                                                               \
            pExprResult res = new_ExprResult(type);                                     \
            res->value.to_f = f( ( *begin(args) )->value.from_f);                       \
            return res;                                                                 \
        });        
#define OP_DEFINING_UNARY(var_ret, TT, SIGN, type_ret)                                  \
    var_ret->unaryPrefixOperations[TT] = [var_ret, this](pExprResult operand)           \
        {                                                                               \
            pExprResult res = new_ExprResult(var_ret);                                  \
            res->value.type_ret = (SIGN(operand->value.type_ret) );                     \
            return res;                                                                 \
        };
namespace Interpretation 
{  
    Parser* Parser::_self = Parser::create();
    Parser::Parser() 
    {
        globalNamespace = pNamespace(new Namespace(this, "") ); 
        globalNamespace->statementsBlock = globalNamespace->new_StatementsBlock();
        pType functionType = basicTypes[TT_FUNCTION] = new Type(this);
        functionType->functionCallOperator = 
            [functionType, this](pExprResult funcPtr, const vector<pExprResult>& args) 
        {
            pFunction func = (Function*)(funcPtr->value.pointer);
            return func->execute(args, funcPtr->thisPointer);
        };
        pType intType = basicTypes[TT_INT] = new Interpretation::Type(this, true, [this](const string &s) 
            {
                pExprResult res = new_ExprResult();
                res->value._int = stoi(s);
                return res;
            }, "int", [](pExprResult val) 
            {
                char buf[100]; 
                sprintf(buf, "%d", val->value._int); 
                return string(buf); 
            });
        pType boolType = basicTypes[TT_BOOL] = new Interpretation::Type(this, true, [this](const string &s) 
            {
                pExprResult res = new_ExprResult();
                res->value._bool = (s == "true");
                return res;
            }, "bool", [](pExprResult val) 
            { 
                return (val->value._bool ? "true" : "false"); 
            });
        pType doubleType = basicTypes[TT_DOUBLE] = new Type(this, false, [this](const string &s) 
            {
                pExprResult res = new_ExprResult();
                res->value._double = stod(s);
                return res;
            }, "double", [](pExprResult val) 
            {
                char buf[100]; 
                sprintf(buf, "%lf", val->value._double); 
                return string(buf); 
            }, [this]() {  return new_ExprResult();  });
        pType mstrType = basicTypes[TT_MSTR] = new Type(this, false, [this](const string &s) 
            {
                pExprResult res = new_ExprResult();
                res->value.pointer = new string(s);
                return res;
        }, "mstr", [](pExprResult val) { return *((string*)val->value.pointer); } );
        OP_DEFINING(intType, intType, intType, TT_PLUS,     +, _int, _int, _int);
        OP_DEFINING(intType, intType, intType, TT_MINUS,    -, _int, _int, _int);
        OP_DEFINING(intType, intType, intType, TT_ASTERISK, *, _int, _int, _int);
        OP_DEFINING(intType, intType, intType, TT_SLASH,    /, _int, _int, _int);
        OP_DEFINING(intType, intType, intType, TT_PERCENT,  %, _int, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_LESS,    < , _bool, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_GR,      > , _bool, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_EQUAL,   == , _bool, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_LESS_EQ, <= , _bool, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_GR_EQ,   >= , _bool, _int, _int);
        OP_DEFINING(boolType, intType, intType, TT_NOT_EQ,  != , _bool, _int, _int);
        OP_DEFINING_ASSIGN(intType, _int);
        OP_DEFINING_ASSIGN(doubleType, _double);
        OP_DEFINING_UNARY(intType, TT_MINUS, -, _int);
        OP_DEFINING_UNARY(doubleType, TT_MINUS, -, _double);
        intType->copyConstructor = 
            [intType, this](pExprResult obj) 
        {
            pExprResult res = new_ExprResult(intType);
            res->value._int = (obj->value._int); 
            return res; 
        };
        
        intType->isLogicalTrue = [intType, this](pExprResult operand) 
        {
            return (bool)(operand->value._int != 0);
        };
 
        OP_DEFINING_SIMPLE(doubleType, TT_PLUS, +, _double);
        OP_DEFINING_SIMPLE(doubleType, TT_MINUS, -, _double);
        OP_DEFINING_SIMPLE(doubleType, TT_ASTERISK, *, _double);
        OP_DEFINING_SIMPLE(doubleType, TT_SLASH, /, _double);

        OP_DEFINING_TYPECAST_SIMPLE(doubleType, intType, _double, _int, int);
        OP_DEFINING_TYPECAST_SIMPLE(intType, doubleType, _int, _double, double);

        boolType->isLogicalTrue = [](pExprResult operand) 
        {
            return (operand->value._bool);
        };
        globalNamespace->statementsBlock->vars["read"] = new_ExprResult(functionType);
        globalNamespace->statementsBlock->vars["read"]->value.pointer 
            = new Function(this, "read", [this, intType](const vector<pExprResult> &args) -> pExprResult
        {
            cout << "Enter " << args.size() << " int(s): " << endl;
            for (auto &arg: args) 
            {
                cin >> arg->value._int;
            }
            return (pExprResult)nullptr;
        });
        FUNC_DEFINING_SIMPLE_ARG1(doubleType, "sin", sin, _double, _double);
        FUNC_DEFINING_SIMPLE_ARG1(doubleType, "sqrt", sqrt, _double, _double);
        
        globalNamespace->statementsBlock->vars["rand"] = new_ExprResult(functionType);
        globalNamespace->statementsBlock->vars["rand"]->value.pointer 
            = new Function(this, "rand", [this, intType](const vector<pExprResult> &args) 
        {
            pExprResult res = new_ExprResult(intType);
            res->value._int = rand();
            return res;
        });
        globalNamespace->statementsBlock->vars["true"] = new_ExprResult(boolType);
        globalNamespace->statementsBlock->vars["true"]->value._bool = true;
        globalNamespace->statementsBlock->vars["false"] = new_ExprResult(boolType);
        globalNamespace->statementsBlock->vars["false"]->value._bool = false;

    }
    void Parser::run() {
        try 
        {
            if (globalNamespace->statementsBlock->vars.count("main") > 0) 
            {
                globalNamespace->statementsBlock->vars["main"]->functionCallOperator(vector<pExprResult>());
            }
            else 
            {
                throw exception("entry point not found");
            }
        }
        catch (exception &e) 
        {
            cout << e.what() << endl;
            system("pause");
        }
    }
    void Parser::parse(ifstream &stream) 
    {
        string str;
        source_line = 1;
        while (getline(stream, str) ) 
        {
            source_line_pos = 1;
            splitIntoTokens(str); 
            source_line++;
        }
        tokens.push_back(Token(TT_END, "") );
        tokenNumber = 0;
        parsingStatementsBlockStack.push_back(globalNamespace->statementsBlock);
        try 
        {
            while (hasMoreTokens() ) 
            {
                pStatement pDecl = globalNamespace->new_Statement()->parse();
                pDecl->execute();
            }
        }
        catch (exception &e) 
        {
            cout << e.what() << endl;
            system("pause");
        }
        parsingStatementsBlockStack.pop_back();
    }
    void Parser::splitIntoTokens(string s) 
    {
        string::iterator it = s.begin();
        static bool isStringToken = false;
        static bool isEscapeSequence = false;
        static bool isMultyLineComment = false;
        static Token current(TT_UNDEFINED, "");
        int token_started = this->source_line_pos;
        static auto pushToken = [this, &token_started](Token &token ) 
        { 
            if(token.type != TT_UNDEFINED) 
            {  
                if (token.type == TT_WORD) 
                {
                    if (token.strVal == "int") token.type = TT_INT;
                    else if (token.strVal == "char") token.type = TT_CHAR;
                    else if (token.strVal == "float") token.type = TT_FLOAT;
                    else if (token.strVal == "double") token.type = TT_DOUBLE;
                    else if (token.strVal == "long") token.type = TT_LONG;
                    else if (token.strVal == "bool") token.type = TT_BOOL;
                    else if (token.strVal == "mstr") token.type = TT_MSTR;                    
                    else if (token.strVal == "return") token.type = TT_RETURN;
                    else if (token.strVal == "while") token.type = TT_WHILE;
                    else if (token.strVal == "if") token.type = TT_IF;
                    else if (token.strVal == "else") token.type = TT_ELSE;
                    else if (token.strVal == "for") token.type = TT_FOR;
                    else if (token.strVal == "break") token.type = TT_BREAK;
                    else if (token.strVal == "continue") token.type = TT_CONTINUE;
                    else if (token.strVal == "class") token.type = TT_CLASS;
                    else if (token.strVal == "struct") token.type = TT_STRUCT;
                    else if (token.strVal == "echo") token.type = TT_ECHO;
                    else if (token.strVal == "true") token.type = TT_TRUE;
                    else if (token.strVal == "false") token.type = TT_FALSE;
                }
                else if(token.type == TT_SIGNS) 
                {
                    if (token.strVal == "+") token.type = TT_PLUS;
                    else if (token.strVal == "-") token.type = TT_MINUS;
                    else if (token.strVal == "*") token.type = TT_ASTERISK;
                    else if (token.strVal == "/") token.type = TT_SLASH;
                    else if (token.strVal == "%") token.type = TT_PERCENT;
                    else if (token.strVal == "=") token.type = TT_ASSIGN;
                    else if (token.strVal == "<") token.type = TT_LESS;
                    else if (token.strVal == ">") token.type = TT_GR;
                    else if (token.strVal == "==") token.type = TT_EQUAL;
                    else if (token.strVal == ">=") token.type = TT_GR_EQ;
                    else if (token.strVal == "<=") token.type = TT_LESS_EQ;
                    else if (token.strVal == "!=") token.type = TT_NOT_EQ;
                    else if (token.strVal == "||") token.type = TT_DOUBLE_PIPE;
                    else if (token.strVal == "&&") token.type = TT_DOUBLE_AMP;
                    else if (token.strVal == ",") token.type = TT_COMMA;
                    else if (token.strVal == ".") token.type = TT_DOT;
                    else if (token.strVal == "->") token.type = TT_ARROW;
                    else if (token.strVal == "!") token.type = TT_EXCL;
                    
                }
                token.source_line = this->source_line;
                token.source_line_pos = token_started;
                tokens.push_back(token); 
                token_started = this->source_line_pos + 1;
                token = Token(TT_UNDEFINED, "");
            }
        };
        while (it != s.end() ) 
        {
            if (isMultyLineComment) 
            {
                if (*it == '*' && *(it + 1) == '/') 
                {
                    isMultyLineComment = false;
                    it++;
                    this->source_line_pos++;
                }
            }
            else if (isStringToken) 
            {
                switch(*it) 
                {
                case '\\':
                    if (isEscapeSequence) 
                    {
                        switch (*it) 
                        {
                        case 'n':
                            current.strVal += "\n", isEscapeSequence = false;
                            break;
                        case 'r':
                            current.strVal += "\r", isEscapeSequence = false;
                            break;
                        case 't':
                            current.strVal += "\t", isEscapeSequence = false;
                            break;
                        case '\"':
                            current.strVal += "\"", isEscapeSequence = false;
                            break;
                        default:
                            isEscapeSequence = false;
                            break;
                        }
                    }
                    else {
                        isEscapeSequence = true;
                    }
                case '\"':
                    pushToken(current);
                    isStringToken = false; 
                    break;
                default:
                    current.strVal += *it;
                    break;
                }
            }
            else if (*it == '\"') 
            {
                pushToken(current);
                current.type = TT_STRING;
                isStringToken = true;
            }
            else if (*it == ' ' || *it == '\n' || *it == '\t') 
            {
                pushToken(current);
            }
            else if (*it == '(') 
            {
                pushToken(current), pushToken(current = Token(TT_PARENTHESIS_OPEN, "(") );
            }
            else if (*it == ')') 
            {
                pushToken(current), pushToken(current = Token(TT_PARENTHESIS_CLOSE, ")"));
            }
            else if (*it == '{') 
            {
                pushToken(current), pushToken(current = Token(TT_BRACE_OPEN, "{"));
            }
            else if (*it == '}') 
            {
                pushToken(current), pushToken(current = Token(TT_BRACE_CLOSE, "}"));
            }
            else if (*it == '[') 
            {
                pushToken(current), pushToken(current = Token(TT_BRACKET_OPEN, "{"));
            }
            else if (*it == ']') 
            {
                pushToken(current), pushToken(current = Token(TT_BRACKET_CLOSE, "}"));
            }
            else if (*it == ';') {
                pushToken(current), pushToken(current = Token(TT_SEMICOLON, ";"));
            }
            else if (*it == '*' && current.strVal == "/") 
            {
                isMultyLineComment = true;
                current = Token(TT_UNDEFINED, "");
            }
            /*else if (*it == '/' && current.strVal == "*") 
            {
                current = Token(TT_UNDEFINED, "");
                break;
            }*/
            else if (*it == '.') 
            {
                if (current.type == TT_NUMBER) 
                {
                    current.strVal += *it;
                }
                else 
                {
                    pushToken(current);
                    current.type = TT_SIGNS; 
                    current.strVal += *it;
                }
            }
            else if (isdigit(*it) ) {
                if (current.type == TT_UNDEFINED) {
                    current = Token(TT_NUMBER, string("") + *it);
                }
                else if (current.type == TT_NUMBER || current.type == TT_WORD 
                    || current.strVal == "." || current.strVal == "-")
                {
                    current.strVal += *it;
                    if (current.type == TT_SIGNS) 
                    {
                        current.type = TT_NUMBER;
                    }
                }
            }
            else if (Token::canBeUsedInIdentifier(*it) ) {
                if (current.type != TT_WORD) {
                    if (current.type == TT_UNDEFINED) {
                        current = Token(TT_WORD, string("") );
                    }
                    else if(current.type != TT_NUMBER) {
                        pushToken(current);
                        current.type = TT_WORD;
                    }
                }
                current.strVal += *it; 
            }
            else 
            {
                if (current.type != TT_SIGNS) 
                {
                    pushToken(current);
                    current.type = TT_SIGNS;
                }
                current.strVal += *it;
            }
            it++;
            this->source_line_pos++;
        }
        pushToken(current);
    }
    Token & Parser::currentToken() 
    {
        if (tokenNumber >= (int)tokens.size() ) 
        {
            throw exception("Token expected, but not found");
        }
        else 
        {
            return *(tokens.begin() + tokenNumber);
        }
    }
    Token & Parser::nextToken() 
    {
        tokenNumber++;
        return currentToken();
    }
    TokenType Parser::languageTypeByToken(const Token &t) 
    {
        if(t.type == TT_NUMBER) {
            bool isInt = true;
            bool isFloat = false;
            auto k = 1.0F;
            for (auto c: t.strVal) 
            {
                if (c == '.' || c == 'e' || c == 'E' || c == 'f' || c == 'F') 
                {
                        isInt = false;
                }
                if (c == 'f' || c == 'F') 
                {
                    isFloat = true;
                }
            }
            return (isInt ? TT_INT : (isFloat ? TT_FLOAT : TT_DOUBLE) );
        }
        else if(t.type == TT_STRING) return TT_MSTR;
        else if(t.type == TT_TRUE || t.type == TT_FALSE) return TT_BOOL;
        return TT_UNDEFINED;
    }
    void Parser::parsingException(string func, string file, int line, Token t, string msg) 
    {
        throw exception( (string("parsingException:\n")
            + "Source func: " + func + "\n"
            + "Source file: " + file + "\n"
            + "Source line: " + to_string(__LINE__) + "\n"
            + "Token: " + to_string(t.source_line) + ":" + to_string(t.source_line_pos) + "\n"
            + "Msg: " + msg + (msg.empty() ? "" : "\n" ) ).c_str() );
    }
    pExprResult Parser::new_ExprResult(pType type) 
    {
        return pExprResult(new ExprResult(type) );
    }
    bool Parser::hasMoreTokens() 
    {
        return tokenNumber < ((int) tokens.size() - 1);
    }
    Parser* Parser::get() 
    {
        return _self;
    }
    Parser* Parser::create() 
    {
        Parser::_self = new Parser();
        return Parser::_self;
    }
}