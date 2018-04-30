#include "mytone_audiostream_resampled.h"
#include "math/math_funcs.h"
#include "print_string.h"

AudioStreamPlaybackResampledMyTone::AudioStreamPlaybackResampledMyTone() 
    : active(false){
        AudioServer::get_singleton()->lock();
        pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
        zeromem(pcm_buffer, PCM_BUFFER_SIZE);
        AudioServer::get_singleton()->unlock();
}
AudioStreamPlaybackResampledMyTone::~AudioStreamPlaybackResampledMyTone() {
	if (pcm_buffer) {
		AudioServer::get_singleton()->audio_data_free(pcm_buffer);
		pcm_buffer = NULL;
	}
	
}
void AudioStreamPlaybackResampledMyTone::stop(){
	active = false;
	base->reset();
}

void AudioStreamPlaybackResampledMyTone::start(float p_from_pos){
	seek(p_from_pos);
	active = true;
}
void AudioStreamPlaybackResampledMyTone::seek(float p_time){
	float max = get_length();
	if (p_time < 0) {
		p_time = 0;
	}
	base->set_position(uint64_t(p_time * base->mix_rate) << MIX_FRAC_BITS);
}
void AudioStreamPlaybackResampledMyTone::_mix_internal(AudioFrame *p_buffer, int p_frames){
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
float AudioStreamPlaybackResampledMyTone::get_stream_sampling_rate(){
	return float(base->mix_rate);
}
int AudioStreamPlaybackResampledMyTone::get_loop_count() const {
	return 0;
}
float AudioStreamPlaybackResampledMyTone::get_playback_position() const {
	return 0.0;
}
float AudioStreamPlaybackResampledMyTone::get_length() const {
	return 0.0;
}
bool AudioStreamPlaybackResampledMyTone::is_playing() const {
	return active;
}
AudioStreamMyToneResampled::AudioStreamMyToneResampled()
    : mix_rate(48000), stereo(false), hz(639) {
}

Ref<AudioStreamPlayback> AudioStreamMyToneResampled::instance_playback(){
	Ref<AudioStreamPlaybackResampledMyTone> talking_tree;
	talking_tree.instance();
	talking_tree->base = Ref<AudioStreamMyToneResampled>(this);
	return talking_tree;
}

String AudioStreamMyToneResampled::get_stream_name() const {
	return "Dummy";
}
void AudioStreamMyToneResampled::reset() {
	set_position(0);
}
void AudioStreamMyToneResampled::set_position(uint64_t p) {
	pos = p;
}
void AudioStreamMyToneResampled::gen_tone(int16_t * pcm_buf, int size){
	for( int i = 0; i < size; i++){
		pcm_buf[i] = 32767.0 * sin(2.0*Math_PI*double(pos+i)/(double(mix_rate)/double(hz)));
	}
	pos += size;
}
void AudioStreamMyToneResampled::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset"), &AudioStreamMyToneResampled::reset);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamMyToneResampled::get_stream_name);
	
}
