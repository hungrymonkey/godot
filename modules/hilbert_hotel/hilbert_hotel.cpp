/* summator.cpp */

#include "hilbert_hotel.h"
#include "prime_225.h"

#include "print_string.h"
#include "variant.h"
#include "variant_parser.h"
#include "ustring.h"
#include "os/os.h"


void HilbertHotel::thread_func(void *p_udata){
	HilbertHotel *ac = (HilbertHotel *) p_udata;

	uint64_t msdelay = 1000;
	while(!ac -> exit_thread){

		
			ac->lock();
			ac->unlock();
		
		OS::get_singleton()->delay_usec(msdelay * 1000);
	}
}

Error HilbertHotel::init(){
	thread_exited = false;
	mutex = Mutex::create();
	thread = Thread::create(HilbertHotel::thread_func, this);
	return OK;
}
HilbertHotel *HilbertHotel::singleton = NULL;
HilbertHotel *HilbertHotel::get_singleton() { return singleton; }


void HilbertHotel::unlock() {
	if (!thread || !mutex)
		return;
	mutex->unlock();
}
void HilbertHotel::lock() {
	if (!thread || !mutex)
		return;
	mutex->lock();
}

void HilbertHotel::finish() {

	if (!thread)
		return;

	exit_thread = true;
	Thread::wait_to_finish(thread);
	
	memdelete(thread);
	if (mutex)
		memdelete(mutex);
	thread = NULL;
}

RID HilbertHotel::create_bus() {
	InfiniteBus *ptr = memnew(InfiniteBus(buses.size()));
	RID ret = bus_owner.make_rid(ptr);
	ptr->set_self(ret);
	buses.insert(ptr);
	return ret;
}

void HilbertHotel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("occupy_room", PropertyInfo(Variant::INT, "room_number"), PropertyInfo(Variant::_RID, "r_id")));
}

HilbertHotel::HilbertHotel() {
	singleton = this;
}



_HilbertHotel *_HilbertHotel::singleton = NULL;
_HilbertHotel *_HilbertHotel::get_singleton() { return singleton; }

RID _HilbertHotel::create_bus() {
	return HilbertHotel::get_singleton()->create_bus();
}
void _HilbertHotel::_occupy_room(int room_number, RID bus){
	emit_signal("occupy_room", room_number, bus);
}

void _HilbertHotel::_bind_methods() {

    ClassDB::bind_method(D_METHOD("create_bus"), &_HilbertHotel::create_bus);
	ADD_SIGNAL(MethodInfo("occupy_room", PropertyInfo(Variant::INT, "room_number"), PropertyInfo(Variant::_RID, "r_id")));
}
_HilbertHotel::_HilbertHotel() {
	singleton = this;
	HilbertHotel::get_singleton()->connect("occupy_room", this, "_occupy_room");
}
_HilbertHotel::~_HilbertHotel() {
	HilbertHotel::get_singleton()->disconnect("occupy_room", this, "_occupy_room");
}