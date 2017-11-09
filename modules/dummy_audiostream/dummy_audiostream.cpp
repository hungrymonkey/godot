#include "dummy_audiostream.h"
#include "math/math_funcs.h"

AudioStreamPlaybackDummy::AudioStreamPlaybackDummy() 
    : playing(false){
        AudioServer::get_singleton()->lock();
        pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
        zeromem(pcm_buffer, PCM_BUFFER_SIZE);
        AudioServer::get_singleton()->unlock();
}
void AudioStreamPlaybackDummy::stop(){
    playing = false;
    base->reset();
}

void AudioStreamPlaybackDummy::start(float p_from_pos){
    seek(p_from_pos);
    playing = true;
}
void AudioStreamPlaybackDummy::seek(float p_time){
    float max = get_length();
	if (p_time < 0) {
		p_time = 0;
	}
    base->set_position(uint64_t(p_time * base->mix_rate) << MIX_FRAC_BITS);
}
void AudioStreamPlaybackDummy::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames){
	if (!playing) {
		for (int i = 0; i < p_frames; i++) {
			p_buffer[i] = AudioFrame(0, 0);
		}
		return;
    }
    int64_t offset = 0;
    zeromem(pcm_buffer, PCM_BUFFER_SIZE);
    base->gen_tone((uint16_t * )pcm_buffer, p_frames);
    do_resample<uint16_t, false>((uint16_t * ) pcm_buffer, p_buffer, offset, p_frames, p_frames);
	
}
template <class Depth, bool is_stereo>
void AudioStreamPlaybackDummy::do_resample(const Depth *p_src, AudioFrame *p_dst, int64_t &offset, int32_t &increment, uint32_t amount) {

	// this function will be compiled branchless by any decent compiler

	int32_t final, final_r, next, next_r;
	while (amount--) {
		int64_t pos = offset >> MIX_FRAC_BITS;
		if (is_stereo )
			pos <<= 1;

		final = p_src[pos];
		if (is_stereo)
			final_r = p_src[pos + 1];
		if (sizeof(Depth) == 1) { /* conditions will not exist anymore when compiled! */
			final <<= 8;
			if (is_stereo)
				final_r <<= 8;
		}
		if (is_stereo) {
			next = p_src[pos + 2];
			next_r = p_src[pos + 3];
		} else {
			next = p_src[pos + 1];
		}
		if (sizeof(Depth) == 1) {
			next <<= 8;
			if (is_stereo)
				next_r <<= 8;
		}

		int32_t frac = int64_t(offset & MIX_FRAC_MASK);

		final = final + ((next - final) * frac >> MIX_FRAC_BITS);
		if (is_stereo)
			final_r = final_r + ((next_r - final_r) * frac >> MIX_FRAC_BITS);
	

		if (!is_stereo) {
			final_r = final; //copy to right channel if stereo
		}

		p_dst->l = final / 32767.0;
		p_dst->r = final_r / 32767.0;
		p_dst++;

		offset += increment;
	}
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
    return playing;
}
AudioStreamDummy::AudioStreamDummy()
    : mix_rate(48000), stereo(false), hz(17100) {
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
void AudioStreamDummy::gen_tone(uint16_t * pcm_buf, int size){
    for( int i = 0; i < size; i++){
        pcm_buf[i] = sin(2.0*Math_PI*double(pos+i)/(double(mix_rate)/double(hz)));
    }
    pos += size;
}
void AudioStreamDummy::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset"), &AudioStreamDummy::reset);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamDummy::get_stream_name);
	
}