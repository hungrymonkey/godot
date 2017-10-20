/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "mumble.h"
#include "callback.h"
#include "audio_stream_mumble.h"


void register_mumble_types() {

	ClassDB::register_class<Mumble>();
	ClassDB::register_class<AudioStreamMumble>();
	ClassDB::register_class<AudioStreamPlaybackMumble>();
}

void unregister_mumble_types() {
   //nothing to do here
}

