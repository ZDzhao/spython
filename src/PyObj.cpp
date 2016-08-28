
#include "PyObj.h"
#include "ExprAST.h"

using namespace std;
using namespace ff;
/*
PyObjPtr& PyObjFuncDef::exeFunc(PyContext& context, PyObjPtr& self, ExprASTPtr& arglist){
    DMSG(("PyObjFuncDef::exeFunc...\n"));
    try
    {
        vector<PyObjPtr> allValue;
        FuncArglist* parg = arglist.cast<FuncArglist>();
        map<string, PyObjPtr> nameArg2Value;//!�Ƿ��о������� 

        for (unsigned int i = 0; i < parg->allArgs.size(); ++i){
            FuncArglist::ArgInfo& argInfo = parg->allArgs[i];
            DMSG(("PyObjFuncDef::argType...%s\n", argInfo.argType.c_str()));
            if (argInfo.argType.empty()){
                PyObjPtr v = argInfo.argVal->eval(context);
                allValue.push_back(v);
            }
            else if (argInfo.argType == "="){
                PyObjPtr v = argInfo.argVal->eval(context);
                if (argInfo.argKey->getType() == EXPR_VAR)
                {
                    nameArg2Value[argInfo.argKey.cast<VariableExprAST>()->name] = v;
                }
            } 
        }

        DMSG(("PyObjFuncDef::exeFunc...%u\n", allValue.size()));
        PyContextBackUp backup(context);
        context.curstack = self;
        
        ParametersExprAST* pParametersExprAST = parameters.cast<ParametersExprAST>();
        unsigned int hasConsumeArg = 0;
        for (unsigned j = 0; j < pParametersExprAST->allParam.size(); ++j){
            ParametersExprAST::ParameterInfo& paramInfo = pParametersExprAST->allParam[j];
            
            const string& strVarName = paramInfo.paramKey.cast<VariableExprAST>()->name;
            map<string, PyObjPtr>::iterator it = nameArg2Value.find(strVarName);
            if (it != nameArg2Value.end() && paramInfo.paramType.empty()){//!���������*a, **b,����ȥ��ȡ�������� 
                //!���о�������, ����Ƿ����ظ���ֵ
                if (allValue.size() > hasConsumeArg){
                    throw PyException::buildException("got multiple values for keyword argument");
                }
                
                PyObjPtr& ref = paramInfo.paramKey->eval(context);
                ref = it->second;
                nameArg2Value.erase(it);
            }
            else{
                if (paramInfo.paramType.empty() == false){
                    if (paramInfo.paramType == "*"){
                        PyObjPtr pVal = new PyObjTuple();
                        PyObjPtr& ref = paramInfo.paramKey->eval(context);
                        ref = pVal;
                        
                        for (unsigned int m = hasConsumeArg; m < allValue.size(); ++m){
                            FuncArglist::ArgInfo& argInfo = parg->allArgs[m];
                            if (argInfo.argType == "=" || argInfo.argType == "**"){
                                break;
                            }
                            pVal.cast<PyObjTuple>()->values.push_back(allValue[m]);
                        }
                    }
                    else if (paramInfo.paramType == "**"){
                        PyObjPtr pVal = new PyObjDict();
                        PyObjPtr& ref = paramInfo.paramKey->eval(context);
                        ref = pVal;
                        
                        for (unsigned int m = hasConsumeArg; m < parg->allArgs.size(); ++m){
                            FuncArglist::ArgInfo& argInfo = parg->allArgs[m];
                            if (argInfo.argType != "=" || argInfo.argKey->getType() != EXPR_VAR){
                                throw PyException::buildException("given more arg");
                            }
                            const string& keyName = argInfo.argKey.cast<VariableExprAST>()->name;
                            PyObjPtr tmpKey = new PyObjStr(keyName);
                            pVal.cast<PyObjDict>()->values[tmpKey] = nameArg2Value[keyName];
                            nameArg2Value.erase(keyName);
                        }
                    }
                }
                else if (allValue.size() > hasConsumeArg){
                    //!�����������Ǿ�����������ôҪ����
                    PyObjPtr& ref = paramInfo.paramKey->eval(context);
                    ref = allValue[hasConsumeArg];
                    ++hasConsumeArg;
                }
                else if (paramInfo.paramDefault){
                    PyObjPtr v = paramInfo.paramDefault->eval(context);
                    PyObjPtr& ref = paramInfo.paramKey->eval(context);
                    ref = v;
                }
                else{
                    throw PyException::buildException("need more arg num");
                }
            }
        }
        suite->eval(context);
    }
    catch(ReturnSignal& s){
        self->m_objStack.clear();
        return context.getCacheResult();
    }
    
    self->m_objStack.clear();
    return context.cacheResult(PyObjTool::buildNone());
}
*/
PyObjPtr PyObjClassDef::handleCall(PyObjPtr self, list<PyObjPtr>& args){
    //DMSG(("PyObjFuncDef::handleCall...\n"));
    //DMSG(("PyObjFuncDef::handleCall2...\n"));
    //return funcASTPtr.cast<FuncCodeImpl>()->exeCode(context, args);
    PyObjPtr ret = new PyObjClassObj(self);
    return ret;
}
void PyObjClassDef::dump() {
    DMSG(("%s(class)", classASTPtr->name.c_str()));
}

