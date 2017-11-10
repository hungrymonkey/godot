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
}

void AudioStreamPlaybackDummy::start(float p_from_pos){
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
	int16_t * buf = (int16_t * )pcm_buffer;
    base->gen_tone(buf, p_frames);

	for(int i = 0;  i < p_frames; i++){
		float sample =  float(buf[i])/32767.0;
		//print_line("0: " + rtos(sample));
		p_buffer[i] = AudioFrame(sample, sample);
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
    return 0.0;
}
bool AudioStreamPlaybackDummy::is_playing() const {
    return active;
}
AudioStreamDummy::AudioStreamDummy()
    : mix_rate(48000), stereo(false), hz(639) {
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