#ifndef _EXPRAST_H_
#define _EXPRAST_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "Base.h"

namespace ff {


/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
public:
    long Val;
    PyObjPtr obj;
    NumberExprAST(long v);
    virtual PyObjPtr& eval(PyContext& context) {
        return obj;
    }
    virtual int getType() {
        return EXPR_INT;
    }
};
class FloatExprAST : public ExprAST {
public:
    double Val;
    PyObjPtr obj;
    FloatExprAST(double v);
    virtual PyObjPtr& eval(PyContext& context) {
        return obj;
    }
    virtual int getType() {
        return EXPR_INT;
    }
};
class StrExprAST : public ExprAST {
public:
    std::string val;
    PyObjPtr obj;
    StrExprAST(const std::string& v) ;
    virtual PyObjPtr& eval(PyContext& context) {
        return obj;
    }
    virtual int getType() {
        return EXPR_STR;
    }
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
public:
    VariableExprAST(const std::string &n) {
        this->name = n;
    }
    virtual PyObjPtr& eval(PyContext& context) {
        return this->getFieldVal(context);
    }
    
    PyObjPtr handleAssign(PyObjPtr context, PyObjPtr val){
        //PyObjPtr& v = this->getFieldVal(context);
        //v = val;
        return val;
    }
    virtual int getType() {
        return EXPR_VAR;
    }
};

struct VariableExprAllocator{

    ExprASTPtr alloc(const std::string& name){
        ExprASTPtr& ret = allVar[name];
        if (!ret){
            ExprASTPtr p = new VariableExprAST(name);
            ret = p;
            //DMSG(("alloc field:%s\n", name.c_str()));
        }
        return ret;
    }
    
    template<typename T>
    ExprASTPtr allocIfNotExist(const std::string& name){
        ExprASTPtr& ret = allVar[name];
        if (!ret){
            ExprASTPtr p = new T();
            ret = p;
            //DMSG(("alloc field:%s\n", name.c_str()));
        }
        return ret;
    }
    std::map<std::string, ExprASTPtr> allVar;
};

