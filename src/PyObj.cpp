
#include "PyObj.h"
#include "ExprAST.h"

using namespace std;
using namespace ff;

bool PyObjFuncDef::hasSelfParam(){
    ParametersExprAST* pParametersExprAST = parameters.cast<ParametersExprAST>();
    if (pParametersExprAST->allParam.empty() == false){
        ParametersExprAST::ParameterInfo& paramInfo = pParametersExprAST->allParam[0];
        if (paramInfo.paramKey->getType() == EXPR_VAR && paramInfo.paramKey.cast<VariableExprAST>()->name == "self"){
            return true;
        }
    }
    return false;
}
PyObjPtr& PyObjClassInstance::assignToField(PyContext& context, PyObjPtr& self, ExprASTPtr& fieldName, PyObjPtr& v){
    PyContextBackUp backup(context);
    context.curstack = self;
    
    unsigned int nFieldIndex = fieldName->getFieldIndex(context);

    for (unsigned int i = m_objStack.size(); i <= nFieldIndex; ++i){
        m_objStack.push_back(PyObjTool::buildNULL());
    }
    
    PyObjPtr& ret = m_objStack[nFieldIndex];
    ret = v;
    return ret;
}
PyObjPtr& PyObjClassInstance::getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex)
{
    if (nFieldIndex < m_objStack.size()) {
        PyObjPtr& ret = m_objStack[nFieldIndex];
        //DMSG(("PyObjClassInstance::getVar...%u %d, %ds", nFieldIndex, ret->getType(), ret->handler->handleStr(ret).c_str()));
        if (false == IS_NULL(ret)){
            if (ret->getType() == EXPR_FUNCDEF){
                return context.cacheResult(ret.cast<PyObjFuncDef>()->forkClassFunc(self));
            }
            return ret;
        }
    }
    
    PyObjPtr& ret = classDefPtr->getVar(context, classDefPtr, nFieldIndex);

    if (false == IS_NULL(ret)){
        if (ret->getType() == EXPR_FUNCDEF){
            return context.cacheResult(ret.cast<PyObjFuncDef>()->forkClassFunc(self));
        }
        return ret;
    }

    for (unsigned int i = m_objStack.size(); i <= nFieldIndex; ++i){
        m_objStack.push_back(PyObjTool::buildNULL());
    }
    
    ret = m_objStack[nFieldIndex];

    return ret;
}

PyObjPtr& PyObjFuncDef::exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal){
    //DMSG(("PyObjFuncDef::exeFunc...\n"));
    try
    {
        //DMSG(("PyObjFuncDef::exeFunc...%u\n", allArgsVal.size()));
        PyContextBackUp backup(context);
        context.curstack = new PyCallTmpStack(this->getObjIdInfo());

        ParametersExprAST* pParametersExprAST = parameters.cast<ParametersExprAST>();
        unsigned int hasConsumeArg = 0;
        unsigned int hasAssignArgNum = 0;
        
        if (classInstance){//!self param
            if (pParametersExprAST->allParam.empty()){
                throw PyException::buildException("too many arg num");
            }
            ParametersExprAST::ParameterInfo& paramInfo = pParametersExprAST->allParam[0];
            
            PyObjPtr& ref = paramInfo.paramKey->eval(context);
            ref = classInstance;
            ++hasAssignArgNum;
        }
        
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
                ++hasAssignArgNum;
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

