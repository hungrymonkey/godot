/* summator.cpp */

#include "summator.h"
#include "variant.h"

void Summator::add(int value) {

    count += value;
}

void Summator::reset() {

    count = 0;
}

int Summator::get_total() const {

    return count;
}

void Summator::_bind_methods() {

    ClassDB::bind_method(D_METHOD("add", "value"), &Summator::add);
    ClassDB::bind_method(D_METHOD("reset"), &Summator::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &Summator::get_total);
    ClassDB::bind_method(D_METHOD("setCallback_Obj", "handler"), &Summator::setCallback_Obj);
    ClassDB::bind_method(D_METHOD("setCallback_Ref_FuncRef","handler"), &Summator::setCallback_Ref_FuncRef);
    ClassDB::bind_method(D_METHOD("callCallback","string"), &Summator::setCallback_Ref_FuncRef);

}

Summator::Summator() {
    count = 0;
}

void Summator::setCallback_Ref_FuncRef(  Ref<FuncRef> handler){
    Variant a = Variant("Internal setCallback_Ref_FuncRef message say: quack");
    const Variant *args[1] = {&a};
    Variant::CallError err;

    handler->call_func(args, 1, err);
    this->_cb = handler;
}
void Summator::callCallback(Variant &a){
    const Variant *args[1] = {&a};
    Variant::CallError err;
    _cb->call_func(args, 1, err);
}
void Summator::setCallback_Obj( Object * handler){
    FuncRef *cb = Object::cast_to<FuncRef>(handler);
    Variant a = Variant("Internal setCallback_Obj message say: quack");
    const Variant *args[1] = {&a};
    Variant::CallError err;
    cb->call_func(args, 1, err);
    this->_cb = Ref<FuncRef>(cb);
}