PyObjPtr PyObjClassFunc::handleCall(PyObjPtr context, list<PyObjPtr>& args){
    //DMSG(("PyObjClassFunc::handleCall...\n"));
    //args.insert(args.begin(), classSelf);
    args.push_front(classSelf);
    return NULL;//funcDefPtr.cast<PyObjFuncDef>()->handleCall(context, args);
}

void PyObjClassObj::dump() {
    DMSG(("%s(classobj)", classDefPtr.cast<PyObjClassDef>()->classASTPtr->name.c_str()));
}

PyObjPtr& PyObjClassObj::getVar(PyObjPtr& self, unsigned int nFieldIndex)
{
    if (nFieldIndex < m_objStack.size()) {
        return this->PyObj::getVar(self, nFieldIndex);
    }

    for (unsigned int i = m_objStack.size(); i <= nFieldIndex; ++i){
        m_objStack.push_back(PyObjTool::buildNULL());
    }
    
    PyObjPtr& ret = m_objStack[nFieldIndex];
    ret = classDefPtr->getVar(classDefPtr, nFieldIndex);
    if (PY_FUNC_DEF == ret->getType()){
        ret = new PyObjClassFunc(self, ret);
    }
    
    return ret;
}

PyObjPtr& PyObjFuncDef::exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal){
    DMSG(("PyObjFuncDef::exeFunc...\n"));
    try
    {
        DMSG(("PyObjFuncDef::exeFunc...%u\n", allArgsVal.size()));
        PyContextBackUp backup(context);
        context.curstack = new PyCallTmpStack(this->getObjIdInfo());

        ParametersExprAST* pParametersExprAST = parameters.cast<ParametersExprAST>();
        unsigned int hasConsumeArg = 0;
        unsigned int hasAssignArgNum = 0;
        for (; hasAssignArgNum < pParametersExprAST->allParam.size(); ++hasAssignArgNum){
            unsigned int j = hasAssignArgNum;
            ParametersExprAST::ParameterInfo& paramInfo = pParametersExprAST->allParam[j];
            
            const string& strVarName = paramInfo.paramKey.cast<VariableExprAST>()->name;
            if (paramInfo.paramType.empty() == false){ //!����ͨ����, *a, **b  �ں��ͳһ���� 
                break;
            }
            else if (allArgsVal.size() <= hasConsumeArg){
                if (paramInfo.paramDefault){
                    PyObjPtr v = paramInfo.paramDefault->eval(context);
                    PyObjPtr& ref = paramInfo.paramKey->eval(context);
                    ref = v;
                    ++hasAssignArgNum;
                }
                else{
                    throw PyException::buildException("need more arg num");
                }
                continue;
            }
            else if (allArgsVal[hasConsumeArg].argType.empty()){//!��ͨ��ֵ�����Ĳ��� f(1, 2, 3)
                PyObjPtr& ref = paramInfo.paramKey->eval(context);
                ref = argAssignVal[hasConsumeArg];
                ++hasConsumeArg;
            }
            else if (allArgsVal[hasConsumeArg].argType == "="){//!��������  f(a=1, b=2, c=3)
                //!�����������Ǿ�����������ߵ�ȫ�����Ǿ���������
                unsigned int fromIndex = hasConsumeArg;
                //!�ȼ��һ�飬����о���������û�ж��屨����
                for (unsigned int n = fromIndex; n < allArgsVal.size(); ++n){
                    ArgTypeInfo& argInfo = allArgsVal[n];
                    if (argInfo.argType == "="){
                        bool bFind = false;
                        for (unsigned int m = j; m < pParametersExprAST->allParam.size(); ++m){
                            const string& strArgName = pParametersExprAST->allParam[m].paramKey.cast<VariableExprAST>()->name;
                            if (strArgName == argInfo.argKey){
                                bFind = true;
                                break;
                            }
                        }
                        if (bFind){
                            continue;
                        }
                        throw PyException::buildException("got an unexpected keyword argument");
                        break;
                    }
                }
                
                for (unsigned int m = j; m < pParametersExprAST->allParam.size(); ++m){
                    paramInfo = pParametersExprAST->allParam[m];
                    if (paramInfo.paramType.empty() == false){ //!����ͨ����, *a, **b  �ں��ͳһ���� 
                        break;
                    }
                    const string& strArgName = paramInfo.paramKey.cast<VariableExprAST>()->name;
                    
                    bool hitFlag = false;
                    for (unsigned int n = fromIndex; n < allArgsVal.size(); ++n){
                        ArgTypeInfo& argInfo = allArgsVal[n];
                        if (argInfo.argKey == strArgName){//!hit
                            PyObjPtr& ref = paramInfo.paramKey->eval(context);
                            ref = argAssignVal[n];
                            hitFlag = true;
                            ++hasAssignArgNum;
                            break;
                        }
                    }
                    if (hitFlag){
                        continue;
                    }
                    else if(paramInfo.paramDefault){//!�����Ĭ��ֵ 
                        PyObjPtr v = paramInfo.paramDefault->eval(context);
                        PyObjPtr& ref = paramInfo.paramKey->eval(context);
                        ref = v;
                        ++hasAssignArgNum;
                    }
                    else{
                        throw PyException::buildException("need more arg num");
                    }
                }
                break;
            }
            else{
                throw PyException::buildException("arg not assign value");
            }
        }
        
        //!����* **����� 
        for (; hasAssignArgNum < pParametersExprAST->allParam.size(); ++hasAssignArgNum){
            unsigned int j = hasAssignArgNum;
            ParametersExprAST::ParameterInfo& paramInfo = pParametersExprAST->allParam[j];
            
            const string& strVarName = paramInfo.paramKey.cast<VariableExprAST>()->name;
            if (paramInfo.paramType == "*"){
                PyObjPtr pVal = new PyObjTuple();
                PyObjPtr& ref = paramInfo.paramKey->eval(context);
                ref = pVal;
                
                for (unsigned int m = hasConsumeArg; m < allArgsVal.size(); ++m){
                    ArgTypeInfo& argInfo = allArgsVal[m];
                    if (argInfo.argType == "=" || argInfo.argType == "**"){
                        break;
                    }
                    pVal.cast<PyObjTuple>()->value.push_back(argAssignVal[m]);
                }
                continue;
            }
            else if (paramInfo.paramType == "**"){
                PyObjPtr pVal = new PyObjDict();
                PyObjPtr& ref = paramInfo.paramKey->eval(context);
                ref = pVal;
                
                for (unsigned int m = hasConsumeArg; m < allArgsVal.size(); ++m){
                    ArgTypeInfo& argInfo = allArgsVal[m];
                    if (argInfo.argType != "="){
                        throw PyException::buildException("given more arg");
                    }
                    const string& keyName = argInfo.argKey;
                    PyObjPtr tmpKey = new PyObjStr(keyName);
                    pVal.cast<PyObjDict>()->value[tmpKey] = argAssignVal[m];
                }
                continue;
            }
        }
        suite->eval(context);
    }
    catch(ReturnSignal& s){
        return context.getCacheResult();
    }
    
    return context.cacheResult(PyObjTool::buildNone());
}

