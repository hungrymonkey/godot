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
void HilbertHotel::add(int value) {
	Dictionary d;
}

void HilbertHotel::reset() {
}

int HilbertHotel::get_total() const {
	return 0;
}
Variant HilbertHotel::parse_dict() const {
	char * str = "{\n\"p_name\": {\n1: \"boys\",\n617863994: \"girls\"\n},\n\"spawn_points\": {\n1: 0,\n617863994: 1\n}\n}";
	VariantParser::StreamString ss;
	VariantParser parser;
	String err_string;
	Variant ret;
	int err_line;
	ss.s = String(str);
	Error err_ret = parser.parse(&ss, ret, err_string, err_line);
	print_line("variant type is : " + itos(ret.get_type()));
	return ret;
}

void HilbertHotel::_bind_methods() {

    ClassDB::bind_method(D_METHOD("add", "value"), &HilbertHotel::add);
    ClassDB::bind_method(D_METHOD("reset"), &HilbertHotel::reset);
    ClassDB::bind_method(D_METHOD("get_total"), &HilbertHotel::get_total);
    ClassDB::bind_method(D_METHOD("parse_dict"), &HilbertHotel::parse_dict);
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

void _HilbertHotel::_bind_methods() {

    ClassDB::bind_method(D_METHOD("create_bus"), &HilbertHotel::create_bus);
	ADD_SIGNAL(MethodInfo("occupy_room", PropertyInfo(Variant::INT, "room_number"), PropertyInfo(Variant::_RID, "r_id")));
}
_HilbertHotel::_HilbertHotel() {
	singleton = this;
}