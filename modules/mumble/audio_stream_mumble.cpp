#include "audio_stream_mumble.h"


AudioStreamPlaybackMumble::AudioStreamPlaybackMumble()
			: offset(0), active(false) {
}
AudioStreamPlaybackMumble::~AudioStreamPlaybackMumble(){
}

void AudioStreamPlaybackMumble::start(float p_from_pos) {
	seek(p_from_pos);
	active = 1;
}
void AudioStreamPlaybackMumble::stop() {
	active = false;
}
bool AudioStreamPlaybackMumble::is_playing() const {
	return active;
}
int AudioStreamPlaybackMumble::get_loop_count() const {
	//times it looped
	return 0;
}
float AudioStreamPlaybackMumble::get_playback_position() const{
	return 0.0;
}
void AudioStreamPlaybackMumble::seek(float p_time){
	//not seeking a stream	
}
float AudioStreamPlaybackMumble::get_length() const {
	//if supported, otherwise return 0
	
	int len = base -> data_bytes;
	switch(base->format){
		case AudioStreamMumble::FORMAT_8_BITS: len /= 1; break;
		case AudioStreamMumble::FORMAT_16_BITS: len /= 2; break;	
	}
	return float(len) / base->mix_rate;
}
void AudioStreamPlaybackMumble::mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames){
	if (!active) {
		for (int i = 0; i < p_frames; i++) {
			p_buffer[i] = AudioFrame(0, 0);
		}
		return;
	}
	int len = base->data_bytes;
	switch (base->format) {
		case AudioStreamSample::FORMAT_8_BITS: len /= 1; break;
		case AudioStreamSample::FORMAT_16_BITS: len /= 2; break;
		case AudioStreamSample::FORMAT_IMA_ADPCM: len *= 2; break;
	}

	if (base->stereo) {
		len /= 2;
	}
	int64_t loop_begin_fp = 0;
	int64_t loop_end_fp = 0;

}
template <class Depth, bool is_stereo>
void AudioStreamPlaybackMumble::do_resample(const Depth *p_src, AudioFrame *p_dst, int64_t &offset, int32_t &increment, uint32_t amount) {

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
void AudioStreamMumble::_bind_methods(){

	ClassDB::bind_method(D_METHOD("set_format", "format"), &AudioStreamMumble::set_format);
	ClassDB::bind_method(D_METHOD("get_format"), &AudioStreamMumble::get_format);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "format", PROPERTY_HINT_ENUM, "8-Bit,16-Bit"), "set_format", "get_format");
	BIND_ENUM_CONSTANT(FORMAT_8_BITS);
	BIND_ENUM_CONSTANT(FORMAT_16_BITS);
}
AudioStreamMumble::AudioStreamMumble()
	: data_bytes(0), mix_rate(44100), format(FORMAT_8_BITS),
	offset(0), stereo(false) {
	AudioServer::get_singleton()->lock();
	data = AudioServer::get_singleton()->audio_data_alloc(BUFFER_SIZE);
	zeromem(data, BUFFER_SIZE);
	AudioServer::get_singleton()->unlock();
	

}
AudioStreamMumble::~AudioStreamMumble(){
}

Ref<AudioStreamPlayback> AudioStreamMumble::instance_playback(){
	Ref<AudioStreamPlaybackMumble> mumble;
	mumble.instance();
	mumble->base = Ref<AudioStreamMumble>(this);
	return mumble;
}

void AudioStreamMumble::append_data(const int16_t * pcm_data, uint32_t size){
	AudioServer::get_singleton()->lock();
	uint8_t *dataptr = (uint8_t *)data;
	if(offset > 0){
		memmove( dataptr + DATA_PAD, offset + dataptr + DATA_PAD, data_bytes ); 
	}
	offset = 0;
	copymem( dataptr + DATA_PAD + data_bytes, pcm_data, size * 2);
	data_bytes += size * 2;
	AudioServer::get_singleton()->unlock();
}

PoolVector<uint8_t> AudioStreamMumble::get_data() const {
	PoolVector<uint8_t> pv;
	if (data_bytes > 0) {
		pv.resize(data_bytes);
		{
			PoolVector<uint8_t>::Write w = pv.write();
			uint8_t *dataptr = (uint8_t *)data;
			copymem(w.ptr(), dataptr + DATA_PAD + offset, data_bytes);
		}
	}
	return pv;
}


String AudioStreamMumble::get_stream_name() const {
	/*
	if (audio_stream.is_valid()) {
		return "Mumble: " + audio_stream->get_name();
	}*/
	return "Mumble";
}
void AudioStreamMumble::set_format(Format p_format) {
	format = p_format;
}
AudioStreamMumble::Format AudioStreamMumble::get_format() const {
	return format;
}
