/* summator.cpp */

#include "hilbert_hotel.h"
#include "variant.h"
#include "os/os.h"
#include "dictionary.h"
#include "prime_225.h"




void HilbertHotel::thread_func(void *p_udata){
	HilbertHotel *ac = (HilbertHotel *) p_udata;

	uint64_t msdelay = 1000;
	while(!ac -> exit_thread){
		if(!ac -> empty()) {
			ac->lock();
			ac->register_rooms();
			ac->unlock();
		}
		OS::get_singleton()->delay_usec(msdelay * 1000);
	}
}

Error HilbertHotel::init(){
	thread_exited = false;
	counter = 0;
	mutex = Mutex::create();
	thread = Thread::create(HilbertHotel::thread_func, this);
	return OK;
}
HilbertHotel *HilbertHotel::singleton = NULL;
HilbertHotel *HilbertHotel::get_singleton() { return singleton; }
void HilbertHotel::register_rooms() {
	List<RID> list;
	bus_owner.get_owned_list(&list);
	for( List<RID>::Element *e = list.front(); e; e = e->next()) {
		auto bus = bus_owner.getornull(e->get());
		if(bus){
			uint64_t room = bus->next_room();
			_emit_occupy_room(room, bus->get_self());
		}	
	}
}

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
void HilbertHotel::_emit_occupy_room(uint64_t room, RID rid) {
	_HilbertHotel::get_singleton()->_occupy_room(room, rid);
}
Variant HilbertHotel::get_bus_info(RID id){
	InfiniteBus * bus = bus_owner.getornull(id);
	if(bus){
		Dictionary d;
		d["prime"] = bus->get_bus_num();
		d["current_room"] = bus->get_current_room();
		return d;
	}
	return Variant();
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
	//lock();
	List<RID> list;
	bus_owner.get_owned_list(&list);
	InfiniteBus *ptr = memnew(InfiniteBus(PRIME[counter++]));
	RID ret = bus_owner.make_rid(ptr);
	ptr->set_self(ret);
	//buses.insert(ptr);
	//unlock();
	return ret;
}
bool HilbertHotel::delete_bus(RID id) {
	if (bus_owner.owns(id)) {
		bus_owner.free(id);
		return true;
	}
	return false;
}
void HilbertHotel::clear() {
	List<RID> list;
	bus_owner.get_owned_list(&list);
	for( List<RID>::Element *e = list.front(); e; e = e->next()) {
		delete_bus(e->get());
	}
}
bool HilbertHotel::empty() {
	List<RID> list;
	bus_owner.get_owned_list(&list);
	return list.size() <= 0;
}
void HilbertHotel::_bind_methods() {
}

HilbertHotel::HilbertHotel() {
	singleton = this;
}



_HilbertHotel *_HilbertHotel::singleton = NULL;
_HilbertHotel *_HilbertHotel::get_singleton() { return singleton; }

RID _HilbertHotel::create_bus() {
	return HilbertHotel::get_singleton()->create_bus();
}
bool _HilbertHotel::delete_bus(RID rid) {
	return HilbertHotel::get_singleton()->delete_bus(rid);
}
void _HilbertHotel::_occupy_room(int room_number, RID bus){
	emit_signal("occupy_room", room_number, bus);
}
Variant _HilbertHotel::get_bus_info(RID id) {
	return HilbertHotel::get_singleton()->get_bus_info(id);
}
void _HilbertHotel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_bus_info", "r_id"), &_HilbertHotel::get_bus_info);
    ClassDB::bind_method(D_METHOD("create_bus"), &_HilbertHotel::create_bus);
	ClassDB::bind_method(D_METHOD("delete_bus"), &_HilbertHotel::delete_bus);
	ADD_SIGNAL(MethodInfo("occupy_room", PropertyInfo(Variant::INT, "room_number"), PropertyInfo(Variant::_RID, "r_id")));
}
void _HilbertHotel::connect_singals() {
	HilbertHotel::get_singleton()->connect("occupy_room", _HilbertHotel::get_singleton(), "_occupy_room");
}
_HilbertHotel::_HilbertHotel() {
	singleton = this;
}
_HilbertHotel::~_HilbertHotel() {
}