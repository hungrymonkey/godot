#include "mytone_audiostream.h"
#include "math/math_funcs.h"
#include "print_string.h"

AudioStreamPlaybackMyTone::AudioStreamPlaybackMyTone() 
    : active(false){
        AudioServer::get_singleton()->lock();
        pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
        zeromem(pcm_buffer, PCM_BUFFER_SIZE);
        AudioServer::get_singleton()->unlock();
}
AudioStreamPlaybackMyTone::~AudioStreamPlaybackMyTone() {
	if(pcm_buffer){
		AudioServer::get_singleton()->audio_data_free(pcm_buffer);
		pcm_buffer = NULL;
	}
}
void AudioStreamPlaybackMyTone::stop(){
	active = false;
	base->reset();
}

void AudioStreamPlaybackMyTone::start(float p_from_pos){
	seek(p_from_pos);
	active = true;
}
void AudioStreamPlaybackMyTone::seek(float p_time){
	float max = get_length();
	if (p_time < 0) {
		p_time = 0;
	}
	base->set_position(uint64_t(p_time * base->mix_rate) << MIX_FRAC_BITS);
}
void AudioStreamPlaybackMyTone::mix(AudioFrame *p_buffer, float p_rate, int p_frames){
	ERR_FAIL_COND(!active);
	if (!active) {
		return;
	}
	zeromem(pcm_buffer, PCM_BUFFER_SIZE);
	int16_t * buf = (int16_t * )pcm_buffer;
	base->gen_tone(buf, p_frames);

	for(int i = 0;  i < p_frames; i++){
		float sample =  float(buf[i])/32767.0;
		p_buffer[i] = AudioFrame(sample, sample);
	}	
}
int AudioStreamPlaybackMyTone::get_loop_count() const {
	return 0;
}
float AudioStreamPlaybackMyTone::get_playback_position() const {
	return 0.0;
}
float AudioStreamPlaybackMyTone::get_length() const {
	return 0.0;
}
bool AudioStreamPlaybackMyTone::is_playing() const {
	return active;
}
AudioStreamMyTone::AudioStreamMyTone()
    : mix_rate(44100), stereo(false), hz(639) {
}

Ref<AudioStreamPlayback> AudioStreamMyTone::instance_playback(){
	Ref<AudioStreamPlaybackMyTone> talking_tree;
	talking_tree.instance();
	talking_tree->base = Ref<AudioStreamMyTone>(this);
	return talking_tree;
}

String AudioStreamMyTone::get_stream_name() const {
	return "Dummy";
}
void AudioStreamMyTone::reset() {
	set_position(0);
}
void AudioStreamMyTone::set_position(uint64_t p) {
	pos = p;
}
void AudioStreamMyTone::gen_tone(int16_t * pcm_buf, int size){
	for( int i = 0; i < size; i++){
		pcm_buf[i] = 32767.0 * sin(2.0*Math_PI*double(pos+i)/(double(mix_rate)/double(hz)));
	}
	pos += size;
}
void AudioStreamMyTone::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset"), &AudioStreamMyTone::reset);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamMyTone::get_stream_name);
	
}
