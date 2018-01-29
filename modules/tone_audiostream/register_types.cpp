/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "mytone_audiostream.h"

void register_tone_audiostream_types() {

	ClassDB::register_class<AudioStreamMyTone>();
}

void unregister_tone_audiostream_types() {
	//nothing to do here
}
