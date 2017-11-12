/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "dummy_audiostream.h"
#include "sdl2_audiocapture.h"

static SDL2AudioCapture *SDL2_audio_capture = NULL;


void register_dummy_audiostream_types() {
	ClassDB::register_class<AudioStreamDummy>();
	SDL2_audio_capture = memnew(SDL2AudioCapture);
	SDL2_audio_capture->init();
	SDL2_audio_capture->set_singleton();
}

void unregister_dummy_audiostream_types() {
	SDL2_audio_capture->finish();
}
