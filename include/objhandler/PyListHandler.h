#ifndef _PY_LIST_HANDLER_H_
#define _PY_LIST_HANDLER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <vector>
#include <stdexcept>

#include "Base.h"
#include "singleton.h"

namespace ff {

class PyListHandler: public PyObjHandler{
public:
    virtual int getType() const {
        return PY_LIST;
    }
    virtual std::string handleStr(PyContext& context, const PyObjPtr& self) const;
    virtual bool handleBool(PyContext& context, const PyObjPtr& self) const;
    virtual bool handleEqual(PyContext& context, const PyObjPtr& self, const PyObjPtr& val) const;
    virtual bool handleLessEqual(PyContext& context, const PyObjPtr& self, const PyObjPtr& val) const;
    virtual bool handleGreatEqual(PyContext& context, const PyObjPtr& self, const PyObjPtr& val) const;
    
    virtual PyObjPtr& handleAdd(PyContext& context, PyObjPtr& self, PyObjPtr& val);
    virtual PyObjPtr& handleSub(PyContext& context, PyObjPtr& self, PyObjPtr& val);
    virtual PyObjPtr& handleMul(PyContext& context, PyObjPtr& self, PyObjPtr& val);
    virtual PyObjPtr& handleDiv(PyContext& context, PyObjPtr& self, PyObjPtr& val);
    virtual PyObjPtr& handleMod(PyContext& context, PyObjPtr& self, PyObjPtr& val);
    virtual long handleLen(PyContext& context, PyObjPtr& self);
    virtual PyObjPtr& handleSlice(PyContext& context, PyObjPtr& self, int start, int* stop, int step);
};


}
#endif


