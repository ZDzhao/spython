#ifndef _PYOBJ_H_
#define _PYOBJ_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <vector>
#include <stdexcept>
/*
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif
*/

#include "Base.h"
#include "singleton.h"

#include "objhandler/PyIntHandler.h"
#include "objhandler/PyFloatHandler.h"
#include "objhandler/PyBoolHandler.h"
#include "objhandler/PyFuncHandler.h"
#include "objhandler/PyTupleHandler.h"
#include "objhandler/PyDictHandler.h"
#include "objhandler/PyStrHandler.h"
#include "objhandler/PyClassHandler.h"
#include "objhandler/PyClassInstanceHandler.h"
#include "objhandler/PyModHandler.h"
#include "objhandler/PyBuiltinTypeHandler.h"

namespace ff {

#define THROW_EVAL_ERROR(X) throw PyException::buildException(X)
struct PyObjBuiltinTool{
    static PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e, const std::string& strType);
};
class PyObjInt:public PyObj {
public:
    long value;
    PyObjInt(long n = 0):value(n) {
        this->handler = singleton_t<PyIntHandler>::instance_ptr();
    }
    virtual void dump() {
        DMSG(("%ld(int)", value));
    }
    
    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjInt> >::instance_ptr()->objInfo;
    }
    PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e){
        return PyObjBuiltinTool::getVar(context, self, nFieldIndex, e, "int");
    }
};

class PyObjFloat:public PyObj {
public:
    double value;
    PyObjFloat(double n = 0):value(n) {
        this->handler = singleton_t<PyFloatHandler>::instance_ptr();
    }
    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjFloat> >::instance_ptr()->objInfo;
    }
    PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e){
        return PyObjBuiltinTool::getVar(context, self, nFieldIndex, e, "float");
    }
};
class PyObjBool:public PyObj {
public:
    bool value;
    PyObjBool(bool n = false):value(n) {
        this->handler = singleton_t<PyBoolHandler>::instance_ptr();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjBool> >::instance_ptr()->objInfo;
    }
    PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e){
        return PyObjBuiltinTool::getVar(context, self, nFieldIndex, e, "bool");
    }
};
typedef SmartPtr<PyObjBool> PyObjBoolPtr;

class PyObjStr:public PyObj {
public:
    std::string value;
    PyObjStr(const std::string& v):value(v) {
        this->handler = singleton_t<PyStrHandler>::instance_ptr();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjStr> >::instance_ptr()->objInfo;
    }
    PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e){
        return PyObjBuiltinTool::getVar(context, self, nFieldIndex, e, "str");
    }
};


class PyObjModule:public PyObj {
public:
    enum {
        MOD_LOADING = 0,
        MOD_LOADOK
    };
    static PyObjPtr BuildModule(PyContext& context, const std::string& s, const std::string& p);
    PyObjModule(const std::string& v, std::string p = "built-in"):loadFlag(MOD_LOADING), moduleName(v),path(p) {
        this->handler = singleton_t<PyModHandler>::instance_ptr();
        selfObjInfo = singleton_t<ObjIdTypeTraits<PyObjModule> >::instance_ptr()->objInfo;
        //!different function has different object id 
        selfObjInfo.nObjectId = singleton_t<ObjFieldMetaData>::instance_ptr()->allocObjId();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return selfObjInfo;
    }
public:
    int                             loadFlag; //!0 loading 1 load ok
    std::string                     moduleName;
    std::string                     path;
    ObjIdInfo                       selfObjInfo;  
};

class PyObjTuple:public PyObj {
public:
    PyObjTuple(){
        this->handler = singleton_t<PyTupleHandler>::instance_ptr();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjTuple> >::instance_ptr()->objInfo;
    }
    PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e){
        return PyObjBuiltinTool::getVar(context, self, nFieldIndex, e, "tuple");
    }
public:
    std::vector<PyObjPtr> value;
};

class PyObjDict:public PyObj {
public:
    struct HashUtil{
        std::size_t operator()(const PyObjPtr& a) const {
            return size_t(a.get());
        }
    };
    
    //map�ıȽϺ���
    struct cmp_key
    {
        bool operator()(const PyObjPtr &k1, const PyObjPtr &k2)const
        {
            std::size_t a = k1->getHandler()->handleHash(k1);
            std::size_t b = k2->getHandler()->handleHash(k2);
            return a < b;
        }
    };
    PyObjDict(){
        this->handler = singleton_t<PyDictHandler>::instance_ptr();
    }
    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyObjTuple> >::instance_ptr()->objInfo;
    }
    typedef std::map<PyObjPtr, PyObjPtr, cmp_key> DictMap;
    DictMap value;
};

