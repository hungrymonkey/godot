#include "dummy_audiostream.h"
#include "math/math_funcs.h"
#include "print_string.h"
AudioStreamPlaybackDummy::AudioStreamPlaybackDummy() 
    : active(false){
        AudioServer::get_singleton()->lock();
        pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
        zeromem(pcm_buffer, PCM_BUFFER_SIZE);
        AudioServer::get_singleton()->unlock();
}

void AudioStreamPlaybackDummy::stop(){
    active = false;
    base->reset();
//	base->talk();
}

void AudioStreamPlaybackDummy::start(float p_from_pos){
//	base->mute();
    seek(p_from_pos);
    active = true;
}
void AudioStreamPlaybackDummy::seek(float p_time){
    float max = get_length();
	if (p_time < 0) {
		p_time = 0;
	}
    base->set_position(uint64_t(p_time * base->mix_rate) << MIX_FRAC_BITS);
}
void AudioStreamPlaybackDummy::_mix_internal(AudioFrame *p_buffer, int p_frames){
	ERR_FAIL_COND(!active);
	if (!active) {
		return;
    }
    zeromem(pcm_buffer, PCM_BUFFER_SIZE);
	int smaller_buf = base->get_available_bytes()/2 < p_frames ? base->get_available_bytes()/2 : p_frames;
    base->get_data((uint8_t *)pcm_buffer, smaller_buf * 2);
	int16_t * buf = (int16_t * )pcm_buffer;

	for(int i = 0;  i < smaller_buf; i++){
		float sample =  float(buf[i])/32767.0;
	//	print_line("0: " + rtos(sample));
		p_buffer[i] = AudioFrame(sample, sample);
	}
	int todo = p_frames - smaller_buf;
	while(todo){
		p_buffer[p_frames-todo-1] = AudioFrame(0,0);
		todo--;
	}
}
float AudioStreamPlaybackDummy::get_stream_sampling_rate(){
	return float(base->mix_rate);
}
int AudioStreamPlaybackDummy::get_loop_count() const {
    return 0;
}
float AudioStreamPlaybackDummy::get_playback_position() const {
    return 0.0;
}
float AudioStreamPlaybackDummy::get_length() const {
	float a = base->get_available_bytes()/2;
    return a/float(base->mix_rate);
}
bool AudioStreamPlaybackDummy::is_playing() const {
    return active;
}

AudioStreamDummy::AudioStreamDummy()
    : mix_rate(48000), stereo(false), hz(639), pointer(0) {
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		ERR_PRINTS("Couldn't initialize SDL: " + String(SDL_GetError()));
	}
	SDL_AudioSpec wanted;
	SDL_zero(wanted);
	wanted.freq = 48000;
	wanted.format = AUDIO_S16SYS;
	wanted.channels = 1;
	//https://www.opus-codec.org/docs/html_api/group__opusencoder.html#ga88621a963b809ebfc27887f13518c966
	//this is 8 frames or 160 ms
	wanted.samples = 4096;
	wanted.callback = AudioStreamDummy::_sdl_callback;
	wanted.userdata = this;
	//https://wiki.libsdl.org/SDL_OpenAudioDevice#device
	devid_in = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &wanted, 0);
	if (!devid_in) {
		ERR_PRINTS("Couldn't open an audio device for capture: " + String(SDL_GetError()));
	}
	SDL_PauseAudioDevice(devid_in, SDL_FALSE);
}
void AudioStreamDummy::_sdl_callback(void * usr_data, unsigned char * pcm, int len){
	AudioStreamDummy *as = static_cast<AudioStreamDummy*>(usr_data);
//	print_line("callback: "+itos(len));

	as->put_data(pcm, len);	
}
int AudioStreamDummy::get_available_bytes() const{
	return data.size() - pointer;
}
Error AudioStreamDummy::get_data(uint8_t *p_buffer, int p_bytes) {
	int recv;
	get_partial_data(p_buffer, p_bytes, recv);
	if (recv != p_bytes)
		return ERR_INVALID_PARAMETER;
	return OK;
}
void AudioStreamDummy::talk(){
	SDL_PauseAudioDevice(devid_in, SDL_FALSE);
}
void AudioStreamDummy::mute(){
	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
}
Error AudioStreamDummy::get_partial_data(uint8_t *p_buffer, int p_bytes, int &r_received) {
		if (pointer + p_bytes > data.size()) {
			r_received = data.size() - pointer;
			if (r_received <= 0) {
				r_received = 0;
				return OK; //you got 0
			}
		} else {
			r_received = p_bytes;
		}

		PoolVector<uint8_t>::Read r = data.read();
		copymem(p_buffer, r.ptr() + pointer, r_received);

		pointer += r_received;
		// FIXME: return what? OK or ERR_*
		// return OK for now so we don't maybe return garbage
		return OK;
}
Error AudioStreamDummy::put_data(const uint8_t * pcm_data, int p_bytes){
	if (p_bytes <= 0)
		return OK;

	if (pointer + p_bytes > data.size()) {
		data.resize(pointer + p_bytes);
	}

	PoolVector<uint8_t>::Write w = data.write();
	copymem(&w[pointer], pcm_data, p_bytes);	
	emit_signal("audio_recieved");
	return OK;
}
Ref<AudioStreamPlayback> AudioStreamDummy::instance_playback(){
	Ref<AudioStreamPlaybackDummy> talking_tree;
	talking_tree.instance();
	talking_tree->base = Ref<AudioStreamDummy>(this);
	return talking_tree;
}

String AudioStreamDummy::get_stream_name() const {
	return "Dummy";
}
void AudioStreamDummy::reset() {
    set_position(0);
	data.resize(0);
	pointer = 0;
}
void AudioStreamDummy::set_position(uint64_t p) {
    pos = p;
}
void AudioStreamDummy::gen_tone(int16_t * pcm_buf, int size){
    for( int i = 0; i < size; i++){
        pcm_buf[i] = 32767.0 * sin(2.0*Math_PI*double(pos+i)/(double(mix_rate)/double(hz)));
    }
    pos += size;
}
void AudioStreamDummy::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset"), &AudioStreamDummy::reset);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamDummy::get_stream_name);
	
}
