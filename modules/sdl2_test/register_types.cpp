
#include "register_types.h"
#include "class_db.h"
#include "sdl2_audiocapture.h"


static SDL2AudioCapture *SDL2_audio_capture = NULL;

void register_sdl2_test_types(){
	SDL2_audio_capture = memnew(SDL2AudioCapture);
	SDL2_audio_capture->init();
	SDL2_audio_capture->set_singleton();
}
void unregister_sdl2_test_types(){
	SDL2_audio_capture->finish();
	if(SDL2_audio_capture)
		memdelete(SDL2_audio_capture);
}
/* yes, the word in the middle must be the same as the module folder name */
