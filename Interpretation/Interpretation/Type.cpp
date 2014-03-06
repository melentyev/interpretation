#include "Type.h"

namespace Interpretation {
    pExprResult Type::valueOf(const std::string &s) {
        pExprResult res = _valueOf(s);
        res->type = this;
        return res;
    }
    pExprResult Type::defaultConstructor() 
    {
        pExprResult res = _defaultConstructor();
        res->type = this;
        return res;
    }
    pExprResult Type::getObjectMember(pExprResult obj, const std::string &field) {
        return obj->fieldValues[field];
    }
}