class PowerAST : public ExprAST {
public:
    ExprASTPtr                  atom;
    std::vector<ExprASTPtr>     trailer;

public:
    PowerAST() {
        this->name = "power";
    }
    virtual int getType() {
        return EXPR_POWER;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
};
class StmtAST : public ExprAST {

public:
    StmtAST(){
        //DMSG(("build call %s\n", Callee->name.c_str()));
    }
    virtual int getType() {
        return EXPR_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};
class PrintAST : public ExprAST {

public:
    PrintAST(){
    }
    virtual int getType() {
        return EXPR_PRINT_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};
class DelAST : public ExprAST {

public:
    DelAST(ExprASTPtr& v):exprlist(v){
    }
    virtual int getType() {
        return EXPR_DEL_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    ExprASTPtr exprlist;
};
class PassAST : public ExprAST {

public:
    PassAST(){
        this->name = "pass";
    }
    virtual int getType() {
        return EXPR_PASS_STMT;
    }
    virtual PyObjPtr& eval(PyContext& context) {
        return PyObjTool::buildNone();
    }
public:
};
class BreakAST : public ExprAST {
public:
    BreakAST(){
        this->name = "continue";
    }
    virtual int getType() {
        return EXPR_BREAK_STMT;
    }
    virtual PyObjPtr& eval(PyContext& context) {
        throw FlowCtrlSignal(FlowCtrlSignal::BREAK);
        return PyObjTool::buildNone();
    }
public:
};
class ContinueAST : public ExprAST {
public:
    ContinueAST(){
        this->name = "break";
    }
    virtual int getType() {
        return EXPR_CONTINUE_STMT;
    }
    virtual PyObjPtr& eval(PyContext& context) {
        throw FlowCtrlSignal(FlowCtrlSignal::CONTINUE);
        return PyObjTool::buildNone();
    }
public:
};
class ImportAST : public ExprAST {
public:
    ImportAST(ExprASTPtr& p):param(p){
        this->name = "import";
    }
    virtual int getType() {
        return EXPR_IMPORT_STMT;
    }
    virtual PyObjPtr& eval(PyContext& context) {
        return PyObjTool::buildNone();
    }
    virtual std::string dump(int nDepth);
public:
    ExprASTPtr param;
};
/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    enum {
        OP_ASSIGN = 0,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,
        
        OP_EQ,
        OP_LESS,//<
        OP_GREAT,//>
        OP_LESSEQ, //<=
        OP_GREATEQ, //>=
        OP_NOTEQ, //!= not
        OP_IN,
        OP_NOTIN,
        
        OP_OR,
        OP_AND,
    };
    std::string op;
    ExprASTPtr left, right;
    int optype;
public:
    BinaryExprAST(const std::string& o, ExprASTPtr& l, ExprASTPtr& r)
        : op(o), left(l), right(r),optype(-1) {
        this->name = op;

        if (op == "="){
            optype = OP_ASSIGN;
        }
        else if (op == "+"){
            optype = OP_ADD;
        }
        else if (op == "-"){
            optype = OP_SUB;
        }
        else if (op == "*"){
            optype = OP_MUL;
        }
        else if (op == "/"){
            optype = OP_DIV;
        }
        else if (op == "%"){
            optype = OP_MOD;
        }
        else if (op == "==" || op == "is"){
            optype = OP_EQ;
        }
        else if (op == "!=" or op == "not" or op == "<>"){
            optype = OP_NOTEQ;
        }
        else if (op == "<"){
            optype = OP_LESS;
        }
        else if (op == ">"){
            optype = OP_GREAT;
        }
        else if (op == "<="){
            optype = OP_LESSEQ;
        }
        else if (op == ">="){
            optype = OP_GREATEQ;
        }
        else if (op == "in" || op == "is in"){
            optype = OP_IN;
        }
        else if (op == "not in"){
            optype = OP_NOTIN;
        }
        else if (op == "or"){
            optype = OP_OR;
        }
        else if (op == "not in"){
            optype = OP_AND;
        }
    }
    virtual int getType() {
        return EXPR_BIN;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    virtual PyObjPtr& getFieldVal(PyObjPtr& context);
};

class ReturnAST : public ExprAST {

public:
    ReturnAST(ExprASTPtr& v):testlist(v){
    }
    virtual int getType() {
        return EXPR_DEL_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    ExprASTPtr testlist;
};
class RaiseAST : public ExprAST {

public:
    RaiseAST(){
    }
    virtual int getType() {
        return EXPR_RAISE_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};
class GlobalAST : public ExprAST {

public:
    GlobalAST(){
    }
    virtual int getType() {
        return EXPR_GLOBAL_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};
class ExecAST : public ExprAST {

public:
    ExecAST(){
    }
    virtual int getType() {
        return EXPR_EXEC_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};
class AssertAST : public ExprAST {

public:
    AssertAST(){
    }
    virtual int getType() {
        return EXPR_ASSERT_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
public:
    std::vector<ExprASTPtr> exprs;
};

class AugassignAST : public ExprAST {
public:
    std::string op;
    ExprASTPtr left, right;

public:
    AugassignAST(const std::string& o, ExprASTPtr l, ExprASTPtr r)
        : op(o), left(l), right(r) {
        
        this->name = op;
        //DMSG(("BinaryExprAST Op:%s,left=%s,right=%s\n", this->name.c_str(), left->name.c_str(), right->name.c_str()));
    }
    virtual int getType() {
        return EXPR_AUGASSIGN;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    virtual PyObjPtr& getFieldVal(PyContext& context);
};


class FuncCodeImpl: public ExprAST{
public:
    ExprASTPtr         varAstforName;
    std::vector<ExprASTPtr> argsDef;
    std::vector<ExprASTPtr> body;
public:
    FuncCodeImpl(){
        //this->varAstforName = singleton_t<VariableExprAllocator>::instance_ptr()->alloc(p->name);
        //DMSG(("FunctionAST Proto.name=%s\n", proto->name.c_str()));
    }
    virtual PyObjPtr exeCode(PyObjPtr context, std::list<PyObjPtr>&  tmpArgsInput);
    virtual int getType() {
        return EXPR_FUNCDEF;
    }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST: public ExprAST {
public:
    ExprASTPtr codeImplptr;
public:
    /*
    FunctionAST():codeImplptr(new FuncCodeImpl()){
        
        //this->varAstforName = singleton_t<VariableExprAllocator>::instance_ptr()->alloc(p->name);
        //DMSG(("FunctionAST Proto.name=%s\n", proto->name.c_str()));
    }*/
    virtual int getType() {
        return EXPR_FUNCDEF;
    }

    virtual PyObjPtr& eval(PyContext& context);
};

class  ClassCodeImpl: public ExprAST{
public:
    ExprASTPtr         varAstforName;
public:
    ClassCodeImpl(){
        //this->varAstforName = singleton_t<VariableExprAllocator>::instance_ptr()->alloc(p->name);
        //DMSG(("FunctionAST Proto.name=%s\n", proto->name.c_str()));
    }

    virtual int getType() {
        return EXPR_CLASSDEF;
    }
};

/// ClassAST - This class represents a class definition itself.

class ClassAST: public ExprAST {
public:
    ExprASTPtr codeImplptr;
    std::vector<ExprASTPtr> classFieldCode;
public:
    /*ClassAST():codeImplptr(new ClassCodeImpl()){
    }*/
    virtual int getType() {
        return EXPR_CLASSDEF;
    }

