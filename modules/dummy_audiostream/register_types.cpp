/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "dummy_audiostream.h"

void register_dummy_audiostream_types() {

        ClassDB::register_class<AudioStreamDummy>();
}

void unregister_dummy_audiostream_types() {
   //nothing to do here
}