class PyCppFunc{
public:
    virtual ~PyCppFunc(){
    }
    
    virtual PyObjPtr& exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal) = 0;
};
typedef SmartPtr<PyCppFunc> PyCppFuncPtr;

class PyObjFuncDef:public PyObj {
public:
    PyObjFuncDef(const std::string& n, ExprASTPtr p, ExprASTPtr s, PyCppFuncPtr f = NULL):name(n), parameters(p), suite(s), pyCppfunc(f){
        selfObjInfo = singleton_t<ObjIdTypeTraits<PyObjFuncDef> >::instance_ptr()->objInfo;
        //!different function has different object id 
        selfObjInfo.nObjectId = singleton_t<ObjFieldMetaData>::instance_ptr()->allocObjId();
        this->handler = singleton_t<PyFuncHandler>::instance_ptr();
    }
    PyObjFuncDef(const std::string& n):name(n){
        this->handler = singleton_t<PyFuncHandler>::instance_ptr();
    }
    //PyObjPtr& exeFunc(PyContext& context, PyObjPtr& self, ExprASTPtr& arglist);
    void processParam(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal);
    PyObjPtr& exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal);
    
    virtual const ObjIdInfo& getObjIdInfo(){
        return selfObjInfo;
    }
    
    PyObjPtr forkClassFunc(PyObjPtr& obj){
        PyObjFuncDef* ret  = new PyObjFuncDef(name);
        ret->selfObjInfo   = selfObjInfo;
        ret->parameters    = parameters;
        ret->suite         = suite;
        ret->classInstance = obj;
        ret->pyCppfunc     = pyCppfunc;
        return ret;
    }
    bool hasSelfParam();
    
    std::string     name;
    ExprASTPtr      parameters;
    ExprASTPtr      suite;
    ObjIdInfo       selfObjInfo;
    PyObjPtr        classInstance;
    
    PyCppFuncPtr    pyCppfunc;
};

class PyObjClassDef:public PyObj {
public:
    static PyObjPtr build(PyContext& context, const std::string& s, ObjIdInfo* p = NULL);
    static PyObjPtr build(PyContext& context, const std::string& s, std::vector<PyObjPtr>& parentClass);
    PyObjClassDef(const std::string& s, ObjIdInfo* p = NULL);
    virtual const ObjIdInfo& getObjIdInfo(){
        return selfObjInfo;
    }
    void processInheritInfo(PyContext& context, PyObjPtr& self);
    PyObjPtr& getVar(PyContext& pc, PyObjPtr& self2, unsigned int nFieldIndex, ExprAST* e);
    
    std::string             name;
    std::vector<PyObjPtr>   parentClass;
    ObjIdInfo               selfObjInfo;
    ExprAST*                expr__class__;
};

class PyObjClassInstance:public PyObj {
public:
    PyObjClassInstance(PyObjPtr& v):classDefPtr(v){
        this->handler = singleton_t<PyClassInstanceHandler>::instance_ptr();
        
        selfObjInfo = classDefPtr->getObjIdInfo();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return classDefPtr.cast<PyObjClassDef>()->getObjIdInfo();
    }
    virtual int getFieldNum() const { 
        return std::max(m_objStack.size(), classDefPtr->m_objStack.size()); 
    }
    virtual PyObjPtr& getVar(PyContext& context, PyObjPtr& self, unsigned int nFieldIndex, ExprAST* e);
    
    virtual PyObjPtr& assignToField(PyContext& context, PyObjPtr& self, ExprASTPtr& fieldName, PyObjPtr& v); //!special process field assign
    
    
    PyObjPtr            classDefPtr;
    ObjIdInfo           selfObjInfo;
};

class PyBuiltinTypeInfo:public PyObj {
public:
    PyBuiltinTypeInfo(int n):nType(n){
        this->handler = singleton_t<PyBuiltinTypeHandler>::instance_ptr();
    }

    virtual const ObjIdInfo& getObjIdInfo(){
        return singleton_t<ObjIdTypeTraits<PyBuiltinTypeInfo> >::instance_ptr()->objInfo;
    }

    int nType;
};


class PyCppFuncWrap: public PyCppFunc{
public:
    typedef PyObjPtr  (*PyCppFunc)(PyContext& context, std::vector<PyObjPtr>& argAssignVal);
    
