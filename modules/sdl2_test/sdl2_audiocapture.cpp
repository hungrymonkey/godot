#include "sdl2_audiocapture.h"
#include "os/os.h"


SDLDevice::SDLDevice(int sample_rate, int format, int frame_size) {
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		ERR_PRINTS("Couldn't initialize SDL: " + String(SDL_GetError()));
	}
	switch(format){
		case SDL2AudioCapture::FORMAT_8_BIT:
			wanted.format = AUDIO_S8;
			break;
		case SDL2AudioCapture::FORMAT_16_BIT:
			wanted.format = AUDIO_S16SYS;
			break;
		case SDL2AudioCapture::FORMAT_FLOAT:
			wanted.format = AUDIO_F32SYS;
			break;
	}
	this->format = format;
	wanted.freq = sample_rate;
	wanted.channels = 1;
	wanted.samples = frame_size;
	wanted.callback = NULL;
	devid_in = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &wanted, 0);
	if (!devid_in) {
		ERR_PRINTS("Couldn't open an audio device for capture: " + String(SDL_GetError()));
	}
}
SDLDevice::~SDLDevice(){
	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
	SDL_CloseAudioDevice(devid_in);
}

void SDLDevice::talk(){
	SDL_PauseAudioDevice(devid_in, SDL_FALSE);
}
bool SDLDevice::recording(){
	return SDL_GetAudioDeviceStatus(devid_in) == SDL_AUDIO_PLAYING;
}
int SDLDevice::get_available_frames() const {
	return SDL_GetQueuedAudioSize(devid_in)/wanted.samples/format;
}
void SDLDevice::mute(){
	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
	SDL_ClearQueuedAudio(devid_in);
}
PoolByteArray SDLDevice::get_frame() {
	int8_t buf[wanted.samples * format];
	uint32_t bytes = SDL_DequeueAudio( devid_in, buf, sizeof(buf));
	PoolByteArray pcm;
	pcm.resize(sizeof(buf));
	PoolVector<uint8_t>::Write w = pcm.write();
	copymem(w.ptr(), buf, sizeof(buf));
	return pcm;
}
SDL2AudioCapture *SDL2AudioCapture::singleton = NULL;
SDL2AudioCapture *SDL2AudioCapture::get_singleton() {
	return singleton;
}
void SDL2AudioCapture::thread_func(void *p_udata){
	SDL2AudioCapture *ac = (SDL2AudioCapture *) p_udata;

	uint64_t usdelay = 20000;
	while(!ac -> exit_thread){
		/*
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
		}*/
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void SDL2AudioCapture::_bind_methods() {
	ADD_SIGNAL(MethodInfo("emit_pcm", PropertyInfo(Variant::POOL_BYTE_ARRAY, "audio_frame")));
}
Error SDL2AudioCapture::init(){
	thread_exited = false;
	mutex = Mutex::create();
	thread = Thread::create(SDL2AudioCapture::thread_func, this);
	return OK;
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
	
	memdelete(thread);
	if (mutex)
		memdelete(mutex);
	thread = NULL;
}
SDL2AudioCapture::SDL2AudioCapture() {
	mutex = NULL;
	thread = NULL;
	singleton = this;
}

RID SDL2AudioCapture::create(int sample_rate, int format, int frame_size){
	SDLDevice *dev = memnew(SDLDevice(sample_rate, format, frame_size));
	return mic_owner.make_rid(dev);
}
void SDL2AudioCapture::destroy(RID dev) {
	mic_owner.free(dev);
}
_SDL2AudioCapture *_SDL2AudioCapture::get_singleton() {
	return singleton;
}

_SDL2AudioCapture *_SDL2AudioCapture::singleton = NULL;

_SDL2AudioCapture::_SDL2AudioCapture() {
	singleton = this;
	SDL2AudioCapture::get_singleton()->connect("emit_pcm", this, "_emit_pcm");
}
void _SDL2AudioCapture::_emit_pcm(PoolByteArray pcm) {
	emit_signal("emit_pcm", pcm);
}
RID _SDL2AudioCapture::create(int sample_rate, int format, int frame_size){
	return SDL2AudioCapture::get_singleton()->create(sample_rate, format, frame_size);
}
void _SDL2AudioCapture::destroy(RID device) {
	SDL2AudioCapture::get_singleton()->destroy(device);
}
void _SDL2AudioCapture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "sample_rate", "format", "frame_size"), &_SDL2AudioCapture::create);
	ClassDB::bind_method(D_METHOD("destroy", "rid"), &_SDL2AudioCapture::destroy);

	ClassDB::bind_method(D_METHOD("_emit_pcm"), &_SDL2AudioCapture::_emit_pcm);
	ADD_SIGNAL(MethodInfo("emit_pcm", PropertyInfo(Variant::POOL_BYTE_ARRAY, "audio_frame")));

}
