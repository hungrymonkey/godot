/* summator.h */
#ifndef SUMMATOR_H
#define SUMMATOR_H

#include "reference.h"
#include "func_ref.h"
class Summator : public Reference {
    GDCLASS(Summator, Reference);
    
    int count;
    Ref<FuncRef> _cb;
protected:
    static void _bind_methods();

public:
    void add(int value);
    void reset();
    int get_total() const;
    void setCallback_Obj(Object * o);
    void setCallback_Ref_FuncRef(Ref<FuncRef> ref);
    void callCallback(Variant & a);
    Summator();
};

#endif