    PyCppFuncWrap(PyCppFunc f):cppFunc(f){
    }
    virtual PyObjPtr& exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal){
        if (cppFunc){
            PyObjPtr v = (*cppFunc)(context, argAssignVal);
            return context.cacheResult(v);
        }
        return context.cacheResult(PyObjTool::buildNone());
    }
public:
    PyCppFunc     cppFunc;
};
class PyCppClassFuncWrap: public PyCppFunc{
public:
    typedef PyObjPtr  (*PyCppFunc)(PyContext& context, PyObjPtr& self, std::vector<PyObjPtr>& argAssignVal);
    
    PyCppClassFuncWrap(PyCppFunc f):cppFunc(f){
    }
    virtual PyObjPtr& exeFunc(PyContext& context, PyObjPtr& self, std::vector<ArgTypeInfo>& allArgsVal, std::vector<PyObjPtr>& argAssignVal){
        if (cppFunc){
            PyObjPtr& selfobj = self.cast<PyObjFuncDef>()->classInstance;
            if (!selfobj){
                throw PyException::buildException("arg self not assigned");
            }
            PyObjPtr v = (*cppFunc)(context, selfobj, argAssignVal);
            return context.cacheResult(v);
        }
        return context.cacheResult(PyObjTool::buildNone());
    }
public:
    PyCppFunc     cppFunc;
};

struct PyCppUtil{
    static std::string strFormat(const char * format, ...);
    static void pyAssert(PyObjPtr& v, int nType){
        if (v->getType() != nType){
            throw PyException::buildException("%d instance needed, given:%d", nType, v->getType());
        }
    }
    static PyObjPtr genInt(long n){
        return new PyObjInt(n);
    }
    static PyObjPtr genStr(const std::string& s){
        return new PyObjStr(s);
    }
    static PyObjPtr genFunc(PyCppFuncWrap::PyCppFunc f, std::string n = ""){
        return new PyObjFuncDef(n, NULL, NULL, new PyCppFuncWrap(f));
    }
    static PyObjPtr genFunc(PyCppClassFuncWrap::PyCppFunc f, std::string n = ""){
        return new PyObjFuncDef(n, NULL, NULL, new PyCppClassFuncWrap(f));
    }
    static PyObjPtr getAttr(PyContext& context, PyObjPtr& obj, const std::string& filedname);
    static void setAttr(PyContext& context, PyObjPtr& obj, const std::string& fieldName, PyObjPtr v);
    
    static std::map<std::string, PyObjPtr> getAllFieldData(PyObjPtr obj);
    static PyObjPtr& callPyfunc(PyContext& context, PyObjPtr& func, std::vector<PyObjPtr>& args);
};
#define PyCheckNone(x) (x->getType() == PY_NONE)
#define PyCheckInt(x) (x->getType() == PY_INT)
#define PyAssertInt(x) PyCppUtil::pyAssert(x, PY_INT)
#define PyCheckFloat(x) (x->getType() == PY_FLOAT)
#define PyAssertFloat(x) PyCppUtil::pyAssert(x, PY_FLOAT)
#define PyCheckStr(x) (x->getType() == PY_STR)
#define PyAssertStr(x) PyCppUtil::pyAssert(x, PY_STR)
#define PyCheckBool(x) (x->getType() == PY_BOOL)
#define PyAssertBool(x) PyCppUtil::pyAssert(x, PY_BOOL)

#define PyCheckFunc(x) (x->getType() == PY_FUNC_DEF)
#define PyAsserFunc(x) PyCppUtil::pyAssert(x, PY_FUNC_DEF)

#define PyCheckModule(x) (x->getType() == PY_MOD)
#define PyAsserModule(x) PyCppUtil::pyAssert(x, PY_MOD)

#define PyCheckTuple(x) (x->getType() == PY_TUPLE)
#define PyAssertTuple(x) PyCppUtil::pyAssert(x, PY_TUPLE)
#define PyCheckList(x) (x->getType() == PY_LIST)
#define PyAssertList(x) PyCppUtil::pyAssert(x, PY_LIST)
#define PyCheckDict(x) (x->getType() == PY_DICT)
#define PyAssertDict(x) PyCppUtil::pyAssert(x, PY_DICT)

#define PY_RAISE_STR(context, v) context.cacheResult(PyCppUtil::genStr(v)); throw PyExceptionSignal()
}
#endif