    virtual PyObjPtr& eval(PyContext& context);
};

//! if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ['else' ':' suite]
class IfExprAST: public ExprAST {
public:
    std::vector<ExprASTPtr>          ifTest;
    std::vector<ExprASTPtr>          ifSuite;
    ExprASTPtr                       elseSuite;

public:
    IfExprAST(){
        this->name = "if";
    }
    virtual int getType() {
        return EXPR_IF_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};

//! while_stmt: 'while' test ':' suite ['else' ':' suite]
class WhileExprAST: public ExprAST {
public:
    ExprASTPtr          test;
    ExprASTPtr          suite;
    ExprASTPtr          elseSuite;

public:
    WhileExprAST(){
        this->name = "while";
    }
    virtual int getType() {
        return EXPR_WHILE_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};
//! for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
class ForExprAST: public ExprAST {
public:
    ExprASTPtr          exprlist;
    ExprASTPtr          testlist;
    ExprASTPtr          suite;
    ExprASTPtr          elseSuite;

public:
    ForExprAST(){
        this->name = "for";
    }
    virtual int getType() {
        return EXPR_FOR_STMT;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};
//! listmaker: test ( list_for | (',' test)* [','] )
class ListMakerExprAST: public ExprAST {
public:
    std::vector<ExprASTPtr>  test;
    ExprASTPtr               list_for;

public:
    ListMakerExprAST(){
        this->name = "list";
    }
    virtual int getType() {
        return EXPR_LISTMAKER;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};

class DictorsetMakerExprAST: public ExprAST {
public:
    std::vector<ExprASTPtr>  testKey;
    std::vector<ExprASTPtr>  testVal;
    
    std::vector<ExprASTPtr>  test;
    ExprASTPtr               comp_for;

public:
    DictorsetMakerExprAST(){
        this->name = "dict";
    }
    virtual int getType() {
        return EXPR_DICTORSETMAKER;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};

class ParametersExprAST: public ExprAST {
public:
    std::vector<ExprASTPtr>  fpdef;
    std::vector<ExprASTPtr>  test;

public:
    ParametersExprAST(){
        this->name = "parameters";
    }
    virtual int getType() {
        return EXPR_PARAMETERS;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};


class FuncDefExprAST: public ExprAST {
public:
    ExprASTPtr              funcname;
    ExprASTPtr              parameters;
    ExprASTPtr              suite;

public:
    FuncDefExprAST(){
        this->name = "funcdef";
    }
    virtual int getType() {
        return EXPR_FUNCDEF;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};


class ClassDefExprAST: public ExprAST {
public:
    ExprASTPtr              classname;
    ExprASTPtr              testlist;
    ExprASTPtr              suite;

public:
    ClassDefExprAST(){
        this->name = "classdef";
    }
    virtual int getType() {
        return EXPR_CLASSDEF;
    }
    virtual std::string dump(int nDepth);
    virtual PyObjPtr& eval(PyContext& context);
    
};

struct ForBreakContinueFlag{
    ForBreakContinueFlag():flagContinue(false), flagBreak(false){
    }
    bool flagContinue;
    bool flagBreak;
};
class ContinueExprAST : public ExprAST {
public:
    ContinueExprAST(){
    }
    virtual PyObjPtr& eval(PyContext& context) {
        //DMSG(("continue expr eval---------\n"));
        singleton_t<ForBreakContinueFlag>::instance_ptr()->flagContinue = true;
        return context.curstack;
    }
    virtual int getType() {
        return EXPR_CONTINUE_STMT;
    }
};

class BreakExprAST : public ExprAST {
public:
    BreakExprAST(){
    }
    virtual PyObjPtr& eval(PyContext& context) {
        //DMSG(("break expr eval---------\n"));
        singleton_t<ForBreakContinueFlag>::instance_ptr()->flagBreak = true;
        return context.curstack;
    }
    virtual int getType() {
        return EXPR_BREAK_STMT;
    }
};

class ForExprASTOld: public ExprAST {
public:
    ExprASTPtr                  iterTuple;
    ExprASTPtr                  iterFunc;
    std::vector<ExprASTPtr>     forBody;
public:
    ForExprASTOld(){
        this->name = "for";
        //DMSG(("FunctionAST Proto.name=%s\n", proto->name.c_str()));
    }
    virtual int getType() {
        return EXPR_FOR_STMT;
    }
    virtual PyObjPtr& eval(PyContext& context);
};


class TupleExprAST: public ExprAST {
public:
    std::vector<ExprASTPtr>    values;

public:
    TupleExprAST(){
        this->name = "tuple";
        //DMSG(("FunctionAST Proto.name=%s\n", proto->name.c_str()));
    }
    virtual int getType() {
        return EXPR_TUPLE;
    }
    virtual PyObjPtr& eval(PyContext& context);
    
    PyObjPtr handleAssign(PyObjPtr context, PyObjPtr value);
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {

public:
    CallExprAST(ExprASTPtr& c, ExprASTPtr& a)
        : varFuncName(c), argsTuple(a) {
        //DMSG(("build call %s\n", Callee->name.c_str()));
    }
    virtual int getType() {
        return EXPR_CALL;
    }
    virtual PyObjPtr& eval(PyContext& context);
public:
    ExprASTPtr varFuncName;
    ExprASTPtr argsTuple;
};


}
#endif


