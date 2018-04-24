/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "hilbert_hotel.h"
#include "engine.h"

static HilbertHotel *hilbert_hotel = NULL;
static _HilbertHotel *_hilbert_hotel = NULL;


void register_hilbert_hotel_types() {
        hilbert_hotel = memnew(HilbertHotel);
        hilbert_hotel->init();
        _hilbert_hotel = memnew(_HilbertHotel);
        ClassDB::register_class<_HilbertHotel>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("HilbertHotel", _HilbertHotel::get_singleton()));
        HilbertHotel::get_singleton()->connect("occupy_room", _HilbertHotel::get_singleton(), "_occupy_room");
}

void unregister_hilbert_hotel_types() {
        HilbertHotel::get_singleton()->disconnect("occupy_room", _HilbertHotel::get_singleton(), "_occupy_room");
        memdelete(hilbert_hotel);
        memdelete(_hilbert_hotel);
   //nothing to do here
}

