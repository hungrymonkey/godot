
/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "engine.h"

#include "talking_tree.h"
#include "sdl2_audiocapture.h"
#include "audio_stream_talking_tree.h"

static SDL2AudioCapture *SDL2_audio_capture = NULL;
static TalkingTree *talking_tree = NULL;
static _TalkingTree *_talking_tree = NULL;

void register_talkingtree_types() {
	
	ClassDB::register_class<AudioStreamTalkingTree>();
	SDL2_audio_capture = memnew(SDL2AudioCapture);
	SDL2_audio_capture->init();
	SDL2_audio_capture->set_singleton();

	talking_tree = memnew(TalkingTree);
	talking_tree->init();
	_talking_tree = memnew(_TalkingTree);
	ClassDB::register_class<_TalkingTree>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("TalkingTree", _TalkingTree::get_singleton()));

}

void unregister_talkingtree_types() {
	//nothing to do here
	SDL2_audio_capture->finish();

	talking_tree->finish();
}
