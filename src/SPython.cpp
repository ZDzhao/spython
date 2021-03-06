
#include "SPython.h"
#include "ExprAST.h"
#include "PyObj.h"

#include "StrTool.h"
#include "ext/PyBaseExt.h"
#include "ext/PyTupleExt.h"
#include "ext/PyListExt.h"
#include "ext/PyDictExt.h"
#include "ext/PyWeakExt.h"
#include "ext/PyTimeExt.h"
#include "ext/PyDatetimeExt.h"
#include "ext/PyJsonExt.h"
#include "ext/PySysExt.h"
#include "ext/PyOsExt.h"
#include "ext/PyStringIOExt.h"
#include "ext/PyMathExt.h"
#include "ext/PyRandomExt.h"
#include "ext/PyIOExt.h"
#include "ext/PyStructExt.h"
#include "ext/PyCopyExt.h"
   
using namespace std;
using namespace ff;
          
SPython::SPython(){
    pycontext.syspath  = new PyObjList();
    pycontext.curstack = PyObjModule::BuildModule(pycontext, "__main__", "built-in");
    pycontext.curstack.cast<PyObjModule>()->loadFlag = PyObjModule::MOD_LOADOK;
    PyBaseExt::init(pycontext);
    PyTupleExt::init(pycontext);
    PyListExt::init(pycontext);
    PyDictExt::init(pycontext);
    PyWeakExt::init(pycontext);
    PyTimeExt::init(pycontext);
    PyDatetimeExt::init(pycontext);
    PyJsonExt::init(pycontext);
    PyOsExt::init(pycontext);
    PySysExt::init(pycontext);
    PyStringIOExt::init(pycontext);
    PyMathExt::init(pycontext);
    PyRandomExt::init(pycontext);
    PyIOExt::init(pycontext);
    PyStructExt::init(pycontext);
    PyCopyExt::init(pycontext);
    
    //pycontext.syspath += "pysrc;;pylib;justfortest;thrift;pylib/thrift/protocol;pylib/thrift/transport";
}

PyObjPtr SPython::importFile(const std::string& modname, string __module__){
    if (__module__.empty()){
        __module__ = modname;
    }
    return PyOpsUtil::importFile(pycontext, modname, __module__);
}


