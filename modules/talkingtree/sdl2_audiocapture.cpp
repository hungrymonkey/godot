#include "sdl2_audiocapture.h"
#include "os/os.h"


SDL2AudioCapture *SDL2AudioCapture::singleton = NULL;
SDL2AudioCapture *SDL2AudioCapture::get_singleton() {
	return singleton;
}
void SDL2AudioCapture::thread_func(void *p_udata){
	SDL2AudioCapture *ac = (SDL2AudioCapture *) p_udata;

	uint64_t usdelay = 16000;
	while(!ac -> exit_thread){
		if(ac->recording()){
			ac->lock();
			int frames = ac->get_available_frames();
			int16_t buf[SDL2AudioCapture::FRAME_SIZE];
			PoolVector<uint8_t> pcm;
			pcm.resize(sizeof(buf));
			for(int i = 0; i < frames; i++){
				uint32_t bytes = SDL_DequeueAudio( ac->devid_in, buf, sizeof(buf));
				PoolVector<uint8_t>::Write w = pcm.write();
				copymem(w.ptr(), buf, sizeof(buf));
				ac->emit_signal("get_pcm", pcm);
			}
			ac->unlock();
		}
		OS::get_singleton()->delay_usec(usdelay);
	}
}
void SDL2AudioCapture::set_singleton() {
	singleton = this;
}
void SDL2AudioCapture::_bind_methods() {
	ADD_SIGNAL(MethodInfo("get_pcm", PropertyInfo(Variant::POOL_BYTE_ARRAY, "audio_frame")));
}
Error SDL2AudioCapture::init(){
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		ERR_PRINTS("Couldn't initialize SDL: " + String(SDL_GetError()));
	}
	
	wanted.freq = SDL2AudioCapture::SAMPLE_RATE;
	wanted.format = AUDIO_S16SYS;
	wanted.channels = 1;
	wanted.samples = SDL2AudioCapture::FRAME_SIZE;
	wanted.callback = NULL;
	devid_in = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &wanted, 0);
	if (!devid_in) {
		ERR_PRINTS("Couldn't open an audio device for capture: " + String(SDL_GetError()));
	}
	//print_line("device num: " + itos(devid_in));

	thread_exited = false;
	mutex = Mutex::create();
	thread = Thread::create(SDL2AudioCapture::thread_func, this);
	return OK;
}
void SDL2AudioCapture::talk(){
	SDL_PauseAudioDevice(devid_in, SDL_FALSE);
}
bool SDL2AudioCapture::recording(){
	return SDL_GetAudioDeviceStatus(devid_in) == SDL_AUDIO_PLAYING;
}
int SDL2AudioCapture::get_available_frames() const {
	return SDL_GetQueuedAudioSize(devid_in)/SDL2AudioCapture::FRAME_SIZE/2;
}
void SDL2AudioCapture::mute(){
	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
	SDL_ClearQueuedAudio(devid_in);
}
void SDL2AudioCapture::lock() {
	if (!thread || !mutex)
		return;
	mutex->lock();
}

void SDL2AudioCapture::unlock() {

	if (!thread || !mutex)
		return;
	mutex->unlock();
}
void SDL2AudioCapture::finish() {

	if (!thread)
		return;

	exit_thread = true;
	Thread::wait_to_finish(thread);

	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
	SDL_CloseAudioDevice(devid_in);
	memdelete(thread);
	if (mutex)
		memdelete(mutex);
	thread = NULL;
}
SDL2AudioCapture::SDL2AudioCapture() {
	mutex = NULL;
	thread = NULL;
} 